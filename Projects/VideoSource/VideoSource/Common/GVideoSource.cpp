/*
 * Copyright (c) 2015-2023 Pengju Lu, Yanli Wang

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
 
 
#include "../Include/GVideoSource.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"

#include <iostream>


std::string GetErr(int iErrNo)
{
	std::vector<char> vTemp(1024);
	av_make_error_string(&vTemp[0], 1024, iErrNo);
	return std::string(vTemp.begin(),vTemp.end());
}

bool GVideoSource::ReadPacket(AVPacket* pPacket)
{
	while (true)
	{
		int iRet = av_read_frame(m_pFormatCtx, pPacket);
		if (iRet >= 0)
		{
			if (pPacket->stream_index != m_iVideoStream || pPacket->size <= 0)
			{
				continue;
			}

			return true;
		}
		else if (AVERROR_EOF == iRet)
		{
			m_bRunning = false;
			m_eError = G_eVideoSourceErrorEOF;
			GLOGGER(info) << "ReadPacket(): END OF FILE. Stop decoding. ";
			return false;
		}
		else
		{
			GLOGGER(error) << "av_read_frame() FAILED. ret: " << GetErr(iRet);
			return false;
		}
	}
}

int GVideoSource::OutputFrame(const G_tVideoFrame* pVideoFrame)
{
	if (GFrameFormat::CheckFrame(pVideoFrame))
	{
		GFrameFormat::GVideoFrame4Que frame;
		frame.eFrameFormat = pVideoFrame->eFrameFormat;
		frame.iWidth = static_cast<int>(pVideoFrame->uiWidth);
		frame.iHeight = static_cast<int>(pVideoFrame->uiHeight);

		if (m_qFrameQueue.CreateFrame(frame))
		{
			unsigned char* pDst = (unsigned char*)(m_qFrameQueue.InMapFrame());
			g_check_exception(nullptr != pDst, GVideoSourceException());
			GFrameFormat::Frame2Buffer(pVideoFrame, pDst);

			m_qFrameQueue.CompleteFrame();
			return 0;
		}
	}

	return -1;
}

G_enVideoSourceError GVideoSource::Create(const std::string& sURL)
{
	if ("" == sURL)
	{
		return G_eVideoSourceErrorOpenFailed;
	}

	//av_log_set_level(AV_LOG_QUIET);

	//format.
	av_dict_set(&pOpts, "rtsp_transport", "tcp", 0);
	if (avformat_open_input(&m_pFormatCtx, sURL.c_str(), NULL, &pOpts) != 0)
	{
		GLOGGER(error) << "open URL FAILED.";
		return G_eVideoSourceErrorOpenFailed;
	}

	if (avformat_find_stream_info(m_pFormatCtx, NULL) < 0)
	{
		GLOGGER(error) << "av_find_stream_info ERROR\n";
		return G_eVideoSourceErrorOpenFailed;
	}

	m_iVideoStream = -1;
	GLOGGER(info) << "==========> m_pFormatCtx->nb_streams = " << m_pFormatCtx->nb_streams;

	for (int i = 0; i < m_pFormatCtx->nb_streams; i++)
	{
		if (m_pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_iVideoStream = i;
			GLOGGER(info) << "the first video stream index: m_iVideoStream = " << m_iVideoStream;
			break;
		}
	}

	if (m_iVideoStream == -1)
	{
		return G_eVideoSourceErrorOpenFailed;
	}

	pPacket = av_packet_alloc();
    
	//decoder.
	AVCodecParameters* pCodecPar = m_pFormatCtx->streams[m_iVideoStream]->codecpar;
	const AVCodec *pCodec = avcodec_find_decoder(pCodecPar->codec_id);
	if (pCodec == nullptr)
	{
		return G_eVideoSourceErrorOpenFailed;
	}

	pCodecCtx = avcodec_alloc_context3(pCodec);

	if (avcodec_parameters_to_context(pCodecCtx, pCodecPar) < 0)
	{
		GLOGGER(error) << "Could not copy codec parameters to context";
		return G_eVideoSourceErrorOpenFailed;
	}

	if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0)
	{
		return G_eVideoSourceErrorOpenFailed;
	}

	pFrame = av_frame_alloc();
	if (pFrame == nullptr)
	{
		return G_eVideoSourceErrorOpenFailed;
	}

	pFrameDst = av_frame_alloc();
	if (pFrameDst == nullptr)
	{
		return G_eVideoSourceErrorOpenFailed;
	}
	
	m_qFrameQueue.Init(2^25, false);

	m_bRunning = true;
	m_tdDecoder = std::thread(&(GVideoSource::DecodeFunc), (void*)(this));
	return G_eVideoSourceErrorNone;
}

void GVideoSource::DecodeFunc(void* pContext)
{
	GVideoSource* pDecoder = (GVideoSource*)(pContext);

	while (pDecoder->m_bRunning)
	{
		pDecoder->DecodeOneStep();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void GVideoSource::DecodeOneStep()
{
	if(!ReadPacket(pPacket))
	{
		av_packet_unref(pPacket);
		GLOGGER(error) << "read raw stream FAILED. just return.";
		return;
	}
	
	//decode.
	int iRet = avcodec_send_packet(pCodecCtx, pPacket);
	av_packet_unref(pPacket);
	
	if (iRet < 0)
	{
		GLOGGER(error) << "avcodec_send_packet() FAILED. ret: " << GetErr(iRet);
		//Exception. TODO.
		return;
	}

	iRet = avcodec_receive_frame(pCodecCtx, pFrame);
	if (AVERROR(EAGAIN) == iRet)
	{
		return;
	}
	else if(AVERROR_EOF == iRet)
	{
		m_bRunning = false;
		m_eError = G_eVideoSourceErrorEOF;
		GLOGGER(info) << "DecodeOneStep(): END OF FILE. Stop decoding. ";
		return;
	}
	else if (iRet < 0)
	{
		GLOGGER(error) << "avcodec_receive_frame() ERR. ret: " << GetErr(iRet);
		return;
		//Exception. TODO.
	}

	Convert2YUV420p(pFrame, pFrameDst);

	//output frame if one frame was decoded.
	if (pFrameDst->width > 0 && pFrameDst->height > 0 && nullptr != pFrameDst->data[0] && nullptr != pFrameDst->data[1] && nullptr != pFrameDst->data[2])
	{
		G_tVideoFrame videoFrame;
		videoFrame.eFrameFormat = G_enFrameFormatYUV420P;
		videoFrame.pData[0] = pFrameDst->data[0];
		videoFrame.pData[1] = pFrameDst->data[1];
		videoFrame.pData[2] = pFrameDst->data[2];
		videoFrame.pData[3] = nullptr;

		videoFrame.uiLineStep_Byte[0] = pFrameDst->linesize[0];
		videoFrame.uiLineStep_Byte[1] = pFrameDst->linesize[1];
		videoFrame.uiLineStep_Byte[2] = pFrameDst->linesize[2];
		videoFrame.uiLineStep_Byte[3] = 0;

		videoFrame.uiWidth = pCodecCtx->width;
		videoFrame.uiHeight = pCodecCtx->height;

		OutputFrame(&videoFrame);
	}
}

void GVideoSource::Convert2YUV420p(const AVFrame* pFrameSrc, AVFrame* pFrameDst)
{
	if (nullptr == pSwsContext)
	{
		pSwsContext = sws_getContext(pFrameSrc->width, pFrameSrc->height, pCodecCtx->pix_fmt,
									 pFrameSrc->width, pFrameSrc->height, AV_PIX_FMT_YUV420P,
									 SWS_BILINEAR, NULL, NULL, NULL);
		pFrameDst->format = AV_PIX_FMT_YUV420P;
		pFrameDst->width  = pFrameSrc->width;
		pFrameDst->height = pFrameSrc->height;
		int iRet = av_frame_get_buffer(pFrameDst, 32);
		if (iRet < 0)
		{
			GLOGGER(error) << "Error: could not allocate the video frame data.";
			exit(EXIT_FAILURE);
		}
	}

	int iRet = sws_scale(pSwsContext,
						 pFrameSrc->data,
						 pFrameSrc->linesize,
						 0,
						 pFrameSrc->height,
						 pFrameDst->data,
						 pFrameDst->linesize);

	if (iRet < 0)
	{
		GLOGGER(error) << "Error: sws_scale FAILED.";
		return;
	}
	
	return;
}

G_enVideoSourceError GVideoSource::Grab(G_tVideoFrame* pFrame)
{
	memset(pFrame, 0, sizeof(G_tVideoFrame));

	GFrameFormat::GVideoFrame4Que frame;
	if (m_qFrameQueue.GrabFrame(frame))
	{
		unsigned char* pData = (unsigned char*)(m_qFrameQueue.OutMapFrame());
		g_check_exception(nullptr != pData, GVideoSourceException());

		pFrame->eFrameFormat = frame.eFrameFormat;
		pFrame->uiWidth = static_cast<unsigned int>(frame.iWidth);
		pFrame->uiHeight = static_cast<unsigned int>(frame.iHeight);

		pFrame->pData[0] = pData;
		pFrame->pData[1] = pData + frame.iWidth * frame.iHeight;
		pFrame->pData[2] = pData + (frame.iWidth * frame.iHeight * 5) / 4;
		pFrame->pData[3] = nullptr;
		pFrame->uiLineStep_Byte[0] = frame.iWidth;
		pFrame->uiLineStep_Byte[1] = frame.iWidth / 2;
		pFrame->uiLineStep_Byte[2] = frame.iWidth / 2;
		pFrame->uiLineStep_Byte[3] = 0;
	}
	else
	{
		return G_eVideoSourceErrorNotReady;
	}
	
	return m_eError;
}

void GVideoSource::Drop()
{
	m_qFrameQueue.ReleaseFrame();
}

void GVideoSource::Release()
{
	m_bRunning = false;
	m_eError = G_eVideoSourceErrorNone;
	m_tdDecoder.join();

	av_dict_free(&pOpts);
	av_free(pFrame);
	av_free(pFrameDst);
	avcodec_close(pCodecCtx);
	av_packet_free(&pPacket);
	avformat_close_input(&m_pFormatCtx);
}
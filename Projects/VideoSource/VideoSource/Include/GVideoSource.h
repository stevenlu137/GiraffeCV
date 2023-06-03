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
 
 
 
#ifndef G_VIDEO_SOURCE_H_
#define G_VIDEO_SOURCE_H_

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>

extern "C"
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
}


#include "VideoSource/GCVideoSourceErrorIF.h"
#include "Common/FrameFormat/GCFrameFormat.h"
#include "Common/Buffers/GDataFrameQueue.h"
#include "Common/FrameFormat/GFrameFormatUtils.h"


#ifdef WIN32
#define G_VIDEO_SOURCE_CALL_CONVENTION __stdcall
#elif __APPLE__
#define G_VIDEO_SOURCE_CALL_CONVENTION
#elif __ANDROID__
#define G_VIDEO_SOURCE_CALL_CONVENTION
#elif __linux__
#define G_VIDEO_SOURCE_CALL_CONVENTION
#endif

class GVideoSource
{
public:
    GVideoSource():
    m_pFormatCtx(nullptr),
    m_iVideoStream(-1),
    m_eError(G_eVideoSourceErrorNone),
    m_bRunning(false),
    pOpts(nullptr),
    pPacket(nullptr),
    pCodecCtx(nullptr),
    pFrame(nullptr),
    pFrameDst(nullptr),
    pSwsContext(nullptr)
    {}
    
    G_enVideoSourceError Create(const std::string& sURL);
    G_enVideoSourceError Grab(G_tVideoFrame* pFrame);
    void Drop();
    void Release();

private:
    static void DecodeFunc(void* pContext);
	void DecodeOneStep();
	void Convert2YUV420p(const AVFrame* pFrameSrc, AVFrame* pFrameDst);

    bool ReadPacket(AVPacket* pPacket);
	int OutputFrame(const G_tVideoFrame* pVideoFrame);

private:
    AVFormatContext *m_pFormatCtx;
    int m_iVideoStream;

    G_enVideoSourceError m_eError;
    
	std::thread m_tdDecoder;
	bool m_bRunning;

    AVDictionary *pOpts;
    AVPacket* pPacket;

	AVCodecContext *pCodecCtx;
	AVFrame *pFrame;
	AVFrame *pFrameDst;

	SwsContext *pSwsContext;

    GDataFrameQueue<GFrameFormat::GVideoFrame4Que> m_qFrameQueue;
    
};

#endif //G_VIDEO_SOURCE_H_
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
 
 
 
#include "GDataTransfer.h"

#include "opencv2/highgui/highgui.hpp"

#include "Common/Exception/GiraffeException.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"


bool GTextureGroupUploader::Init(GStitcherConfig::enStitcherGLOptionsUploading eUploadingOption
	, enFrameFormat eInputFrameFormat, int iNumOfFrame)
{
	//create uploading PBO & setup.
	//create texture group & setup.

	if (0 >= iNumOfFrame)
	{
		GLOGGER(error) << "GTextureGroupUploader::Init(). iNumOfFrame CAN NOT be negtive.";
		return false;
	}

	m_eUploadingOption = eUploadingOption;
	m_eInputFrameFormat = eInputFrameFormat;

	m_iPBOSize_Byte = 0;

	m_vCurrentFrameType.resize(iNumOfFrame);
	for (int iFrameIdx = 0; iFrameIdx < m_vCurrentFrameType.size(); ++iFrameIdx)
	{
		m_vCurrentFrameType[iFrameIdx].eFrameFormat = eInputFrameFormat;
		m_vCurrentFrameType[iFrameIdx].iWidth = 0;
		m_vCurrentFrameType[iFrameIdx].iHeight = 0;
	}

	if (GStitcherConfig::eOptionUploadingPBO == eUploadingOption)
	{
		//todo.
		m_uiPBOUploading = 0;
		glGenBuffers(1, &m_uiPBOUploading);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_uiPBOUploading);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	
	cv::Mat mTempTexture(8, 8, CV_8UC3);
	mTempTexture.setTo(0);

	if (eFrameFormatRGB == m_eInputFrameFormat ||
		eFrameFormatBGR == m_eInputFrameFormat)
	{
		m_vTextures.resize(iNumOfFrame);
		glGenTextures(m_vTextures.size(), &m_vTextures[0]);
		for (int i = 0; i < m_vTextures.size(); ++i)
		{
			glBindTexture(GL_TEXTURE_2D, m_vTextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			//float vColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, vColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
				, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, mTempTexture.data);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	else if (eFrameFormatYUV420P == m_eInputFrameFormat ||
		eFrameFormatYV12 == m_eInputFrameFormat)
	{
		m_vTextures.resize(iNumOfFrame * 3);
		glGenTextures(m_vTextures.size(), &m_vTextures[0]);
		for (int i = 0; i < m_vTextures.size(); ++i)
		{
			glBindTexture(GL_TEXTURE_2D, m_vTextures[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			//float vColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, vColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
				, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, mTempTexture.data);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
	else if (eFrameFormatNV12 == m_eInputFrameFormat)
	{
		m_vTextures.resize(iNumOfFrame * 2);
		glGenTextures(m_vTextures.size(), &m_vTextures[0]);
		for (int i = 0; i < iNumOfFrame; ++i)
		{
			glBindTexture(GL_TEXTURE_2D, m_vTextures[2 * i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			//float vColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, vColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
				, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE, mTempTexture.data);

			glBindTexture(GL_TEXTURE_2D, 0);


			glBindTexture(GL_TEXTURE_2D, m_vTextures[2 * i + 1]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
			//float vColor[4] = { 0.0, 0.0, 0.0, 0.0 };
			//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, vColor);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG
				, 8, 8, 0, GL_RG, GL_UNSIGNED_BYTE, mTempTexture.data);

			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	return true;
}

bool GTextureGroupUploader::CheckInputFrameFormat(const std::vector<std::pair<int,GFrameType>>& vInputFrameType) const
{
	for (int iAFrameIdx = 0; iAFrameIdx < vInputFrameType.size(); ++iAFrameIdx)
	{
		if (m_eInputFrameFormat != vInputFrameType[iAFrameIdx].second.eFrameFormat)
		{
			return false;
		}
	}

	return true;
}

int GTextureGroupUploader::CalcFrameSize_Byte(const GFrameType& frameType) const
{
	if (eFrameFormatRGB == m_eInputFrameFormat ||
		eFrameFormatBGR == m_eInputFrameFormat)
	{
		return frameType.iWidth * frameType.iHeight * 3;
	}
	else if (eFrameFormatYUV420P == m_eInputFrameFormat ||
		eFrameFormatYV12 == m_eInputFrameFormat)
	{
		return (frameType.iWidth * frameType.iHeight * 3) / 2;
	}
	else if (eFrameFormatNV12 == m_eInputFrameFormat)
	{
		return (frameType.iWidth * frameType.iHeight * 3) / 2;
	}
	else
	{
		GLOGGER(error) << "GTextureGroupUploader::CalcFrameGroupSize_Byte(). WRONG input frame type.";
		throw GPanoRenderException();
	}
}

int GTextureGroupUploader::CalcFramePackageSize_Byte(const std::vector<std::pair<int, GFrameType>>& vFrameType)
{
	int iRet = 0;
	for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
	{
		iRet += CalcFrameSize_Byte(vFrameType[iAFrameIdx].second);
	}
	return iRet;
}

bool GTextureGroupUploader::TextureNeedsResize(int iTextureIdx, const GFrameType& newFrameType)
{
	return (m_vCurrentFrameType[iTextureIdx].iWidth != newFrameType.iWidth ||
		m_vCurrentFrameType[iTextureIdx].iHeight != newFrameType.iHeight);
}

bool GTextureGroupUploader::ResizePBO(const std::vector<std::pair<int, GFrameType>>& vFrameType)
{
	if (GStitcherConfig::eOptionUploadingPBO == m_eUploadingOption)
	{
		if (m_iPBOSize_Byte < CalcFramePackageSize_Byte(vFrameType))
		{
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_uiPBOUploading);
			m_iPBOSize_Byte = CalcFramePackageSize_Byte(vFrameType);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, m_iPBOSize_Byte, nullptr, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		}
		else
		{
			int iMaxPackageSize = m_cbPackagesSizeBuffer[0];

			for (int i = 0; i < m_cbPackagesSizeBuffer.size(); ++i)
			{
				iMaxPackageSize = (iMaxPackageSize > m_cbPackagesSizeBuffer[i]) ? iMaxPackageSize : m_cbPackagesSizeBuffer[i];
			}

			if (m_iPBOSize_Byte > iMaxPackageSize)
			{
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_uiPBOUploading);
				m_iPBOSize_Byte = iMaxPackageSize;
				glBufferData(GL_PIXEL_UNPACK_BUFFER, m_iPBOSize_Byte, nullptr, GL_DYNAMIC_DRAW);
				glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
			}
		}
	}

	return true;
}

bool GTextureGroupUploader::ResizeTextures(const std::vector<std::pair<int, GFrameType>>& vFrameType)
{
	for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
	{
		int iFrameIdx = vFrameType[iAFrameIdx].first;
		const GFrameType& frameType = vFrameType[iAFrameIdx].second;

		if (TextureNeedsResize(iFrameIdx, frameType))
		{
			if (eFrameFormatRGB == m_eInputFrameFormat ||
				eFrameFormatBGR == m_eInputFrameFormat)
			{
				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB
					, frameType.iWidth, frameType.iHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
			}
			else if (eFrameFormatYUV420P == m_eInputFrameFormat ||
				eFrameFormatYV12 == m_eInputFrameFormat)
			{
				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx * 3]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
					, frameType.iWidth, frameType.iHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx * 3 + 1]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
					, frameType.iWidth / 2, frameType.iHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx * 3 + 2]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
					, frameType.iWidth / 2, frameType.iHeight / 2, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
			}
			else if (eFrameFormatNV12 == m_eInputFrameFormat)
			{
				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx * 2]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED
					, frameType.iWidth, frameType.iHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

				glBindTexture(GL_TEXTURE_2D, m_vTextures[iFrameIdx * 2 + 1]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RG
					, frameType.iWidth / 2, frameType.iHeight / 2, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
			}

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		m_vCurrentFrameType[iFrameIdx] = frameType;
	}

	return true;
}

bool GTextureGroupUploader::Upload(std::vector<std::shared_ptr<GFrameSourceIF>> vPtrSource, GTextureGroup& textureGroup)
{
	std::vector<std::pair<int, GFrameType>> vFrameType;
	
	for (int iQueueIdx = 0; iQueueIdx < vPtrSource.size(); ++iQueueIdx)
	{
		GFrameType frameType;
		if (!vPtrSource[iQueueIdx]->GrabFrame(frameType))
		{
			continue;
		}

		vFrameType.push_back(std::pair<int, GFrameType>(iQueueIdx, frameType));
	}

	if (vFrameType.empty())
	{
		GLOGGER(info) << "GTextureGroupUploader::Upload(). No frame avaliable now.";
		return false;
	}
	else
	{
		GLOGGER(info) << "GTextureGroupUploader::Upload(). frames: " << vFrameType.size();
	}

	if (!CheckInputFrameFormat(vFrameType))
	{
		GLOGGER(error) << "GTextureGroupUploader::Upload(). WRONG input frame format.";
		for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
		{
			vPtrSource[vFrameType[iAFrameIdx].first]->ReleaseFrame();
		}

		return false;
	}

	int iPackageSize = CalcFramePackageSize_Byte(vFrameType);
	m_cbPackagesSizeBuffer.push_back(iPackageSize);

	ResizePBO(vFrameType);

	ResizeTextures(vFrameType);

	if (GStitcherConfig::eOptionUploadingTexSubImage == m_eUploadingOption)
	{
		ReadData2TextureGroup_TexSubImage(vPtrSource, vFrameType, m_vTextures);
	}
	else if (GStitcherConfig::eOptionUploadingPBO == m_eUploadingOption)
	{
		ReadData2TextureGroup_PBO(vPtrSource, vFrameType, m_vTextures);
	}

	for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
	{
		vPtrSource[vFrameType[iAFrameIdx].first]->ReleaseFrame();
	}

	textureGroup.eTextureFormat = m_eInputFrameFormat;
	textureGroup.vTextureGroup = m_vTextures;
	return true;
}

bool GTextureGroupUploader::ReadData2TextureGroup_TexSubImage(const std::vector<std::shared_ptr<GFrameSourceIF>>& vPtrSource
	, const std::vector<std::pair<int, GFrameType>>& vFrameType
	, const std::vector<GLuint>& vTextureGroup)
{
	for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
	{
		int iFrameIdx = vFrameType[iAFrameIdx].first;
		const GFrameType& frameType = vFrameType[iAFrameIdx].second;

		if (eFrameFormatRGB == m_eInputFrameFormat ||
			eFrameFormatBGR == m_eInputFrameFormat)
		{
			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx]);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RGB, GL_UNSIGNED_BYTE
				, vPtrSource[iFrameIdx]->OutMapFrame());
		}
		else if (eFrameFormatYUV420P == m_eInputFrameFormat ||
			eFrameFormatYV12 == m_eInputFrameFormat)
		{

			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RED, GL_UNSIGNED_BYTE
				, (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame()));

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3 + 1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RED, GL_UNSIGNED_BYTE
				, (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame())
				+ (frameType.iHeight * frameType.iWidth));

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3 + 2]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RED, GL_UNSIGNED_BYTE
				, (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame())
				+ ((frameType.iHeight * frameType.iWidth) * 5) / 4);
		}
		else if (eFrameFormatNV12 == m_eInputFrameFormat)
		{

			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 2]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RED, GL_UNSIGNED_BYTE
				, (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame()));

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 2 + 1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RG, GL_UNSIGNED_BYTE
				, (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame())
				+ (frameType.iHeight * frameType.iWidth));
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return true;
}

bool GTextureGroupUploader::ReadData2TextureGroup_PBO(const std::vector<std::shared_ptr<GFrameSourceIF>>& vPtrSource
	, const std::vector<std::pair<int, GFrameType>>& vFrameType
	, const std::vector<GLuint>& vTextureGroup)
{
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_uiPBOUploading);

#if (defined __APPLE__) || (defined __ANDROID__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, CalcFramePackageSize_Byte(vFrameType), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
#elif (defined WIN32) || (defined __linux__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY));
#endif //(defined __APPLE__) || (defined __ANDROID__)

	if (nullptr == pDstBuffer)
	{
		GLOGGER(error) << "GTextureGroupUploader::ReadData2TextureGroup_PBO(). Exception: glMapBuffer returns null pointer.";
		throw GPanoRenderException();
	}

	for (int iAFrameIdx = 0; iAFrameIdx < vFrameType.size(); ++iAFrameIdx)
	{
		int iFrameIdx = vFrameType[iAFrameIdx].first;
		const GFrameType& frameType = vFrameType[iAFrameIdx].second;

		unsigned char* pSrcBuffer = (unsigned char*)(vPtrSource[iFrameIdx]->OutMapFrame());

		//cv::namedWindow("t", CV_WINDOW_KEEPRATIO);
		//cv::Mat mTemp(cv::Size(frameType.iWidth, frameType.iHeight), CV_8UC1, pSrcBuffer);
		//cv::Mat mTemp(cv::Size(frameType.iWidth / 2, frameType.iHeight / 2), CV_8UC1, pSrcBuffer + frameType.iWidth*frameType.iHeight);
		//cv::Mat mTemp(cv::Size(frameType.iWidth / 2, frameType.iHeight / 2), CV_8UC1, pSrcBuffer + (frameType.iWidth*frameType.iHeight * 5) / 4);
		//cv::imshow("t", mTemp);
		//cv::waitKey(0);

		int iFrameSize_Byte = CalcFrameSize_Byte(frameType);
		std::copy(pSrcBuffer, pSrcBuffer + iFrameSize_Byte, pDstBuffer);
		pDstBuffer += iFrameSize_Byte;
	}
	glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

	unsigned char* pOffset = 0;
	for (int iAFrameIdx2 = 0; iAFrameIdx2 < vFrameType.size(); ++iAFrameIdx2)
	{
		int iFrameIdx = vFrameType[iAFrameIdx2].first;
		const GFrameType& frameType = vFrameType[iAFrameIdx2].second;

		if (eFrameFormatRGB == m_eInputFrameFormat ||
			eFrameFormatBGR == m_eInputFrameFormat)
		{
			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}
			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx]);

			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RGB, GL_UNSIGNED_BYTE, pOffset);
			pOffset += frameType.iWidth * frameType.iHeight * 3;
		}
		else if (eFrameFormatYUV420P == m_eInputFrameFormat ||
			eFrameFormatYV12 == m_eInputFrameFormat)
		{
			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RED, GL_UNSIGNED_BYTE, pOffset);
			pOffset += frameType.iWidth * frameType.iHeight;

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3 + 1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RED, GL_UNSIGNED_BYTE, pOffset);
			pOffset += (frameType.iHeight * frameType.iWidth) / 4;

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 3 + 2]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RED, GL_UNSIGNED_BYTE, pOffset);
			pOffset += (frameType.iHeight * frameType.iWidth) / 4;
		}
		else if (eFrameFormatNV12 == m_eInputFrameFormat)
		{
			if (frameType.iWidth == 0 ||
				frameType.iHeight == 0)
			{
				continue;
			}

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 2]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth, frameType.iHeight, GL_RED, GL_UNSIGNED_BYTE, pOffset);
			pOffset += frameType.iWidth * frameType.iHeight;

			glBindTexture(GL_TEXTURE_2D, vTextureGroup[iFrameIdx * 2 + 1]);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0
				, frameType.iWidth / 2, frameType.iHeight / 2, GL_RG, GL_UNSIGNED_BYTE, pOffset);
			pOffset += (frameType.iHeight * frameType.iWidth) / 2;
		}

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	return true;
}

void GTextureGroupUploader::Release()
{
	glDeleteBuffers(1, &m_uiPBOUploading);
	if (!m_vTextures.empty())
	{
		glDeleteTextures(m_vTextures.size(), &(m_vTextures[0]));
	}
}

bool GFrameDownloader::Init(int iWidth, int iHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Create(eGOpenGLFrameBufferTypeNormal, iWidth, iHeight);
	m_FBO.SetViewPort(0, 0, iWidth, iHeight);

	return bRet;
}

void GFrameDownloader::Bind()
{
	//release the inactive channels, set active flag to false.
	std::vector<std::map<float, std::pair<bool, std::shared_ptr<GDownChannelIF>>>::iterator> vIterators;

	for (auto it = m_mpDownChannels.begin(); it != m_mpDownChannels.end(); ++it)
	{
		if (!(it->second.first))
		{
			vIterators.push_back(it);
		}

		it->second.first = false;
	}

	for (auto iit : vIterators)
	{
		iit->second.second->Release();
		m_mpDownChannels.erase(iit);
	}

	//bind FBO to the rendering pipeline.
	m_FBO.Bind();
}

void GFrameDownloader::Clear()
{
	m_FBO.Clear();
}

GOpenGLFrameBuffer& GFrameDownloader::GetFBO()
{
	return m_FBO;
}

bool GFrameDownloader::ChangeFrameSize(int iWidth, int iHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Resize(iWidth, iHeight);
	return bRet;
}

void GFrameDownloader::SetViewPort(int X, int Y, int iWidth, int iHeight)
{
	m_FBO.SetViewPort(X, Y, iWidth, iHeight);
}

cv::Point2i GFrameDownloader::NormalizedPix2Pix(const cv::Point2f& ptNormalizedPix)
{
	return m_FBO.NormalizedPix2Pix(ptNormalizedPix);
}

float GFrameDownloader::GetDepth(float fX, float fY)
{
	return m_FBO.GetDepth(fX, fY);
}

int GFrameDownloader::Width(float fScale)
{
	if (m_mpDownChannels.end() == m_mpDownChannels.find(fScale))
	{
		GLOGGER(error) << "GFrameDownloader::Width(). Error: Unknown scale value: " << fScale;
		throw GPanoRenderException();
	}
	else
	{
		return m_mpDownChannels[fScale].second->Width();
	}
}

int GFrameDownloader::Height(float fScale)
{
	if (m_mpDownChannels.end() == m_mpDownChannels.find(fScale))
	{
		GLOGGER(error) << "GFrameDownloader::Height(). Error: Unknown scale value: " << fScale;
		throw GPanoRenderException();
	}
	else
	{
		return m_mpDownChannels[fScale].second->Height();
	}
}

void GFrameDownloader::StartDownload(float fScale, GStitcherConfig::enStitcherGLOptionsDownloading eDownloadOption)
{
	int iWidth = static_cast<int>(static_cast<float>(m_FBO.Width())*fScale);
	int iHeight = static_cast<int>(static_cast<float>(m_FBO.Height())*fScale);

	if (m_mpDownChannels.end() == m_mpDownChannels.find(fScale))
	{
		std::shared_ptr<GDownChannelIF> ptrDownChannel = nullptr;

		if (GStitcherConfig::eOptionDownloadingPBO == eDownloadOption)
		{
			ptrDownChannel = std::shared_ptr<GDownChannelIF>(new GDownChannelPBO());
		}
		else if (GStitcherConfig::eOptionDownloadingReadPix == eDownloadOption)
		{
			ptrDownChannel = std::shared_ptr<GDownChannelIF>(new GDownChannelReadPix());
		}
		else
		{
			GLOGGER(error) << "GFrameDownloader::StartDownload(). Exception: Invalid eDownloadOption value: " << eDownloadOption;
			throw GPanoRenderException();
		}

		if (!ptrDownChannel->Init(iWidth, iHeight))
		{
			GLOGGER(error) << "GFrameDownloader::StartDownload(). Exception: Down channel Init failed.";
			throw GPanoRenderException();
		}

		m_mpDownChannels[fScale] = std::pair<bool, std::shared_ptr<GDownChannelIF>>(true, ptrDownChannel);
	}
	else
	{
		if (!(m_mpDownChannels[fScale].second->ChangeFrameSize(iWidth, iHeight)))
		{
			GLOGGER(error) << "GFrameDownloader::StartDownload(). Exception: Down channel ChangeFrameSize failed.";
			throw GPanoRenderException();
		}

		m_mpDownChannels[fScale].first = true;
	}


	m_FBO >> (m_mpDownChannels[fScale].second->GetFBO());
	m_mpDownChannels[fScale].second->StartDownload();
}

bool GFrameDownloader::IsActivated(float fScale)
{
	return (m_mpDownChannels.end() == m_mpDownChannels.find(fScale)) ? false : m_mpDownChannels[fScale].first;	
}

unsigned char* GFrameDownloader::MapFrame(float fScale)
{
	if (m_mpDownChannels.end() == m_mpDownChannels.find(fScale))
	{
		return nullptr;
	}
	else
	{
		return m_mpDownChannels[fScale].second->Map();
	}
}

void GFrameDownloader::UnmapFrame(float fScale)
{
	if (m_mpDownChannels.end() == m_mpDownChannels.find(fScale))
	{
		GLOGGER(error) << "GFrameDownloader::UnmapFrame(). Error: Unknown scale value: " << fScale;
	}
	else
	{
		m_mpDownChannels[fScale].second->Unmap();
	}

	return;
}

void GFrameDownloader::Release()
{
	m_FBO.Release();

	for (auto chan : m_mpDownChannels)
	{
		chan.second.second->Release();
	}
}

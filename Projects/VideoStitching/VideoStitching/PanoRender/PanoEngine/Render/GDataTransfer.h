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
 
 
 
#ifndef G_DATA_TRANSFER_H_
#define G_DATA_TRANSFER_H_

#include <vector>
#include <map>
#include <memory>

#include "opencv2/core/core.hpp"
#include <boost/circular_buffer.hpp>

#include "PanoRender/GPanoRenderCommonIF.h"
#include "PanoRender/GOpenGLPixelBuffer.h"
#include "GDownChannels.h"

class GTextureGroupUploader
{
public:
	GTextureGroupUploader() :
		m_eUploadingOption(GStitcherConfig::eOptionUploadingTexSubImage)
		, m_eInputFrameFormat(eFrameFormatRGB)
		, m_uiPBOUploading(0)
		, m_iPBOSize_Byte(0)
		, m_vTextures(std::vector<GLuint>())
		, m_cbPackagesSizeBuffer(boost::circular_buffer<int>(10))
	{}
	bool Init(GStitcherConfig::enStitcherGLOptionsUploading eUploadingOption, enFrameFormat eInputFrameFormat, int iNumOfFrame);
	bool Upload(std::vector<std::shared_ptr<GFrameSourceIF>> vPtrSource, GTextureGroup& textureGroup);
	void Release();

private:
	bool CheckInputFrameFormat(const std::vector<std::pair<int, GFrameType>>& vInputFrameType) const;

	int CalcFrameSize_Byte(const GFrameType& frameType) const;

	int CalcFramePackageSize_Byte(const std::vector<std::pair<int, GFrameType>>& vFrameType);

	bool TextureNeedsResize(int iTextureIdx, const GFrameType& newFrameType);

	bool ResizePBO(const std::vector<std::pair<int, GFrameType>>& vFrameType);

	bool ResizeTextures(const std::vector<std::pair<int, GFrameType>>& vFrameType);

	bool ReadData2TextureGroup_TexSubImage(const std::vector<std::shared_ptr<GFrameSourceIF>>& vPtrSource
		, const std::vector<std::pair<int, GFrameType>>& vFrameType
		, const std::vector<GLuint>& vTextureGroup);

	bool ReadData2TextureGroup_PBO(const std::vector<std::shared_ptr<GFrameSourceIF>>& vPtrSource
		, const std::vector<std::pair<int, GFrameType>>& vFrameType
		, const std::vector<GLuint>& vTextureGroup);
private:
	GStitcherConfig::enStitcherGLOptionsUploading m_eUploadingOption;
	enFrameFormat m_eInputFrameFormat;

	GLuint m_uiPBOUploading;
	int m_iPBOSize_Byte;

	std::vector<GFrameType> m_vCurrentFrameType;
	std::vector<GLuint> m_vTextures;

	boost::circular_buffer<int> m_cbPackagesSizeBuffer;
};


class GFrameDownloader
{
public:
	GFrameDownloader()
	{}

	bool Init(int iWidth, int iHeight);
	void Bind();
	void Clear();
	GOpenGLFrameBuffer& GetFBO();
	bool ChangeFrameSize(int iWidth, int iHeight);
	void SetViewPort(int X, int Y, int iWidth, int iHeight);
	glm::vec4 GetViewPort() { return m_FBO.GetViewPort(); };
	cv::Point2i NormalizedPix2Pix(const cv::Point2f& ptNormalizedPix);
	float GetDepth(float fX, float fY);
	int Width(float fScale);
	int Height(float fScale);
	void StartDownload(float fScale, GStitcherConfig::enStitcherGLOptionsDownloading eDownloadOption);
	bool IsActivated(float fScale);
	unsigned char* MapFrame(float fScale);
	void UnmapFrame(float fScale);
	void Release();

private:
	std::map<float, std::pair<bool, std::shared_ptr<GDownChannelIF>>> m_mpDownChannels;
	GOpenGLFrameBuffer m_FBO;
};


#endif//!G_DATA_TRANSFER_H_
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
 
 
 
#ifndef G_PANO_RENDER_COMMON_H_
#define G_PANO_RENDER_COMMON_H_

#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include "GGLHeaders.h"

enum enFrameFormat
{
	eFrameFormatRGB = 0,
	eFrameFormatBGR,
	eFrameFormatYUV420P,
	eFrameFormatYV12,
	eFrameFormatNV12,
};

struct GTextureGroup
{
	enFrameFormat eTextureFormat;
	std::vector<GLuint> vTextureGroup;
};

struct GFrameType
{
	int iWidth;
	int iHeight;
	int step_Byte[3];
	enFrameFormat eFrameFormat;

	int Size_Byte() const
	{
		if (eFrameFormatBGR == eFrameFormat ||
			eFrameFormatRGB == eFrameFormat)
		{
			return iWidth * iHeight * 3;
		}
		else if (eFrameFormatYUV420P == eFrameFormat ||
			eFrameFormatYV12 == eFrameFormat)
		{
			return (iWidth * iHeight * 3) / 2;
		}
		else if (eFrameFormatNV12 == eFrameFormat)
		{
			return (iWidth * iHeight * 3) / 2;
		}

		//todo.
		return 0;
	}
};

class GFrameBufferIF
{
public:
	virtual ~GFrameBufferIF(){};

	virtual void Init(int iSize_Byte) = 0;
	virtual bool CreateFrame(const GFrameType& frameType) = 0;
	virtual void* InMapFrame() = 0;
	virtual void CompleteFrame() = 0;
};

class GFrameSourceIF
{
public:
	virtual ~GFrameSourceIF(){};

	virtual bool GrabFrame(GFrameType& frameType) = 0;
	virtual void* OutMapFrame() = 0;
	virtual void ReleaseFrame() = 0;
};

class GViewCamera4RenderIF
{
public:
	virtual ~GViewCamera4RenderIF(){};
	virtual glm::mat4 CalcP() const = 0;
	virtual glm::mat4 CalcV() const = 0;
};

class GStitcherConfig
{
public:
	GStitcherConfig() :
		m_eRenderingMode(eRenderingModeOnScreen)
		, m_eOptionUploading(eOptionUploadingPBO)
		, m_eOptionDownloading(eOptionDownloadingPBO)
		, m_eInputFrameFormat(eFrameFormatYUV420P)
	{}

	enum enRenderingMode
	{
		eRenderingModeOnScreen = 0,
		eRenderingModeOffScreen = 1,
		eRenderingModeOffScreenRealTime = 2,
	};

	enum enStitcherGLOptionsUploading
	{
		eOptionUploadingTexSubImage = 0,
		eOptionUploadingPBO,
	};

	enum enStitcherGLOptionsDownloading
	{
		eOptionDownloadingReadPix = 0,
		eOptionDownloadingPBO,
	};

	enRenderingMode m_eRenderingMode;
	enStitcherGLOptionsUploading m_eOptionUploading;
	enStitcherGLOptionsDownloading m_eOptionDownloading;
	enFrameFormat m_eInputFrameFormat;
};


#endif //G_PANO_RENDER_COMMON_H_
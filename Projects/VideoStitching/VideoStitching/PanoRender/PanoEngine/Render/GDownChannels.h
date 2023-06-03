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
 
 
 
#ifndef G_MULTI_RESOLUTION_DOWN_CHANNELS_H_
#define G_MULTI_RESOLUTION_DOWN_CHANNELS_H_

#include "opencv2/core/core.hpp"

#include "PanoRender/GPanoRenderCommonIF.h"
#include "PanoRender/GOpenGLFrameBuffer.h"
#include "PanoRender/GOpenGLPixelBuffer.h"

class GDownChannelIF
{
public:
	virtual ~GDownChannelIF(){};

	virtual bool Init(int iWidth, int iHeight) = 0;
	virtual GOpenGLFrameBuffer& GetFBO() = 0;
	virtual bool ChangeFrameSize(int iWidth, int iHeight) = 0;
	virtual int Width() = 0;
	virtual int Height() = 0;
	virtual void StartDownload() = 0;
	virtual unsigned char* Map() = 0;
	virtual void Unmap() = 0;
	virtual void Release() = 0;
};

class GDownChannelPBO : public GDownChannelIF
{
public:
	GDownChannelPBO()
	{}

	virtual bool Init(int iDefaultWidth, int iDefaultHeight);
	virtual GOpenGLFrameBuffer& GetFBO();
	virtual bool ChangeFrameSize(int iWidth, int iHeight);
	virtual int Width();
	virtual int Height();
	virtual void StartDownload();
	virtual unsigned char* Map();
	virtual void Unmap();
	virtual void Release();

private:
	GOpenGLFrameBuffer m_FBO;
	GOpenGLPixelBuffer m_PBO;
};

class GDownChannelReadPix : public GDownChannelIF
{
public:
	GDownChannelReadPix()
	{};

	virtual bool Init(int iWidth, int iHeight);
	virtual GOpenGLFrameBuffer& GetFBO();
	virtual bool ChangeFrameSize(int iWidth, int iHeight);
	virtual int Width();
	virtual int Height();
	virtual void StartDownload();
	virtual unsigned char* Map();
	virtual void Unmap();
	virtual void Release();

private:
	GOpenGLFrameBuffer m_FBO;
	cv::Mat m_mBuffer;
};


#endif //G_MULTI_RESOLUTION_DOWN_CHANNELS_H_
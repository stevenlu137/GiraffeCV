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
 
 
 
#ifndef G_OPENGL_PIXEL_BUFFER_H_
#define G_OPENGL_PIXEL_BUFFER_H_

#include "opencv2/core/core.hpp"

#include "GOpenGLObjectsIF.h"
#include "PanoRender/GPanoRenderCommonIF.h"


class GOpenGLPixelBuffer : public GOpenGLObjectsIF
{
public:
	GOpenGLPixelBuffer() :
		m_iBindPoint(0)
		, m_iUsage(0)
		, m_iFrameWidth(0)
		, m_iFrameHeight(0)
	{}

	bool Create(int iBindPoint, int iUsage, int iWidth = 0, int iHeight = 0);

	bool Resize(int iWidth, int iHeight);

	void FeedIn(void* pData, int iWidth, int iHeight);

	unsigned char* Map();

	void Unmap();

	void operator<<(const cv::Mat& mFrame);

	void operator>>(cv::Mat& mFrame);

	void Release();

private:
	int m_iBindPoint;
	int m_iUsage;

	int m_iFrameWidth;
	int m_iFrameHeight;
};

#endif //G_OPENGL_PIXEL_BUFFER_H_

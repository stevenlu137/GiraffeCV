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
 
 
 
#ifndef G_OPENGL_FRAME_BUFFER_H_
#define G_OPENGL_FRAME_BUFFER_H_

#include "opencv2/core/core.hpp"

#include "GOpenGLObjectsIF.h"
#include <glm/glm.hpp>

class GOpenGLPixelBuffer;

enum enGOpenGLFrameBufferType
{
	eGOpenGLFrameBufferTypeDisplay = 0,
	eGOpenGLFrameBufferTypeNormal = 1,
};

class GOpenGLFrameBuffer : public GOpenGLObjectsIF
{
public:
	GOpenGLFrameBuffer() :
		m_iMaxRenderBufferSize(0)
		, m_iFrameWidth(0)
		, m_iFrameHeight(0)
		, m_iX0(0)
		, m_iY0(0)
		, m_iViewportWidth(0)
		, m_iViewportHeight(0)
	{
		m_RBO[0] = 0;
		m_RBO[1] = 0;
	}

	bool Create(enGOpenGLFrameBufferType eFrameBufferType, int iWidth = 0, int iHeight = 0);

	bool Resize(int iWidth, int iHeight);

	int Width() const { return m_iFrameWidth; };

	int Height() const { return m_iFrameHeight; };

	void SetViewPort(int X, int Y, int iPortWidth, int iPortHeight);

	void SetViewPort();

	glm::vec4 GetViewPort() { return glm::vec4(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight); };

	int ViewPortX() const { return m_iX0; };

	int ViewPortY() const { return m_iY0; };

	int ViewPortWidth() const { return m_iViewportWidth; };

	int ViewPortHeight() const { return m_iViewportHeight; };

	cv::Point2i NormalizedPix2Pix(const cv::Point2f& ptNormalizedPix);

	float GetDepth(float fX, float fY);

	void Clear();

	void Bind();

	void operator>>(GOpenGLFrameBuffer& FBO);

	void operator>>(GOpenGLPixelBuffer& PBO);

	void operator>>(cv::Mat& mFrame);

	void Release();

private:
	GLint m_iMaxRenderBufferSize;

	int m_iFrameWidth;
	int m_iFrameHeight;

	int m_iX0;
	int m_iY0;
	int m_iViewportWidth;
	int m_iViewportHeight;

	GLuint m_RBO[2];
};

#endif //G_OPENGL_FRAME_BUFFER_H_
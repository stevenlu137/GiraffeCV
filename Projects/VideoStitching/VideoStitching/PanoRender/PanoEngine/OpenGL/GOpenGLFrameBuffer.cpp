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
 
 
 
#include "PanoRender/GOpenGLFrameBuffer.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "Common/Exception/GiraffeException.h"
#include "PanoRender/GOpenGLPixelBuffer.h"



bool GOpenGLFrameBuffer::Create(enGOpenGLFrameBufferType eFrameBufferType, int iWidth/* = 0*/, int iHeight/* = 0*/)
{
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &m_iMaxRenderBufferSize);

	m_iFrameWidth = iWidth;
	m_iFrameHeight = iHeight;

	if (eGOpenGLFrameBufferTypeDisplay == eFrameBufferType)
	{
		m_uiObjectID = 0;
	}
	else if (eGOpenGLFrameBufferTypeNormal == eFrameBufferType)
	{
		//create FBO,RBO & setup.

		glGenFramebuffers(1, &m_uiObjectID);
		glGenRenderbuffers(2, &(m_RBO[0]));

		if (m_iFrameWidth > static_cast<int>(m_iMaxRenderBufferSize) ||
			m_iFrameHeight > static_cast<int>(m_iMaxRenderBufferSize))
		{
			Release();

			GLOGGER(error) << "GOpenGLFrameBuffer::Create(). Error: Render buffer size is too large. Acqired width: " << m_iFrameWidth
				<< ", acquired height: " << m_iFrameHeight << "MaxRenderBufferSize: " << m_iMaxRenderBufferSize << ".";
			return false;
		}

		if (m_iFrameWidth != 0 &&
			m_iFrameHeight != 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO[0]);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_iFrameWidth, m_iFrameHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindRenderbuffer(GL_RENDERBUFFER, m_RBO[1]);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_iFrameWidth, m_iFrameHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);

			glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_RBO[0]);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO[1]);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
	else
	{
		GLOGGER(error) << "GOpenGLFrameBuffer::Create(). Error: Not supported eFrameBufferType: " << eFrameBufferType;
		return false;
	}

	return true;
}

bool GOpenGLFrameBuffer::Resize(int iWidth, int iHeight)
{
	if ((m_iFrameWidth == iWidth) &&
		(m_iFrameHeight == iHeight))
	{
		return true;
	}

	if (0 == m_uiObjectID)
	{
		GLOGGER(error) << "GOpenGLFrameBuffer::Resize(). Error: try to resize the default FBO.";
		return false;
	}

	m_iFrameWidth = iWidth;
	m_iFrameHeight = iHeight;

	if (m_iFrameWidth > static_cast<int>(m_iMaxRenderBufferSize) ||
		m_iFrameHeight > static_cast<int>(m_iMaxRenderBufferSize))
	{
		GLOGGER(error) << "GOpenGLFrameBuffer::Resize(). Error: Render buffer size is too large. Acqired width: " << m_iFrameWidth
			<< ", acquired height: " << m_iFrameHeight << "MaxRenderBufferSize: " << m_iMaxRenderBufferSize << ".";
		return false;
	}

	GLuint uiNewRBO[2] = { 0, 0 };
	glGenRenderbuffers(2, &(uiNewRBO[0]));

	glBindRenderbuffer(GL_RENDERBUFFER, uiNewRBO[0]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, m_iFrameWidth, m_iFrameHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, uiNewRBO[1]);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_iFrameWidth, m_iFrameHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, uiNewRBO[0]);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, uiNewRBO[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDeleteRenderbuffers(2, &(m_RBO[0]));

	m_RBO[0] = uiNewRBO[0];
	m_RBO[1] = uiNewRBO[1];

	return true;
}

void GOpenGLFrameBuffer::SetViewPort(int X, int Y, int iPortWidth, int iPortHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);

	m_iX0 = X;
	m_iY0 = Y;
	m_iViewportWidth = iPortWidth;
	m_iViewportHeight = iPortHeight;

	glViewport(m_iX0, m_iY0, (GLsizei)m_iViewportWidth, (GLsizei)m_iViewportHeight);
}

void GOpenGLFrameBuffer::SetViewPort()
{
	glViewport(m_iX0, m_iY0, (GLsizei)m_iViewportWidth, (GLsizei)m_iViewportHeight);
}

cv::Point2i GOpenGLFrameBuffer::NormalizedPix2Pix(const cv::Point2f& ptNormalizedPix)
{
	return cv::Point2i(m_iX0 + static_cast<int>(ptNormalizedPix.x * static_cast<float>(m_iViewportWidth))
		, m_iViewportHeight - m_iY0 - static_cast<int>(ptNormalizedPix.y * static_cast<float>(m_iViewportHeight)));
}

float GOpenGLFrameBuffer::GetDepth(float fX, float fY)
{
	Bind();

	cv::Point2i ptPix = NormalizedPix2Pix(cv::Point2f(fX, fY));
	float fDepth = 0.0f;
	glReadPixels(ptPix.x, ptPix.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &fDepth);
	return fDepth;
}

void GOpenGLFrameBuffer::Clear()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);
	glClearColor(0.282f, 0.282f, 0.282f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GOpenGLFrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);
}

void GOpenGLFrameBuffer::operator>>(GOpenGLFrameBuffer& FBO)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uiObjectID);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO.GetObjectID());

	//glViewport(FBO.m_iX0, FBO.m_iY0, FBO.m_iViewportWidth, FBO.m_iViewportHeight);
	glBlitFramebuffer(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight
		, FBO.m_iX0, FBO.m_iY0, FBO.m_iViewportWidth, FBO.m_iViewportHeight
		, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void GOpenGLFrameBuffer::operator>>(GOpenGLPixelBuffer& PBO)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uiObjectID);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, PBO.GetObjectID());

	if (!PBO.Resize(m_iViewportWidth, m_iViewportHeight))
	{
		GLOGGER(error) << "GOpenGLFrameBuffer::operator>>(). Error: Resize PBO FAILED.";
		throw GPanoRenderException();
	}

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

#if (defined __APPLE__) || (defined __ANDROID__)
	glReadPixels(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight, GL_RGB, GL_UNSIGNED_BYTE, (void*)(nullptr));
#elif (defined WIN32) || (defined __linux__)
	glReadPixels(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight, GL_BGR, GL_UNSIGNED_BYTE, (void*)(nullptr));
#endif //(defined __APPLE__) || (defined __ANDROID__)
}

void GOpenGLFrameBuffer::operator>>(cv::Mat& mFrame)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_uiObjectID);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadBuffer(GL_RENDERBUFFER); todo.
	mFrame.create(m_iViewportHeight, m_iViewportWidth, CV_8UC3);

#if (defined __APPLE__) || (defined __ANDROID__)
	glReadPixels(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight, GL_RGB, GL_UNSIGNED_BYTE, (void*)(mFrame.data));
#elif (defined WIN32) || (defined __linux__)
	glReadPixels(m_iX0, m_iY0, m_iViewportWidth, m_iViewportHeight, GL_BGR, GL_UNSIGNED_BYTE, (void*)(mFrame.data));
#endif //(defined __APPLE__) || (defined __ANDROID__)
}

void GOpenGLFrameBuffer::Release()
{
	if (0 != m_uiObjectID)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_uiObjectID);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDeleteRenderbuffers(2, &(m_RBO[0]));
		glDeleteFramebuffers(1, &m_uiObjectID);
	}
}


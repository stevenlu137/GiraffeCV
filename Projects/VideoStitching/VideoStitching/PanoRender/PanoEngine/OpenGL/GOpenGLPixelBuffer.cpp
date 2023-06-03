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
 
 
 
#include "PanoRender/GOpenGLPixelBuffer.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "Common/Exception/GiraffeException.h"


bool GOpenGLPixelBuffer::Create(int iBindPoint, int iUsage, int iWidth/* = 0*/, int iHeight/* = 0*/)
{
	//create downloading PBO & setup.
	m_iBindPoint = iBindPoint;
	m_iUsage = iUsage;

	m_iFrameWidth = iWidth;
	m_iFrameHeight = iHeight;

	m_uiObjectID = 0;
	glGenBuffers(1, &m_uiObjectID);
	glBindBuffer(m_iBindPoint, m_uiObjectID);
	if (m_iFrameWidth != 0 &&
		m_iFrameHeight != 0)
	{
		glBufferData(m_iBindPoint, m_iFrameWidth * m_iFrameHeight * 3, nullptr, m_iUsage);
	}

	return true;
}

bool GOpenGLPixelBuffer::Resize(int iWidth, int iHeight)
{
	if ((m_iFrameWidth == iWidth) &&
		(m_iFrameHeight == iHeight))
	{
		return true;
	}

	m_iFrameWidth = iWidth;
	m_iFrameHeight = iHeight;

	glBindBuffer(m_iBindPoint, m_uiObjectID);
	glBufferData(m_iBindPoint, m_iFrameWidth * m_iFrameHeight * 3, nullptr, m_iUsage);
	glBindBuffer(m_iBindPoint, 0);

	return true;
}

void GOpenGLPixelBuffer::FeedIn(void* pData, int iWidth, int iHeight)
{
	if (!Resize(iWidth, iHeight))
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::FeedIn(). Exception: Resize PBO FAILED.";
		throw GPanoRenderException();
	}

	if (0 == m_iFrameWidth || 0 == m_iFrameHeight)
	{
		return;
	}

	glBindBuffer(m_iBindPoint, m_uiObjectID);

#if (defined __APPLE__) || (defined __ANDROID__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBufferRange(m_iBindPoint, 0, m_iFrameWidth * m_iFrameHeight * 3, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
#elif (defined WIN32) || (defined __linux__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBuffer(m_iBindPoint, GL_WRITE_ONLY));
#endif //(defined __APPLE__) || (defined __ANDROID__)

	if (nullptr == pDstBuffer)
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::FeedIn(). Exception: glMapBuffer returns null pointer.";
		throw GPanoRenderException();
	}

	unsigned char* pSrcBuffer = (unsigned char*)(pData);
	std::copy(pSrcBuffer, pSrcBuffer + iWidth * iHeight * 3, pDstBuffer);

	glUnmapBuffer(m_iBindPoint);
}

unsigned char* GOpenGLPixelBuffer::Map()
{
	if (0 == m_iFrameWidth || 0 == m_iFrameHeight)
	{
		return nullptr;
	}

	glBindBuffer(m_iBindPoint, m_uiObjectID);

#if (defined __APPLE__) || (defined __ANDROID__)
	unsigned char* pDownloadBuffer = (unsigned char*)(glMapBufferRange(m_iBindPoint, 0, m_iFrameWidth * m_iFrameHeight * 3, GL_MAP_READ_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
#elif (defined WIN32) || (defined __linux__)
	unsigned char* pDownloadBuffer = (unsigned char*)(glMapBuffer(m_iBindPoint, GL_READ_ONLY));
#endif //(defined __APPLE__) || (defined __ANDROID__)

	if (nullptr == pDownloadBuffer)
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::Map(). Exception: glMapBuffer returns null pointer.";
		throw GPanoRenderException();
	}

	return pDownloadBuffer;
}

void GOpenGLPixelBuffer::Unmap()
{
	if (0 != m_iFrameWidth && 0 != m_iFrameHeight)
	{
		glUnmapBuffer(m_iBindPoint);
	}
}

void GOpenGLPixelBuffer::operator<<(const cv::Mat& mFrame)
{
	if (!Resize(mFrame.cols, mFrame.rows))
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::operator<<(). Exception: Resize PBO FAILED.";
		throw GPanoRenderException();
	}

	if (0 == m_iFrameWidth || 0 == m_iFrameHeight)
	{
		return;
	}

	glBindBuffer(m_iBindPoint, m_uiObjectID);

#if (defined __APPLE__) || (defined __ANDROID__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBufferRange(m_iBindPoint, 0, m_iFrameWidth * m_iFrameHeight * 3, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
#elif (defined WIN32) || (defined __linux__)
	unsigned char* pDstBuffer = (unsigned char*)(glMapBuffer(m_iBindPoint, GL_WRITE_ONLY));
#endif //(defined __APPLE__) || (defined __ANDROID__)

	if (nullptr == pDstBuffer)
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::operator<<(). Exception: glMapBuffer returns null pointer.";
		throw GPanoRenderException();
	}

	unsigned char* pSrcBuffer = (unsigned char*)(mFrame.data);
	int iLineWidth_Byte = mFrame.cols*mFrame.elemSize();
	for (int i = 0; i < mFrame.rows; ++i)
	{
		std::copy(pSrcBuffer + i*mFrame.step[0], pSrcBuffer + i*mFrame.step[0] + iLineWidth_Byte, pDstBuffer + i*iLineWidth_Byte);
	}

	glUnmapBuffer(m_iBindPoint);
}

void GOpenGLPixelBuffer::operator>>(cv::Mat& mFrame)
{
	if (0 == m_iFrameWidth || 0 == m_iFrameHeight)
	{
		mFrame.create(0, 0, CV_8UC3);
		return;
	}

	glBindBuffer(GL_PIXEL_PACK_BUFFER, m_uiObjectID);

#if (defined __APPLE__) || (defined __ANDROID__)
	unsigned char* pDownloadBuffer = (unsigned char*)(glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, m_iFrameWidth * m_iFrameHeight * 3, GL_MAP_READ_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
#elif (defined WIN32) || (defined __linux__)
	unsigned char* pDownloadBuffer = (unsigned char*)(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
#endif //(defined __APPLE__) || (defined __ANDROID__)

	if (nullptr == pDownloadBuffer)
	{
		GLOGGER(error) << "GOpenGLPixelBuffer::operator>>. Exception: glMapBuffer returns null pointer.";
		throw GPanoRenderException();
	}

	mFrame.create(m_iFrameHeight, m_iFrameWidth, CV_8UC3);
	cv::Mat mDownloadBuffer = cv::Mat(cv::Size(m_iFrameWidth, m_iFrameHeight), CV_8UC3, pDownloadBuffer);
	mDownloadBuffer.copyTo(mFrame);
	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
}

void GOpenGLPixelBuffer::Release()
{
	glDeleteBuffers(1, &m_uiObjectID);
}


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
 
 
 
#include "GDownChannels.h"


bool GDownChannelPBO::Init(int iDefaultWidth, int iDefaultHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Create(eGOpenGLFrameBufferTypeNormal, iDefaultWidth, iDefaultHeight);
	bRet &= m_PBO.Create(GL_PIXEL_PACK_BUFFER, GL_STREAM_READ, iDefaultWidth, iDefaultHeight);

	m_FBO.SetViewPort(0, 0, iDefaultWidth, iDefaultHeight);

	return bRet;
}

GOpenGLFrameBuffer& GDownChannelPBO::GetFBO()
{
	return m_FBO;
}

bool GDownChannelPBO::ChangeFrameSize(int iWidth, int iHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Resize(iWidth, iHeight);
	bRet &= m_PBO.Resize(iWidth, iHeight);

	m_FBO.SetViewPort(0, 0, iWidth, iHeight);

	return bRet;
}

int GDownChannelPBO::Width()
{
	return m_FBO.Width();
}

int GDownChannelPBO::Height()
{
	return m_FBO.Height();
}

void GDownChannelPBO::StartDownload()
{
	m_FBO >> m_PBO;
}

unsigned char* GDownChannelPBO::Map()
{
	return m_PBO.Map();
}

void GDownChannelPBO::Unmap()
{
	m_PBO.Unmap();
}

void GDownChannelPBO::Release()
{
	m_FBO.Release();
	m_PBO.Release();
}

bool GDownChannelReadPix::Init(int iDefaultWidth, int iDefaultHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Create(eGOpenGLFrameBufferTypeNormal, iDefaultWidth, iDefaultHeight);
	m_mBuffer.create(iDefaultHeight, iDefaultWidth, CV_8UC3);

	m_FBO.SetViewPort(0, 0, iDefaultWidth, iDefaultHeight);

	return bRet;
}

GOpenGLFrameBuffer& GDownChannelReadPix::GetFBO()
{
	return m_FBO;
}

bool GDownChannelReadPix::ChangeFrameSize(int iWidth, int iHeight)
{
	bool bRet = true;
	bRet &= m_FBO.Resize(iWidth, iHeight);
	m_mBuffer.create(iWidth, iHeight, CV_8UC3);

	m_FBO.SetViewPort(0, 0, iWidth, iHeight);

	return bRet;
}

int GDownChannelReadPix::Width()
{
	return m_FBO.Width();
}

int GDownChannelReadPix::Height()
{
	return m_FBO.Height();
}

void GDownChannelReadPix::StartDownload()
{
	return;
}

unsigned char* GDownChannelReadPix::Map()
{
	m_FBO >> m_mBuffer;
	return m_mBuffer.data;
}

void GDownChannelReadPix::Unmap()
{
	return;
}

void GDownChannelReadPix::Release()
{
	m_FBO.Release();
}
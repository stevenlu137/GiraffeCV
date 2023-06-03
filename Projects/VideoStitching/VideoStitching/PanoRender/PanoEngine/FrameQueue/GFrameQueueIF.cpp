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
 
 
 
#include "PanoRender/GFrameQueueIF.h"

#include "Common/Buffers/GDataFrameQueue.h"


class GFrameQueue : public GFrameQueueIF
{
public:
	GFrameQueue(){}

	virtual void Init(int iSize_Byte);
	virtual bool CreateFrame(const GFrameType& frameType);
	virtual void* InMapFrame();
	virtual void CompleteFrame();

	virtual bool GrabFrame(GFrameType& frameType);
	virtual void* OutMapFrame();
	virtual void ReleaseFrame();

private:
	GDataFrameQueue<GFrameType> m_DataFrameQueue;

};

void GFrameQueue::Init(int iSize_Byte)
{
	return m_DataFrameQueue.Init(iSize_Byte);
}

bool GFrameQueue::CreateFrame(const GFrameType& frameType)
{
	return m_DataFrameQueue.CreateFrame(frameType);
}

void* GFrameQueue::InMapFrame()
{
	return m_DataFrameQueue.InMapFrame();
}

void GFrameQueue::CompleteFrame()
{
	return m_DataFrameQueue.CompleteFrame();
}

bool GFrameQueue::GrabFrame(GFrameType& frameType)
{
	return m_DataFrameQueue.GrabFrame(frameType);
}

void* GFrameQueue::OutMapFrame()
{
	return m_DataFrameQueue.OutMapFrame();
}

void GFrameQueue::ReleaseFrame()
{
	return m_DataFrameQueue.ReleaseFrame();
}


std::shared_ptr<GFrameQueueIF> GFrameQueueFactory::CreateFrameQueue()
{
	return std::shared_ptr<GFrameQueueIF>(new GFrameQueue());
}
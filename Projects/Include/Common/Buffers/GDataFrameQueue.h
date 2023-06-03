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
 
 
 
#ifndef G_DATA_FRAME_QUEUE_H_
#define G_DATA_FRAME_QUEUE_H_

#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <cstring>

#include "Common/GiraffeLogger/GiraffeLogger.h"


template<typename TDataFrameHead>
class GDataFrameQueue
{
public:
	GDataFrameQueue() :
		m_bRejectWhenFull(true)
		, m_bReadingFlag(false)
		, m_iTotalSize_Byte(0)
		, m_iMaxFrameNum(4)
	{}

	void Init(int iSize_Byte, bool bRejectWhenFull = true, int iMaxFrameNum = 4);
	bool CreateFrame(const TDataFrameHead& frameHead);
	void* InMapFrame();
	void CompleteFrame();

	bool GrabFrame(TDataFrameHead& frameHead);
	void* OutMapFrame();
	void ReserveFrame();
	void ReleaseFrame();

private:
	int GetWriteableIdx(int iSize_Byte);

	bool CheckShouldResize(const TDataFrameHead& frameHead);

	int CalcNewQueueSize(const TDataFrameHead& frameHead) const;

	void ResizeQueue(int iNewSize);

private:
	struct DataFrameHead_Internal
	{
		DataFrameHead_Internal() :
			iStartIdx(0)
		{}

		DataFrameHead_Internal(const TDataFrameHead& fHead, int iStart)
			:iStartIdx(iStart)
			, frameHead(fHead)
		{}

		TDataFrameHead frameHead;
		int iStartIdx;
	};

private:
	int m_iMaxFrameNum;
	bool m_bRejectWhenFull;

	std::mutex m_mtHeadBufferMutex;

	bool m_bReadingFlag;
	std::mutex m_mtReadingFlagMutex;
	std::condition_variable m_cvReadingFlagCV;

	std::vector<unsigned char> m_vFrameBuffer;
	std::queue<DataFrameHead_Internal, std::deque<DataFrameHead_Internal>> m_qHeadBuffer;
	int m_iTotalSize_Byte; //m_iTotalSize_Byte & m_qHeadBuffer should be protected by m_mtHeadBufferMutex.

	DataFrameHead_Internal m_tempHead4Write;
};

template<typename TDataFrameHead>
void GDataFrameQueue<TDataFrameHead>::Init(int iSize_Byte, bool bRejectWhenFull/* = true*/, int iMaxFrameNum/* = 4*/)
{
	m_bRejectWhenFull = bRejectWhenFull;
	m_vFrameBuffer.resize(iSize_Byte);
	m_iMaxFrameNum = iMaxFrameNum;
}

template<typename TDataFrameHead>
bool GDataFrameQueue<TDataFrameHead>::CreateFrame(const TDataFrameHead& frameHead)
{
	//allcate memory;

	if (CheckShouldResize(frameHead))
	{
		//wait for reading thread finish its work, then lock the whole frame queue until resizing is done.
		std::unique_lock<std::mutex> lk(m_mtReadingFlagMutex);
		m_cvReadingFlagCV.wait(lk, [this]()
		{
			return !(this->m_bReadingFlag);
		});

		ResizeQueue(CalcNewQueueSize(frameHead));
	}

	int iStartIdx = GetWriteableIdx(frameHead.Size_Byte());
	if (m_bRejectWhenFull)
	{
		if (iStartIdx < 0)
		{
			//GLOGGER(info) << "GDataFrameQueue::CreateFrame(). no enough memory.";
			return false;
		}
	}
	else
	{
		while (iStartIdx < 0)
		{
			{
				std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
				if (m_qHeadBuffer.empty())
				{
					return false;
				}
			}

			//wait for reading thread finish its work, then lock the whole frame queue until the oldest head has been popped.
			std::unique_lock<std::mutex> lk(m_mtReadingFlagMutex);
			m_cvReadingFlagCV.wait(lk, [this]()
			{
				return !(this->m_bReadingFlag);
			});

			{
				std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
				m_iTotalSize_Byte -= m_qHeadBuffer.front().frameHead.Size_Byte();
				m_qHeadBuffer.pop();
			}

			iStartIdx = GetWriteableIdx(frameHead.Size_Byte());
		}
	}

	m_tempHead4Write.iStartIdx = iStartIdx;
	m_tempHead4Write.frameHead = frameHead;
	return true;
}

template<typename TDataFrameHead>
void* GDataFrameQueue<TDataFrameHead>::InMapFrame()
{
	return &(m_vFrameBuffer[m_tempHead4Write.iStartIdx]);
}

template<typename TDataFrameHead>
void GDataFrameQueue<TDataFrameHead>::CompleteFrame()
{
	{
		std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
		m_qHeadBuffer.push(m_tempHead4Write);
		m_iTotalSize_Byte += m_tempHead4Write.frameHead.Size_Byte();
	}
}

template<typename TDataFrameHead>
bool GDataFrameQueue<TDataFrameHead>::GrabFrame(TDataFrameHead& frameHead)
{
	memset(&frameHead, 0, sizeof(frameHead));

	{
		std::lock_guard<std::mutex> readingFlagLock(m_mtReadingFlagMutex);
		m_bReadingFlag = true;
	}

	std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);

	if (m_qHeadBuffer.empty())
	{
		{
			std::lock_guard<std::mutex> readingFlagLock(m_mtReadingFlagMutex);
			m_bReadingFlag = false;
		}
		m_cvReadingFlagCV.notify_one();

		//GLOGGER(info) << "GDataFrameQueue::GrabFrame(). no available frame.";
		return false;
	}

	frameHead = m_qHeadBuffer.front().frameHead;

	return true;
}

template<typename TDataFrameHead>
void* GDataFrameQueue<TDataFrameHead>::OutMapFrame()
{
	if (!m_bReadingFlag)
	{
		return nullptr;
	}

	std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
	int iReadIdx = m_qHeadBuffer.front().iStartIdx;

	return &(m_vFrameBuffer[iReadIdx]);
}

template<typename TDataFrameHead>
void GDataFrameQueue<TDataFrameHead>::ReserveFrame()
{
	if (!m_bReadingFlag)
	{
		return;
	}

	{
		std::lock_guard<std::mutex> readingFlagLock(m_mtReadingFlagMutex);
		m_bReadingFlag = false;
	}
	m_cvReadingFlagCV.notify_one();
}

template<typename TDataFrameHead>
void GDataFrameQueue<TDataFrameHead>::ReleaseFrame()
{
	if (!m_bReadingFlag)
	{
		return;
	}

	std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);

	m_iTotalSize_Byte -= m_qHeadBuffer.front().frameHead.Size_Byte();

	m_qHeadBuffer.pop();
	{
		std::lock_guard<std::mutex> readingFlagLock(m_mtReadingFlagMutex);
		m_bReadingFlag = false;
	}
	m_cvReadingFlagCV.notify_one();
}

template<typename TDataFrameHead>
int GDataFrameQueue<TDataFrameHead>::GetWriteableIdx(int iSize_Byte)
{
	//ret: m_iHeadIdx / 0 / -1.
	//to check.

	std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
	if (m_qHeadBuffer.empty())
	{
		return 0;
	}
	if (m_qHeadBuffer.size() >= m_iMaxFrameNum)
	{
		return -1;
	}
	int iHeadIdx = m_qHeadBuffer.back().iStartIdx + m_qHeadBuffer.back().frameHead.Size_Byte();//for write.
	int iTailIdx = m_qHeadBuffer.front().iStartIdx;//for read.
	bool bWarped = (iHeadIdx <= iTailIdx);

	if (bWarped)
	{
		return ((iTailIdx - iHeadIdx) >= iSize_Byte) ? iHeadIdx : -1;
	}
	else
	{
		if ((m_vFrameBuffer.size() - iHeadIdx) >= iSize_Byte)
		{
			return iHeadIdx;
		}
		else if (iTailIdx >= iSize_Byte)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}
}

template<typename TDataFrameHead>
bool GDataFrameQueue<TDataFrameHead>::CheckShouldResize(const TDataFrameHead& frameHead)
{
	std::lock_guard<std::mutex> headBufferLock(m_mtHeadBufferMutex);
	int iNewQueueSize = CalcNewQueueSize(frameHead);
	return (m_vFrameBuffer.size() != iNewQueueSize) && ((m_iTotalSize_Byte + frameHead.Size_Byte()) <= iNewQueueSize);
}

template<typename TDataFrameHead>
int GDataFrameQueue<TDataFrameHead>::CalcNewQueueSize(const TDataFrameHead& frameHead) const
{
	return m_iMaxFrameNum * frameHead.Size_Byte();
}

template<typename TDataFrameHead>
void GDataFrameQueue<TDataFrameHead>::ResizeQueue(int iNewSize)
{
	std::vector<unsigned char> vTempQueue;
	vTempQueue.resize(iNewSize);

	std::queue<DataFrameHead_Internal, std::deque<DataFrameHead_Internal>> newHeadBuffer;
	int iDataEndIdx = 0;
	while (!m_qHeadBuffer.empty())
	{
		DataFrameHead_Internal& frameHead_Internal = m_qHeadBuffer.front();

		std::copy(m_vFrameBuffer.begin() + frameHead_Internal.iStartIdx
			, m_vFrameBuffer.begin() + frameHead_Internal.iStartIdx + frameHead_Internal.frameHead.Size_Byte()
			, vTempQueue.begin() + iDataEndIdx);

		frameHead_Internal.iStartIdx = iDataEndIdx;
		newHeadBuffer.push(frameHead_Internal);

		iDataEndIdx += frameHead_Internal.frameHead.Size_Byte();
		m_qHeadBuffer.pop();
	}

	m_vFrameBuffer.swap(vTempQueue);
	m_qHeadBuffer.swap(newHeadBuffer);
}

#endif //G_DATA_FRAME_QUEUE_H_

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
 
 
 
#ifndef G_SIMPLE_QUEUE_H_
#define G_SIMPLE_QUEUE_H_

#include <queue>
#include <mutex>
#include <functional>


template<typename TElementType>
class GSimpleQueue
{
public:
	GSimpleQueue() :
		m_bRejectWhenFull(true)
		, m_iMaxSize(0)
	{}

	void Init(int iMaxSize, bool bRejectWhenFull = true, const std::function<void(TElementType& e)>& fnRelease = std::function<void(TElementType& e)>());

	bool Front(TElementType& element);
	bool Back(TElementType& element);
	bool PushBack(const TElementType& element);
	bool PopFront();

private:
	bool m_bRejectWhenFull;
	std::function<void(TElementType& e)> m_fnRelease;

	int m_iMaxSize;
	std::queue<TElementType, std::deque<TElementType>> m_qQueue;
	std::mutex m_mtQueueMutex;
};

template<typename TElementType>
bool GSimpleQueue<TElementType>::Front(TElementType& element)
{
	std::lock_guard<std::mutex> queueLock(m_mtQueueMutex);
	if (m_qQueue.empty())
	{
		return false;
	}

	element = m_qQueue.front();
	return true;
}

template<typename TElementType>
void GSimpleQueue<TElementType>::Init(int iMaxSize, bool bRejectWhenFull, const std::function<void(TElementType& e)>& fnRelease)
{
	m_iMaxSize = iMaxSize;
	m_bRejectWhenFull = bRejectWhenFull;
	m_fnRelease = fnRelease;
}

template<typename TElementType>
bool GSimpleQueue<TElementType>::Back(TElementType& element)
{
	std::lock_guard<std::mutex> queueLock(m_mtQueueMutex);
	if (m_qQueue.empty())
	{
		return false;
	}

	element = m_qQueue.back();
	return true;
}

template<typename TElementType>
bool GSimpleQueue<TElementType>::PushBack(const TElementType& element)
{
	std::lock_guard<std::mutex> queueLock(m_mtQueueMutex);
	if (m_qQueue.size() >= m_iMaxSize)
	{
		if (m_bRejectWhenFull)
		{
			return false;
		}
		else
		{
			m_fnRelease(m_qQueue.front());
			m_qQueue.pop();
		}
	}

	m_qQueue.push(element);
	return true;
}

template<typename TElementType>
bool GSimpleQueue<TElementType>::PopFront()
{
	std::lock_guard<std::mutex> queueLock(m_mtQueueMutex);
	if (m_qQueue.empty())
	{
		return false;
	}

	m_qQueue.pop();
	return true;
}





#endif //G_SIMPLE_QUEUE_H_
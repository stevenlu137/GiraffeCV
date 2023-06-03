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
 
 
 
#ifndef G_TASK_QUEUE_H_
#define G_TASK_QUEUE_H_

#include <vector>
#include <queue>
#include <functional>

template<typename TTask>
class GTaskQueue
{
public:
	GTaskQueue() :
		m_iCurrentIdx(0)
	{}

	void Init(int iSize, std::function<void(TTask&)> fnInit);
	TTask* CreateTask();
	void CompleteTask();

	TTask* GrabTask();
	void ReleaseTask();

	void Release(std::function<void(TTask&)> fnRelease);

private:
	std::vector<TTask> m_vTasks;
	std::queue<int> m_qIdxQueue;
	int m_iCurrentIdx;
};

template<typename TTask>
void GTaskQueue<TTask>::Init(int iSize, std::function<void(TTask&)> fnInit)
{
	m_vTasks.resize(iSize);

	for (int i = 0; i < m_vTasks.size(); ++i)
	{
		fnInit(m_vTasks[i]);
	}
}

template<typename TTask>
TTask* GTaskQueue<TTask>::CreateTask()
{
	if (m_qIdxQueue.empty())
	{
		return &(m_vTasks[0]);
	}
	else if ((!m_qIdxQueue.empty()) && (m_qIdxQueue.size() < m_vTasks.size()))
	{
		return &(m_vTasks[(m_qIdxQueue.back() + 1) % m_vTasks.size()]);
	}
	else
	{
		return nullptr;
	}
}

template<typename TTask>
void GTaskQueue<TTask>::CompleteTask()
{
	if (m_qIdxQueue.empty())
	{
		m_qIdxQueue.push(0);
	}
	else if ((!m_qIdxQueue.empty()) && (m_qIdxQueue.size() < m_vTasks.size()))
	{
		m_qIdxQueue.push((m_qIdxQueue.back() + 1) % m_vTasks.size());
	}
}

template<typename TTask>
TTask* GTaskQueue<TTask>::GrabTask()
{
	if (m_vTasks.size() == m_qIdxQueue.size())
	{
		return &m_vTasks[m_qIdxQueue.front()];
	}
	else
	{
		return nullptr;
	}
}

template<typename TTask>
void GTaskQueue<TTask>::ReleaseTask()
{
	if (!m_qIdxQueue.empty())
	{
		m_qIdxQueue.pop();
	}
}

template<typename TTask>
void GTaskQueue<TTask>::Release(std::function<void(TTask&)> fnRelease)
{
	for (int i = 0; i < m_vTasks.size(); ++i)
	{
		fnRelease(m_vTasks[i]);
	}
}



#endif //G_TASK_QUEUE_H_

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
 
 
 
#ifndef G_HANDLE_MAPPER_H_
#define G_HANDLE_MAPPER_H_

#include <map>
#include <memory>
#include <mutex>


template <class TObject>
class GHandleMapper
{
public:
	GHandleMapper() :
		m_iHandle(0)
	{}

	int MakeHandle()
	{
		std::lock_guard<std::mutex> lockGuard(m_mtHandleMapMutex);
		++m_iHandle;
		m_mpHandleMap[m_iHandle]=std::shared_ptr<TObject>(new TObject());
		return (m_iHandle);
	}

	std::shared_ptr<TObject> GetPtr(int iHandle)
	{
		std::lock_guard<std::mutex> lockGuard(m_mtHandleMapMutex);
		auto itObject = m_mpHandleMap.find(iHandle);
		if (m_mpHandleMap.end() == itObject)
		{
			return std::shared_ptr<TObject>(nullptr);
		}
		return itObject->second;
	}

	void ReleaseHandle(int iHandle)
	{
		std::lock_guard<std::mutex> lockGuard(m_mtHandleMapMutex);
		m_mpHandleMap.erase(iHandle);
	}

private:
	int m_iHandle;
	std::map<int, std::shared_ptr<TObject>> m_mpHandleMap;
	std::mutex m_mtHandleMapMutex;
};


#endif //G_HANDLE_MAPPER_H_
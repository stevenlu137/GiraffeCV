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
 
 
 
#include "GCVideoSourceIF.h"

#include "Common/HandleMapper/GHandleMapper.h"
#include "../Include/GVideoSource.h"


static GHandleMapper<GVideoSource> global_handleMapper;

G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceCreate(int* piVideoSourceHandle, const G_tVideoSourceCfg* pConfig)
{
	*piVideoSourceHandle = global_handleMapper.MakeHandle();
	auto ptrVideoSource = global_handleMapper.GetPtr(*piVideoSourceHandle);

    std::string sURL(pConfig->unConfig.pcVideoSourceCfgURL);
    G_enVideoSourceError eRet = ptrVideoSource->Create(sURL);
	if (G_eVideoSourceErrorNone != eRet)
	{
		ptrVideoSource->Release();
		global_handleMapper.ReleaseHandle(*piVideoSourceHandle);
		*piVideoSourceHandle = 0;
	}
	return eRet;
}

G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceGrab(int iVideoSourceHandle, G_tVideoFrame* pFrame)
{
	return global_handleMapper.GetPtr(iVideoSourceHandle)->Grab(pFrame);
}

G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceDrop(int iVideoSourceHandle)
{
	global_handleMapper.GetPtr(iVideoSourceHandle)->Drop();
	return G_eVideoSourceErrorNone;
}

G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceRelease(int iVideoSourceHandle)
{
	auto ptrVideoSource = global_handleMapper.GetPtr(iVideoSourceHandle);
	if (nullptr == ptrVideoSource)
	{
		return G_eVideoSourceErrorNone;
	}
	
	ptrVideoSource->Release();
	global_handleMapper.ReleaseHandle(iVideoSourceHandle);
	return G_eVideoSourceErrorNone;
}

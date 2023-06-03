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
 
 
 
#ifndef G_C_VIDEO_SOURCE_IF_H_
#define G_C_VIDEO_SOURCE_IF_H_

#include "Common/DllExport/GDllExport.h"
#include "Common/DllExport/GCallConvention.h"
#include "Common/FrameFormat/GCFrameFormat.h"
#include "GCVideoSourceErrorIF.h"




#ifndef G_VIDEO_SOURCE_STATIC
#ifdef G_VIDEO_SOURCE_BUILD
#define G_VIDEO_SOURCE_API G_DLL_EXPORT
#else
#define G_VIDEO_SOURCE_API G_DLL_IMPORT
#endif// G_VIDEO_SOURCE_BUILD
#else
#define G_VIDEO_SOURCE_API
#endif// G_VIDEO_SOURCE_STATIC


enum G_enVideoSourceType
{
	G_eVideoSourceTypeNone = 0,
	G_eVideoSourceTypeURL = 1,

	G_eVideoSourceTypeMaxNum = 8192,
};


struct G_tVideoSourceCfg
{
	G_enVideoSourceType eVideoSourceType;

	union
	{
		const char* pcVideoSourceCfgURL;
	}
	unConfig;
};

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

	G_VIDEO_SOURCE_API G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceCreate(int* piVideoSourceHandle, const G_tVideoSourceCfg* pConfig);

	G_VIDEO_SOURCE_API G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceGrab(int iVideoSourceHandle, G_tVideoFrame* pFrame);

	G_VIDEO_SOURCE_API G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceDrop(int iVideoSourceHandle);

	G_VIDEO_SOURCE_API G_enVideoSourceError G_EXPORT_CALL_CONVENTION G_VideoSourceRelease(int iVideoSourceHandle);

#ifdef __cplusplus
}
#endif




#endif  //G_C_VIDEO_SOURCE_IF_H_

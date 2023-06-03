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
 
 
 
#include "PanoRender/GPanoRenderOGLIF.h"
#include "GPanoRenderOGL.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"

static int global_iPanoRenderIdx = 1;
static std::map<int, std::shared_ptr<GStitcherOGLWrapper_PanoRender>> global_mpPanoRenders;
static std::mutex global_mtPanoRendersMapMutex;

static std::mutex mtGlobalInited;
static bool bGlobalInited = false;

int G_CALL_CONVENTION G_CreatePanoRender(G_tPanoRenderConfig panoRenderConfig
	, const char* pModel, unsigned int uiModelSize_Byte, int* piPanoRenderIdx)
{
	try
	{
		GLOGGER(info) << "==> G_CreatePanoRender().";

		{
			std::lock_guard<std::mutex> globalInitedLockGuard(mtGlobalInited);
			if (!bGlobalInited)
			{
				GiraffeLogger::Init();
				bGlobalInited = true;
			}
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender =
			std::shared_ptr<GStitcherOGLWrapper_PanoRender>(new GStitcherOGLWrapper_PanoRender());

		bool bRet = ptrPanoRender->Create(panoRenderConfig, pModel, uiModelSize_Byte);

		if (bRet)
		{
			std::lock_guard<std::mutex> lockGuard(global_mtPanoRendersMapMutex);

			(*piPanoRenderIdx) = global_iPanoRenderIdx;
			global_mpPanoRenders[*piPanoRenderIdx] = ptrPanoRender;
			++global_iPanoRenderIdx;
		}

		GLOGGER(info) << "G_CreatePanoRender(). ==>";
		return (bRet ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
	}
	catch (...)
	{
		GLOGGER(error) << "G_CreatePanoRender(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int GetPanoRender(int iPanoRenderIdx, std::shared_ptr<GStitcherOGLWrapper_PanoRender>& ptrPanoRender)
{
	std::lock_guard<std::mutex> lockGuard(global_mtPanoRendersMapMutex);
	auto itPanoRender = global_mpPanoRenders.find(iPanoRenderIdx);
	if (global_mpPanoRenders.end() == itPanoRender)
	{
		return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
	}
	ptrPanoRender = itPanoRender->second;
	return G_ePanoRenderErrorCodeNone;
}

int G_CALL_CONVENTION G_InitPanoRender(int iPanoRenderIdx, void* hWnd)
{
	try
	{
		GLOGGER(info) << "==> G_InitPanoRender().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_InitPanoRender(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_InitPanoRender(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		bool bRet = ptrPanoRender->InitPanoRender(hWnd);

		GLOGGER(info) << "G_InitPanoRender(). ==>";
		return (bRet ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
	}
	catch (...)
	{
		GLOGGER(error) << "G_InitPanoRender(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GetSupportedPanoTypeList(int iPanoRenderIdx, int* pSupportedPanoTypeList, int* pSize)
{
	try
	{
		GLOGGER(info) << "==> G_GetSupportedPanoTypeList().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GetSupportedPanoTypeList(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GetSupportedPanoTypeList(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		std::vector<G_enPanoType> vList;
		bool bRet = ptrPanoRender->GetSupportedPanoTypeList(vList);
		for (int i = 0; i < vList.size(); ++i)
		{
			pSupportedPanoTypeList[i] = vList[i];
		}
		*pSize = static_cast<int>(vList.size());

		GLOGGER(info) << "G_GetSupportedPanoTypeList(). ==>";
		return (bRet ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
	}
	catch (...)
	{
		GLOGGER(error) << "G_GetSupportedPanoTypeList(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GetViewCamera(int iPanoRenderIdx, G_tViewCamera* pViewCamera)
{
	try
	{
		//GLOGGER(info) << "==> G_GetViewCamera().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GetViewCamera(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GetViewCamera(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->GetViewCamera(*pViewCamera) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		//GLOGGER(info) << "G_GetViewCamera(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_GetViewCamera(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GetOriginalPanoSize(int iPanoRenderIdx, unsigned int* puiWidth, unsigned int* puiHeight)
{
	try
	{
		GLOGGER(info) << "==> G_GetOriginalPanoSize().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GetOriginalPanoSize(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GetOriginalPanoSize(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->GetOriginalPanoSize(*puiWidth, *puiHeight) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_GetOriginalPanoSize(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_GetOriginalPanoSize(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SetViewCamera(int iPanoRenderIdx, G_tViewCamera viewCamera)
{
	try
	{
		//GLOGGER(info) << "==> G_SetViewCamera().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SetViewCamera(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SetViewCamera(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SetViewCamera(viewCamera) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
		//GLOGGER(info) << "G_SetViewCamera(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SetViewCamera(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SetDrawingRegion(int iPanoRenderIdx, unsigned int uiX0, unsigned int uiY0, unsigned int uiWidth, unsigned int uiHeight)
{
	try
	{
		GLOGGER(info) << "==> G_SetDrawingRegion().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SetDrawingRegion(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SetDrawingRegion(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SetDrawingRegion(uiX0, uiY0, uiWidth, uiHeight) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
		GLOGGER(info) << "G_SetDrawingRegion(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SetDrawingRegion(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_InputFrame(int iPanoRenderIdx, int iCameraIdx, unsigned int uiWidth, unsigned int uiHeight, void* pData[3], unsigned int uiStep_Byte[3])
{
	try
	{
		GLOGGER(info) << "==> G_InputFrame().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_InputFrame(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_InputFrame(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->InputFrame(iCameraIdx, uiWidth, uiHeight, pData, uiStep_Byte) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);
		GLOGGER(info) << "G_InputFrame(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_InputFrame(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GetCurrentPanoType(int iPanoRenderIdx, int* pCurrentPanoType)
{
	try
	{
		GLOGGER(info) << "==> G_GetCurrentPanoType().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GetCurrentPanoType(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GetCurrentPanoType(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->GetCurrentPanoType(pCurrentPanoType) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_GetCurrentPanoType(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_GetCurrentPanoType(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SwitchPanoType(int iPanoRenderIdx, int eNewPanoType)
{
	try
	{
		GLOGGER(info) << "==> G_SwitchPanoType().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SwitchPanoType(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SwitchPanoType(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SwitchPanoType(static_cast<G_enPanoType>(eNewPanoType)) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_SwitchPanoType(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SwitchPanoType(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GrabPano(int iPanoRenderIdx, unsigned int* puiWidth, unsigned int* puiHeight, unsigned char* pRGBData)
{
	try
	{
		GLOGGER(info) << "==> G_GrabPano().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GrabPano(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GrabPano(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->GrabPano(puiWidth, puiHeight, pRGBData) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_GrabPano(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_GrabPano(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SetOutputCallBack(int iPanoRenderIdx, G_tOutputCallBack outputCallBackFunc, float fScale, void* pContext)
{
	try
	{
		GLOGGER(info) << "==> G_SetOutputCallBack().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SetOutputCallBack(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		if (fScale <= 0.0)
		{
			return G_ePanoRenderErrorCodeUnknown;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SetOutputCallBack(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SetOutputCallBack(outputCallBackFunc, fScale, pContext) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_SetOutputCallBack(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SetOutputCallBack(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SetPreRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext)
{
	try
	{
		GLOGGER(info) << "==> G_SetPreRenderCallBack().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SetPreRenderCallBack(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SetPreRenderCallBack(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SetPreRenderCallBack(renderCallBackFunc, pContext) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_SetPreRenderCallBack(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SetPreRenderCallBack(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_SetRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext)
{
	try
	{
		GLOGGER(info) << "==> G_SetRenderCallBack().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_SetRenderCallBack(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_SetRenderCallBack(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->SetRenderCallBack(renderCallBackFunc, pContext) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_SetRenderCallBack(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_SetRenderCallBack(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_UnProject(int iPanoRenderIdx, G_tNormalizedPixelCoordinate* pPoints, unsigned int uiPointsSize, G_tPhysicalDirection* pPhysicalDirection)
{
	try
	{
		GLOGGER(info) << "==> G_UnProject().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_UnProject(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_UnProject(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->UnProject(pPoints, uiPointsSize, pPhysicalDirection) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_UnProject(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_UnProject(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_CalcViewCamera(int iPanoRenderIdx, G_tPhysicalDirection pPhysicalDirection, G_tViewCamera* pViewCamera)
{
	try
	{
		GLOGGER(info) << "==> G_CalcViewCamera().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_CalcViewCamera(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_CalcViewCamera(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->CalcViewCamera(pPhysicalDirection, pViewCamera) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_CalcViewCamera(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_CalcViewCamera(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_GetCameraParameters(int iPanoRenderIdx, int iCameraIdx, char* sCameraMode, float* fK, float* fDistortion, float* fR, G_enCameraModelType* peCameraModelType)
{
	try
	{
		GLOGGER(info) << "==> G_GetCameraParameters().";
		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_GetCameraParameters(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_GetCameraParameters(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		int iRet = (ptrPanoRender->GetCameraParameters(iCameraIdx, sCameraMode, fK, fDistortion, fR, peCameraModelType) ? G_ePanoRenderErrorCodeNone : G_ePanoRenderErrorCodeUnknown);

		GLOGGER(info) << "G_GetCameraParameters(). ==>";
		return iRet;
	}
	catch (...)
	{
		GLOGGER(error) << "G_GetCameraParameters(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}

int G_CALL_CONVENTION G_DestroyPanoRender(int iPanoRenderIdx)
{
	try
	{
		GLOGGER(info) << "==> G_DestroyPanoRender().";

		if (iPanoRenderIdx <= 0)
		{
			GLOGGER(error) << "G_DestroyPanoRender(). ERROR: Invalid PanoRender index.";
			return G_ePanoRenderErrorCodeInvalidPanoRenderIdx;
		}

		std::shared_ptr<GStitcherOGLWrapper_PanoRender> ptrPanoRender;
		int iFound = GetPanoRender(iPanoRenderIdx, ptrPanoRender);
		if (G_ePanoRenderErrorCodeNone != iFound)
		{
			GLOGGER(error) << "G_DestroyPanoRender(). ERROR: Invalid PanoRender index.";
			return iFound;
		}

		ptrPanoRender->DestroyPanoRender();

		std::lock_guard<std::mutex> lockGuard(global_mtPanoRendersMapMutex);
		global_mpPanoRenders.erase(iPanoRenderIdx);
		
		GLOGGER(info) << "G_DestroyPanoRender(). ==>";

		GLOGGER_RELEASE();

		return G_ePanoRenderErrorCodeNone;
	}
	catch (...)
	{
		GLOGGER(error) << "G_DestroyPanoRender(). EXCEPTION. ==>";
		return G_ePanoRenderErrorCodeUnknown;
	}
}



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
 
 
 
#include "GPanoRenderOGL.h"

#include <bitset>
#include <sstream>

#include "opencv2/highgui/highgui.hpp"

#include "GFileUtils.h"
#include "GGlobalParametersManager.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "GGLError.h"


#if defined WIN32
#include "Windows.h"
#elif __APPLE__
#elif __ANDROID__
#elif __linux__
#endif // WIN32

struct GGrabCallBackContext
{
	GGrabCallBackContext() :
		uiWidth(0)
		, uiHeight(0)
		, pContext(nullptr)
		, bTaskDone(false)
	{}

	unsigned int uiWidth;
	unsigned int uiHeight;
	unsigned char* pContext;
	bool bTaskDone;
	std::mutex mtTaskMutex;
	std::condition_variable cvTaskDone;
};

void G_CALL_CONVENTION GrabCallBack(unsigned int uiWidth, unsigned int uiHeight, const unsigned char* pBGRData, void* pContext)
{
	GGrabCallBackContext* pGrabContext = (GGrabCallBackContext*)(pContext);

	unsigned int uiWidth4n = (0 != (uiWidth % 4)) ? (uiWidth / 4) * 4 + 4 : (uiWidth / 4) * 4;

	if (((uiWidth4n * uiHeight) > (pGrabContext->uiWidth * pGrabContext->uiHeight)) ||
		(0 == uiWidth || 0 == uiHeight) ||
		(nullptr == pBGRData))
	{
		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GrabCallBack(). Grab Error. Pano width: "
			<< uiWidth4n << ", pano height: " << uiHeight
			<< ". buffer width: " << pGrabContext->uiWidth << ", buffer height: " << pGrabContext->uiHeight << ". ==>";

		pGrabContext->pContext = nullptr;
		pGrabContext->uiWidth = uiWidth4n;
		pGrabContext->uiHeight = uiHeight;

		{
			std::lock_guard<std::mutex> lk(pGrabContext->mtTaskMutex);
			pGrabContext->bTaskDone = true;
		}
		pGrabContext->cvTaskDone.notify_one();

		return;
	}

	pGrabContext->uiWidth = uiWidth4n;
	pGrabContext->uiHeight = uiHeight;

	for (int iLine = 0; iLine < uiHeight; ++iLine)
	{
		const unsigned char* pSrc = pBGRData + uiWidth * iLine * 3;
		unsigned char* pDst = pGrabContext->pContext + uiWidth4n * iLine * 3;
		std::copy(pSrc, pSrc + uiWidth * 3, pDst);
		std::memset(pDst + uiWidth * 3, 0, (uiWidth4n - uiWidth) * 3);
	}

	{
		std::lock_guard<std::mutex> lk(pGrabContext->mtTaskMutex);
		pGrabContext->bTaskDone = true;
	}
	pGrabContext->cvTaskDone.notify_one();
}


unsigned long GStitcherOGLWrapper_PanoRender::GetTasks()
{
	std::bitset<sizeof(unsigned long) * 8> bsRet;
	bsRet.reset();

	if (!m_bRendering)
	{
		bsRet.set(ePanoRenderTaskDestroy);
		return bsRet.to_ulong();
	}

	if (ePanoRenderStateError == m_ePanoRenderState)
	{
		bsRet.set(ePanoRenderTaskDestroy);
		return bsRet.to_ulong();
	}
	else if (ePanoRenderStateUnCreated == m_ePanoRenderState)
	{
		return bsRet.to_ulong();
	}
	//Task InitGL.
	else if (ePanoRenderStateCreated == m_ePanoRenderState)
	{
		std::unique_lock<std::mutex> lk(m_mtHWNDMutex);
		m_cvInitFlag.wait(lk, [this]()
		{
			return ((!this->m_bRendering) || m_bInitCalled);
		});

		bsRet.set((m_bRendering) ? ePanoRenderTaskInit : ePanoRenderTaskDestroy);
	}
	//Task CreateScene & SetViewPort & SetOriginalPanoSize.
	else if (ePanoRenderStateInited <= m_ePanoRenderState)
	{
		if (!m_bDataArrived)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetTasks(). Data Not Arrived. ==>";
			return bsRet.to_ulong();
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			m_RenderParameterManager.ApplyParameters();
		}

		if (m_RenderParameterManager.ShouldCreateScene())
		{
			bsRet.set(ePanoRenderTaskCreateScene);
		}

		if (m_RenderParameterManager.ShouldSetViewPort())
		{
			bsRet.set(ePanoRenderTaskSetViewPort);
		}

		if (m_RenderParameterManager.ShouldSetOriginalPanoSize())
		{
			bsRet.set(ePanoRenderTaskSetOriginalPanoSize);
		}

		if (m_UnprojectContext.bNeedUnProject)
		{
			bsRet.set(ePanoRenderTaskUnProject);
		}

		bsRet.set(ePanoRenderTaskRender);
	}

	return bsRet.to_ulong();
}

void GStitcherOGLWrapper_PanoRender::RenderingLoop(GStitcherOGLWrapper_PanoRender* pPanoRender)
{
	try
	{
		GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::RenderingLoop().";

		while (true)
		{
			unsigned long ulTasks = 0;
			ulTasks = pPanoRender->GetTasks();

			if (0 == ulTasks)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
				continue;
			}

			if (ulTasks & (1 << ePanoRenderTaskInit))
			{
				if (!pPanoRender->InitOpenGL())
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). InitPanoRender ERROR, Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}

				if (!pPanoRender->InitStitcher())
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Init StitcherOGL ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}

				pPanoRender->m_ePanoRenderState = ePanoRenderStateInited;
			}

			if (ulTasks & (1 << ePanoRenderTaskCreateScene))
			{
				GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Should Create Scene with new size. ";

				std::lock_guard<std::mutex> lockGuard(pPanoRender->m_mtStitcherOGLMutex);

				if (!pPanoRender->m_stitcherOGL.CreateScene("default", pPanoRender->m_RenderParameterManager.GetPanoType()
					, pPanoRender->m_RenderParameterManager.GetK0Name(), pPanoRender->m_RenderParameterManager.GetCropRatios()))
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Create scene ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}

				if (!pPanoRender->m_stitcherOGL.SwitchScene("default"))
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Switch scene ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}

				pPanoRender->m_ePanoRenderState = ePanoRenderStateSceneCreated;
			}

			if (ulTasks & (1 << ePanoRenderTaskSetViewPort))
			{
				cv::Rect rect = pPanoRender->m_RenderParameterManager.GetDrawRegion();

				{
					std::lock_guard<std::mutex> lockGuard(pPanoRender->m_mtStitcherOGLMutex);
					pPanoRender->m_FBODisplay.SetViewPort(rect.tl().x, rect.tl().y, rect.width, rect.height);
				}
			}

			if (ulTasks & (1 << ePanoRenderTaskSetOriginalPanoSize))
			{
				cv::Size szOriginalPanoSize = pPanoRender->m_RenderParameterManager.ReadOriginalPanoSize();

				{
					std::lock_guard<std::mutex> lockGuard(pPanoRender->m_mtStitcherOGLMutex);
					pPanoRender->m_iPanoOriginalWidth = szOriginalPanoSize.width;
					pPanoRender->m_iPanoOriginalHeight = szOriginalPanoSize.height;
				}
			}

			if (ulTasks & (1 << ePanoRenderTaskRender))
			{
				auto start = std::chrono::high_resolution_clock::now();

				if (!pPanoRender->Render())
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Render ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}

				if (ulTasks & (1 << ePanoRenderTaskUnProject))
				{
					{
						std::lock_guard<std::mutex> lockGuard(pPanoRender->m_mtStitcherOGLMutex);
						for (int i = 0; i < pPanoRender->m_UnprojectContext.vfInputPoints.size(); ++i)
						{
							cv::Point2f& ptSrc = pPanoRender->m_UnprojectContext.vfInputPoints[i];
							cv::Point3f& ptDst = pPanoRender->m_UnprojectContext.vfOutputPoints[i];

							float fDepth = 0.0;
							glm::vec4 mvViewPort;
							cv::Point2i ptPix = cv::Point2i(0, 0);
							if (GStitcherConfig::eRenderingModeOnScreen == pPanoRender->m_Config.m_eRenderingMode)
							{
								fDepth = pPanoRender->m_FBODisplay.GetDepth(ptSrc.x, ptSrc.y);
								mvViewPort = pPanoRender->m_FBODisplay.GetViewPort();
								ptPix = pPanoRender->m_FBODisplay.NormalizedPix2Pix(ptSrc);
							}
							else
							{
								fDepth = pPanoRender->m_downloaders.GrabTask()->GetDepth(ptSrc.x, ptSrc.y);
								mvViewPort = pPanoRender->m_downloaders.GrabTask()->GetViewPort();
								ptPix = pPanoRender->m_downloaders.GrabTask()->NormalizedPix2Pix(ptSrc);
							}

							pPanoRender->m_UnprojectContext.bRet =
								pPanoRender->m_stitcherOGL.UnProjectPix2Physical(ptPix.x, ptPix.y, fDepth
								, ptDst.x, ptDst.y, ptDst.z
								, mvViewPort);
						}
					}

					{
						std::lock_guard<std::mutex> lk(pPanoRender->m_UnprojectContext.mtTaskMutex);
						pPanoRender->m_UnprojectContext.bNeedUnProject = false;
					}
					pPanoRender->m_UnprojectContext.cvTaskDone.notify_one();
				}

#ifdef WIN32
				if ((GStitcherConfig::eRenderingModeOnScreen == pPanoRender->m_Config.m_eRenderingMode) &&
					(!SwapBuffers(pPanoRender->m_HDC)))
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Swap buffers ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
				if ((GStitcherConfig::eRenderingModeOnScreen == pPanoRender->m_Config.m_eRenderingMode) &&
					(EGL_FALSE == eglSwapBuffers(pPanoRender->m_Display, pPanoRender->m_Surface)))
				{
					GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). Swap buffers ERROR. Terminating.";
					pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
					break;
				}
#endif // WIN32

				auto end = std::chrono::high_resolution_clock::now();
				auto sleepFor = (std::chrono::milliseconds(1000 / pPanoRender->m_iFPS) - std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count();
				sleepFor = (sleepFor < 0) ? 0 : sleepFor;
				std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
			}

			if (ulTasks & (1 << ePanoRenderTaskDestroy))
			{
				break;
			}
		}//while

		pPanoRender->Release();

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::RenderingLoop() ==>";
	}
	catch (...)
	{
		pPanoRender->m_ePanoRenderState = ePanoRenderStateError;
		pPanoRender->Release();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::RenderingLoop(). EXCEPTION. ==>";
		return;
	}
}

bool GStitcherOGLWrapper_PanoRender::Create(G_tPanoRenderConfig panoRenderConfig, const char* pModel, unsigned int uiSize_Byte)
{
	try
	{
		GPanoCameraProfile panoCameraInfo;
		//LoadGraphFromFileBin("E:\\model_5_360_2.bin", mGraph);
		LoadObjectFromFileBinRawData(pModel, static_cast<int>(uiSize_Byte), panoCameraInfo);
		SortByCameraName(panoCameraInfo.vCameraModels);

		if (panoCameraInfo.vPanoTypeList.empty())
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Create(). pano type list is empty. Create failed.";
			return false;
		}

		TransPanoCameraProfile2CameraModel(panoCameraInfo, m_vCameras);

		m_RenderParameterManager.Init(panoCameraInfo.vPanoTypeList, panoCameraInfo.vCameraModels.size());

		//Set up config parameters.
		switch (panoRenderConfig.eInputFormat)
		{
		case G_eInputFormatRGB:
			m_Config.m_eInputFrameFormat = eFrameFormatRGB;
			break;
		case G_eInputFormatYUV420P:
			m_Config.m_eInputFrameFormat = eFrameFormatYUV420P;
			break;
		case G_eInputFormatYV12:
			m_Config.m_eInputFrameFormat = eFrameFormatYV12;
			break;
		case G_eInputFormatNV12:
			m_Config.m_eInputFrameFormat = eFrameFormatNV12;
			break;
		default:
			break;
		}

		switch (panoRenderConfig.eRenderingMode)
		{
		case G_eRenderingModeOnScreen:
			m_Config.m_eRenderingMode = GStitcherConfig::eRenderingModeOnScreen;
			break;
		case G_eRenderingModeOffScreen:
			m_Config.m_eRenderingMode = GStitcherConfig::eRenderingModeOffScreen;
			break;
		case G_eRenderingModeOffScreenRealTime:
			m_Config.m_eRenderingMode = GStitcherConfig::eRenderingModeOffScreenRealTime;
			break;
		default:
			break;
		}

		m_Config.m_eOptionUploading = GStitcherConfig::eOptionUploadingPBO;
		m_Config.m_eOptionDownloading = GStitcherConfig::eOptionDownloadingPBO;


		m_ePanoRenderState = ePanoRenderStateCreated;

		m_iFPS = 30;
		m_tdRendering = std::thread(GStitcherOGLWrapper_PanoRender::RenderingLoop, this);

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Create(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::InitPanoRender(void* hWnd)
{
	try
	{
		if (m_ePanoRenderState != ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitPanoRender(). Not Ready or already Inited. ==>";
			return false;
		}

		{
			std::lock_guard<std::mutex> lk(m_mtHWNDMutex);
			m_HWND = hWnd;
		    m_bInitCalled = true;
		}
		m_cvInitFlag.notify_one();

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitPanoRender(). EXCEPTION. ==>";
		return false;
	}
}

#ifdef WIN32

bool GStitcherOGLWrapper_PanoRender::InitOpenGL()
{
	try
	{
		m_HDC = GetDC((HWND)m_HWND);
		if (nullptr == m_HDC)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). GetDC FAILED.";
			return false;
		}

		m_GLContext = wglCreateContext(m_HDC);
		if (nullptr == m_GLContext)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Create OpenGL context FAILED.";
			return false;
		}

		if (!wglMakeCurrent(m_HDC, m_GLContext))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Bind OpenGL Context FAILED. ==>";
			return false;
		}

		if (glewInit() != GLEW_OK)
		{
			m_ePanoRenderState = ePanoRenderStateError;
			GLOGGER(error) << "GLEW init failed. ";
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). ==>";
			return false;
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). OpenGL Inited. ==>";
		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		Release();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). EXCEPTION. ==>";
		return false;
	}
}
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
bool GStitcherOGLWrapper_PanoRender::GetDisplay(void* pNativeWindow)
{
	//todo. How to get Display of specified pNativeWindow?
	m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if (EGL_NO_DISPLAY == m_Display)
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetDisplay(). eglGetDisplay FAILED. EGL Error: "
			<< eglGetError();
		return false;
	}

	EGLint iMajorVersion = 0;
	EGLint iMinorVersion = 0;
	if (EGL_FALSE == eglInitialize(m_Display, &iMajorVersion, &iMinorVersion))
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetDisplay(). eglInitialize FAILED. EGL Error: "
			<< eglGetError();
		return false;
	}

	GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). EGL Version: " << iMajorVersion << "." << iMinorVersion;

	if (!((iMajorVersion > 1) || ((1 == iMajorVersion) && (iMinorVersion >= 4))))
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetDisplay(). Error: EGL Version should be 1.4 or later.";
		return false;
	}

	return true;
}

bool GStitcherOGLWrapper_PanoRender::GetConfig(EGLConfig& eglConfig)
{
#ifdef __ANDROID__
	std::vector<EGLint>  vCfgAttribs =
	{
		EGL_SURFACE_TYPE, (GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode) ? EGL_WINDOW_BIT : EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_DEPTH_SIZE, 1,
		EGL_NONE
	};
#elif __linux__
	std::vector<EGLint> vCfgAttribs =
	{
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_RED_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_BLUE_SIZE, 5,
		EGL_DEPTH_SIZE, 1,
		EGL_NONE
	};

	if(GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode)
	{
		vCfgAttribs[vCfgAttribs.size() - 1] = EGL_SURFACE_TYPE;
		vCfgAttribs.push_back(EGL_WINDOW_BIT);
		vCfgAttribs.push_back(EGL_NONE);
	}

#endif

	EGLint iNumConfigs = 0;
	if (EGL_FALSE == eglGetConfigs(m_Display, NULL, 0, &iNumConfigs))
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetConfig(). eglGetConfigs FAILED. EGL Error: "
			<< eglGetError();
		return false;
	}

	if (EGL_FALSE == eglChooseConfig(m_Display, &(vCfgAttribs[0]), &eglConfig, 1, &iNumConfigs))
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetConfig(). eglChooseConfig FAILED. EGL Error: "
			<< eglGetError();
		return false;
	}

	return true;
}

bool GStitcherOGLWrapper_PanoRender::GetSurface(EGLConfig eglConfig, void* pNativeWindow)
{
	if (GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode)
	{
		m_Surface = eglCreateWindowSurface(m_Display, eglConfig, (EGLNativeWindowType)pNativeWindow, NULL);
		if (EGL_NO_SURFACE == m_Surface)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetSurface(). eglCreateWindowSurface FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}
	}
	else
	{
		m_Surface = EGL_NO_SURFACE;
	}

	return true;
}

bool GStitcherOGLWrapper_PanoRender::InitOpenGL()
{
	try
	{
		if (!GetDisplay((void*)(m_HWND)))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Error: GetDisplay FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}

		EGLConfig eglConfig;
		if (!GetConfig(eglConfig))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Error: GetConfig FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}

		if (!GetSurface(eglConfig, (void*)(m_HWND)))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Error: GetSurface FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}

#ifdef __ANDROID__
#elif __linux__
		if (!eglBindAPI(EGL_OPENGL_API))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). Error: EGL_OPENGL_API is not supported. EGL Error: "
				<< eglGetError();
			return false;
		}
#endif

#ifdef __ANDROID__
		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
#elif __linux__
		EGLint contextAttribs[] = { EGL_NONE };
#endif
		m_GLContext = eglCreateContext(m_Display, eglConfig, EGL_NO_CONTEXT, contextAttribs);
		if (EGL_NO_CONTEXT == m_GLContext)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). eglCreateContext FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). OpenGL context Created.";

		if (EGL_FALSE == eglMakeCurrent(m_Display, m_Surface, m_Surface, m_GLContext))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). eglMakeCurrent FAILED. EGL Error: "
				<< eglGetError();
			return false;
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). GLEW Initializing.";

		if (glewInit() != GLEW_OK)
		{
			m_ePanoRenderState = ePanoRenderStateError;
			GLOGGER(error) << "GLEW init failed. ";
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). ==>";
			return false;
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). OpenGL Inited. ==>";
		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		Release();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitOpenGL(). EXCEPTION. ==>";
		return false;
	}
}
#endif // WIN32

bool GStitcherOGLWrapper_PanoRender::InitStitcher()
{
	try
	{
		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitStitcher().";

		std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);

		if (!m_stitcherOGL.Init(m_Config, m_vCameras))
		{
			GLOGGER(error) << "StitcherOGL init failed. ";
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitStitcher(). ==>";
			return false;
		}

		m_uploader.Init(m_Config.m_eOptionUploading, m_Config.m_eInputFrameFormat, m_vCameras.size());

		m_downloaders.Init((GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode) ? 1 : 3, [](GFrameDownloader& downloader)
		{
			downloader.Init(10, 10);
		});

		m_FBODisplay.Create(eGOpenGLFrameBufferTypeDisplay);

		m_vPtrFrameQueues.resize(m_vCameras.size());
		for (int iQue = 0; iQue < m_vPtrFrameQueues.size(); ++iQue)
		{
			m_vPtrFrameQueues[iQue] = GFrameQueueFactory::CreateFrameQueue();
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InitStitcher(). ==>";
		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		Release();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InitStitcher(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::UploadTextures()
{
	static bool bHasTextureGroup = false;

	std::vector<std::shared_ptr<GFrameSourceIF>> vFrameQueSources(m_vCameras.size());
	for (int iQue = 0; iQue < m_vPtrFrameQueues.size(); ++iQue)
	{
		vFrameQueSources[iQue] = m_vPtrFrameQueues[iQue];
	}

	//Upload textures.
	bHasTextureGroup |= m_uploader.Upload(vFrameQueSources, m_textureGroup);
	return bHasTextureGroup;
}

bool GStitcherOGLWrapper_PanoRender::Render()
{
	GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::Render().";

	std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);

	if (m_stitcherOGL.IsSceneExist(""))
	{
		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::Render(). Render pano.";
		GViewCamera4RenderIF* pViewCamera = m_RenderParameterManager.GetViewCamera();


		if (!UploadTextures())
		{
			return false;
		}

		if (GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode)
		{
			//Online.
			if (!m_mpWriteOutCallBack.empty())
			{
				if (opengl_compatible(3, 0))
				{
					return RenderWithOutput(pViewCamera->CalcP(), pViewCamera->CalcV());
				}
				else
				{
					return RenderWithOutput4EarlyVersion(pViewCamera->CalcP(), pViewCamera->CalcV());
				}
			}
			else
			{
				return RenderOnly(pViewCamera->CalcP(), pViewCamera->CalcV());
			}
		}
		else
		{
			//Offline.
			return RenderWithOutput(pViewCamera->CalcP(), pViewCamera->CalcV());
		}

		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::Render(). ==>";
	}

	GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::Render(). ==>";
	return true;
}

bool GStitcherOGLWrapper_PanoRender::RenderOnly(const glm::mat4& matP, const glm::mat4& matV)
{
	GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::RenderOnly().";

	//Prepare for rendering.
	m_FBODisplay.Bind();
	m_FBODisplay.SetViewPort();
	m_FBODisplay.Clear();

	bool bRet = m_stitcherOGL.RenderPano(m_textureGroup, matP, matV);

	GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::RenderOnly(). ==>";
	return bRet;
}

bool GStitcherOGLWrapper_PanoRender::RenderWithOutput(const glm::mat4& matP, const glm::mat4& matV)
{
	GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::RenderWithOutput().";

	//Render and start downloading.
	GFrameDownloader* pNewDownloader = m_downloaders.CreateTask();
	if (nullptr != pNewDownloader)
	{
		//Prepare for rendering.
		pNewDownloader->Bind();
		if (!pNewDownloader->ChangeFrameSize(m_iPanoOriginalWidth, m_iPanoOriginalHeight))
		{
			GLOGGER(error) << "GStitcherOGL::RenderWithOutput(). Change original pano buffer size Failed. ==>";
			return false;
		}
		pNewDownloader->SetViewPort(0, 0, m_iPanoOriginalWidth, m_iPanoOriginalHeight);
		pNewDownloader->Clear();


		//Render.
		bool bRet = m_stitcherOGL.RenderPano(m_textureGroup, matP, matV);

		//Start downloading.
		for (auto itCallBack = m_mpWriteOutCallBack.begin(); itCallBack != m_mpWriteOutCallBack.end(); ++itCallBack)
		{
			GWriteOutCallBack& task = itCallBack->second;
			if (!pNewDownloader->IsActivated(task.fScale))
			{
				pNewDownloader->StartDownload(task.fScale, m_Config.m_eOptionDownloading);
			}
		}

		m_downloaders.CompleteTask();
	}

	//Output & Display.
	GFrameDownloader* pDownloader = m_downloaders.GrabTask();
	if (nullptr != pDownloader)
	{
		OutputCallBackwithScale(pDownloader);

		if (GStitcherConfig::eRenderingModeOnScreen == m_Config.m_eRenderingMode)
		{
			m_FBODisplay.Bind();
			m_FBODisplay.SetViewPort();
			pDownloader->GetFBO() >> m_FBODisplay;
		}

		m_downloaders.ReleaseTask();
	}

	GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::RenderWithOutput(). ==>";
	return true;
}

bool GStitcherOGLWrapper_PanoRender::RenderWithOutput4EarlyVersion(const glm::mat4& matP, const glm::mat4& matV)
{
	GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::RenderWithOutput4EarlyVersion().";

	//Prepare for rendering.
	m_FBODisplay.Bind();
	m_FBODisplay.SetViewPort();
	m_FBODisplay.Clear();

	//Render.
	bool bRet = m_stitcherOGL.RenderPano(m_textureGroup, matP, matV);

	//Output.
	m_mPanoBuffer.create(m_FBODisplay.ViewPortHeight(), m_FBODisplay.ViewPortWidth(), CV_8UC3);
	m_FBODisplay >> m_mPanoBuffer;
	OutputCallBack(m_mPanoBuffer.data, m_mPanoBuffer.cols, m_mPanoBuffer.rows);

	GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::RenderWithOutput4EarlyVersion(). ==>";
	return true;
}

void GStitcherOGLWrapper_PanoRender::OutputCallBack(unsigned char* pData, int iWidth, int iHeight)
{
	auto itCallBack = m_mpWriteOutCallBack.begin();
	while (itCallBack != m_mpWriteOutCallBack.end())
	{
		GWriteOutCallBack& task = itCallBack->second;
		if (1 < task.iCallBackTimes)
		{
			(*(task.pCallBackFunc))(iWidth, iHeight, pData, task.pContext);
			--task.iCallBackTimes;
		}
		else if (1 == task.iCallBackTimes)
		{
			(*(task.pCallBackFunc))(iWidth, iHeight, pData, task.pContext);
			auto itToErase = itCallBack;
			++itCallBack;
			m_mpWriteOutCallBack.erase(itToErase);
			continue;
		}
		else if (0 == task.iCallBackTimes)
		{
			auto itToErase = itCallBack;
			++itCallBack;
			m_mpWriteOutCallBack.erase(itToErase);
			continue;
		}
		else if (0 > task.iCallBackTimes)
		{
			(*(task.pCallBackFunc))(iWidth, iHeight, pData, task.pContext);
		}

		++itCallBack;
	}
}

void GStitcherOGLWrapper_PanoRender::OutputCallBackwithScale(GFrameDownloader* pDownloader)
{
	std::vector<float> vProcessedScales;
	std::vector<std::map <std::string, GWriteOutCallBack>::iterator> vIterators2Del;

	for (auto it = m_mpWriteOutCallBack.begin(); it != m_mpWriteOutCallBack.end(); ++it)
	{
		//avoid process the same scale value again.
		if (vProcessedScales.end() != std::find(vProcessedScales.begin(), vProcessedScales.end(), it->second.fScale))
		{
			continue;
		}

		unsigned char* pData = pDownloader->MapFrame(it->second.fScale);
		if (nullptr == pData)
		{
			vProcessedScales.push_back(it->second.fScale);
			continue;
		}

		//find all callbacks with the same scale value.
		for (auto itt = m_mpWriteOutCallBack.begin(); itt != m_mpWriteOutCallBack.end(); ++itt)
		{
			GWriteOutCallBack& task = itt->second;

			if (it->second.fScale != task.fScale)
			{
				continue;
			}

			if (1 < task.iCallBackTimes)
			{
				(*(task.pCallBackFunc))(pDownloader->Width(task.fScale), pDownloader->Height(task.fScale), pData, task.pContext);
				--task.iCallBackTimes;
			}
			else if (1 == task.iCallBackTimes)
			{
				(*(task.pCallBackFunc))(pDownloader->Width(task.fScale), pDownloader->Height(task.fScale), pData, task.pContext);
				vIterators2Del.push_back(itt);
			}
			else if (0 == task.iCallBackTimes)
			{
				vIterators2Del.push_back(itt);
			}
			else if (0 > task.iCallBackTimes)
			{
				(*(task.pCallBackFunc))(pDownloader->Width(task.fScale), pDownloader->Height(task.fScale), pData, task.pContext);
			}
		}

		vProcessedScales.push_back(it->second.fScale);

		pDownloader->UnmapFrame(it->second.fScale);
	}

	for (int i = 0; i < vIterators2Del.size(); ++i)
	{
		m_mpWriteOutCallBack.erase(vIterators2Del[i]);
	}
}

void GStitcherOGLWrapper_PanoRender::Release()
{
	if (nullptr != m_GLContext)
	{
		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);
			m_stitcherOGL.Release();

			m_uploader.Release();

			m_downloaders.Release([](GFrameDownloader& downloader)
			{
				downloader.Release();
			});
		}
	}

#ifdef WIN32
	if (nullptr != m_HDC)
	{
		if (!wglMakeCurrent(m_HDC, nullptr))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). Unbind OpenGL context FAILED.";
		}
	}

	if ((nullptr != m_HWND) && (nullptr != m_HDC))
	{
		if (1 != ReleaseDC((HWND)m_HWND, m_HDC))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). ReleaseDC FAILED.";
		}
		else
		{
			m_HDC = nullptr;
		}
	}

	if (nullptr != m_GLContext)
	{
		if (!wglDeleteContext(m_GLContext))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). Delete OpenGL Context FAILED.";
		}
		else
		{
			m_GLContext = nullptr;
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::Release(). OpenGL Context Deleted.";
		}
	}

	m_HWND = nullptr;
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
	if (m_Display != EGL_NO_DISPLAY)
	{
		if (EGL_FALSE == eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). Unbind OpenGL context FAILED.";
		}

		if (m_GLContext != EGL_NO_CONTEXT)
		{
			if (EGL_FALSE == eglDestroyContext(m_Display, m_GLContext))
			{
				GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). Delete OpenGL Context FAILED.";
			}
			else
			{
				m_GLContext = EGL_NO_CONTEXT;
			}
		}

		if (m_Surface != EGL_NO_SURFACE)
		{
			if (EGL_FALSE == eglDestroySurface(m_Display, m_Surface))
			{
				GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). Delete Surface FAILED.";
			}
			else
			{
				m_Surface = EGL_NO_SURFACE;
			}
		}

		if (EGL_FALSE == eglTerminate(m_Display))
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::Release(). eglTerminate() FAILED.";
		}
		else
		{
			m_Display = EGL_NO_DISPLAY;
		}
	}

	m_HWND = nullptr;
#endif // WIN32
}

bool GStitcherOGLWrapper_PanoRender::GetSupportedPanoTypeList(std::vector<G_enPanoType>& vSupportedPanoTypeList)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetSupportedPanoTypeList(). Not Ready. ==>";
			return false;
		}

		vSupportedPanoTypeList.clear();
		std::vector<enPanoType> vList = m_RenderParameterManager.GetSupportedPanoTypeList();
		for (int i = 0; i < vList.size(); ++i)
		{
			switch (vList[i])
			{
			case ePanoTypeImmersionSemiSphere:
				vSupportedPanoTypeList.push_back(G_ePanoTypeImmersionSemiSphere);
				break;
			case ePanoTypeUnwrappedCylinder180:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinder180);
				break;
			case ePanoTypeUnwrappedCylinder360:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinder360);
				break;
			case ePanoTypeImmersionCylinder360:
				vSupportedPanoTypeList.push_back(G_ePanoTypeImmersionCylinder360);
				break;
			case ePanoTypeImmersionCylinder360Inside:
				vSupportedPanoTypeList.push_back(G_ePanoTypeImmersionCylinder360Inside);
				break;
			case ePanoTypeUnwrappedCylinderSplited:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinderSplited);
				break;
			case ePanoTypeUnwrappedCylinderSplitedFrontBack:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinderSplitedFrontBack);
				break;
			case ePanoTypeUnwrappedSphere180:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedSphere180);
				break;
			case ePanoTypeUnwrappedSphere360:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedSphere360);
				break;
			case ePanoTypeUnwrappedSphere360Flip:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedSphere360Flip);
				break;
			case ePanoTypeUnwrappedCylinder180Flip:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinder180Flip);
				break;
			case ePanoTypeUnwrappedCylinder360Flip:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedCylinder360Flip);
				break;
			case ePanoTypeImmersionFullSphere:
				vSupportedPanoTypeList.push_back(G_ePanoTypeImmersionFullSphere);
				break;
			case ePanoTypeUnwrappedFullSphere360:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedFullSphere360);
				break;
			case ePanoTypeUnwrappedFullSphere360Flip:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedFullSphere360Flip);
				break;
			case ePanoTypeUnwrappedFullSphere360Binocular:
				vSupportedPanoTypeList.push_back(G_ePanoTypeUnwrappedFullSphere360Binocular);
				break;
			case ePanoTypeImmersionFullSphereInside:
				vSupportedPanoTypeList.push_back(G_ePanoTypeImmersionFullSphereInside);
				break;
			case ePanoTypeOriginal:
				vSupportedPanoTypeList.push_back(G_ePanoTypeOriginal);
				break;
			default:
				//todo.
				break;
			}
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetSupportedPanoTypeList(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::GetViewCamera(G_tViewCamera& viewCamera)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetViewCamera(). Not Ready. ==>";
			return false;
		}

		GInteractionControllerDOF dof;
		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			dof = m_RenderParameterManager.ReadInteractionControllerDOF();
		}

		viewCamera.fHorizontal = dof.fHorizontal;
		viewCamera.fVertical = dof.fVertical;
		viewCamera.fZoom = dof.fZoom;

		//GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::GetViewCamera(). Pan: " << viewCamera.fPan
		//	<< ", Tilt: " << viewCamera.fTilt << ", Zoom: " << viewCamera.fZoom;

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetViewCamera(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::GetOriginalPanoSize(unsigned int& uiWidth, unsigned int& uiHeight)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateSceneCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetOriginalPanoSize(). Not Ready. ==>";
			return false;
		}

		cv::Size panoSize;
		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);
			panoSize = cv::Size(m_iPanoOriginalWidth, m_iPanoOriginalHeight);
		}

		int iWidth4n = (0 != (panoSize.width % 4)) ? (panoSize.width / 4) * 4 + 4 : (panoSize.width / 4) * 4;

		uiWidth = static_cast<unsigned int>(iWidth4n);
		uiHeight = static_cast<unsigned int>(panoSize.height);

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetOriginalPanoSize(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::SetViewCamera(G_tViewCamera viewCamera)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SetViewCamera(). Not Ready. ==>";
			return false;
		}

		//G_tViewCamera viewCameraOld;
		//GetViewCamera(viewCameraOld);
		//GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::SetViewCamera(). Pan: " << viewCameraOld.fHorizontal
		//	<< ", Tilt: " << viewCameraOld.fVertical << ", Zoom: " << viewCameraOld.fZoom
		//	<< " ---> Pan: " << viewCamera.fHorizontal
		//	<< ", Tilt: " << viewCamera.fVertical << ", Zoom: " << viewCamera.fZoom;

		GInteractionControllerDOF dof;
		dof.fHorizontal = viewCamera.fHorizontal;
		dof.fVertical = viewCamera.fVertical;
		dof.fZoom = viewCamera.fZoom;

		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			m_RenderParameterManager.SetInteractionControllerDOF(dof);
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SetViewCamera(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::SetDrawingRegion(unsigned int uiX0, unsigned int uiY0, unsigned int uiWidth, unsigned int uiHeight)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SetDrawingRegion(). Not Ready. ==>";
			return false;
		}

		GLOGGER(info) << "==> GStitcherOGLWrapper_PanoRender::SetDrawingRegion(). X0: " << uiX0
			<< "; Y0: " << uiY0 << "; Width: " << uiWidth << "; Height: " << uiHeight;

		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			m_RenderParameterManager.SetDrawRegion(static_cast<int>(uiX0), static_cast<int>(uiY0)
				, static_cast<int>(uiWidth), static_cast<int>(uiHeight));
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SetDrawingRegion(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::InputFrame(int iCameraIdx, unsigned int uiWidth, unsigned int uiHeight, void* pData[3], unsigned int uiStep_Byte[3])
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateInited)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InputFrame(). Not Ready. ==>";
			return false;
		}

		if ((iCameraIdx >= m_vPtrFrameQueues.size()) ||
			(iCameraIdx < 0))
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InputFrame(). Wrong CameraIdx or Camera not supported in this profile: " << iCameraIdx << "  ==>";
			return false;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			m_RenderParameterManager.SetInputFrameSize(static_cast<int>(uiWidth), static_cast<int>(uiHeight));
		}

		GFrameType frameType;
		frameType.eFrameFormat = m_Config.m_eInputFrameFormat;
		frameType.iWidth = static_cast<int>(uiWidth);
		frameType.iHeight = static_cast<int>(uiHeight);
		frameType.step_Byte[0] = static_cast<int>(uiStep_Byte[0]);
		frameType.step_Byte[1] = static_cast<int>(uiStep_Byte[1]);
		frameType.step_Byte[2] = static_cast<int>(uiStep_Byte[2]);

		if (!m_vPtrFrameQueues[iCameraIdx]->CreateFrame(frameType))
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::InputFrame(). CreateFrame failed.";
			return false;
		}

		unsigned char* pBuffer = (unsigned char*)(m_vPtrFrameQueues[iCameraIdx]->InMapFrame());

		if (eFrameFormatRGB == m_Config.m_eInputFrameFormat ||
			eFrameFormatBGR == m_Config.m_eInputFrameFormat)
		{
			unsigned int uiWidth_Byte = uiWidth * 3;
			for (int iLine = 0; iLine < uiHeight; ++iLine)
			{
				unsigned char* pSrc = (unsigned char*)(pData[0]) + iLine * uiStep_Byte[0];
				unsigned char* pDst = pBuffer + iLine * uiWidth_Byte;
				std::copy(pSrc, pSrc + uiWidth_Byte, pDst);
			}
		}
		else if (eFrameFormatYUV420P == m_Config.m_eInputFrameFormat ||
			eFrameFormatYV12 == m_Config.m_eInputFrameFormat)
		{
			//cv::namedWindow("t", CV_WINDOW_KEEPRATIO);
			////cv::Mat mTemp(cv::Size(frameType.iWidth, frameType.iHeight), CV_8UC1, pSrcBuffer);
			//cv::Mat mTemp(cv::Size(uiWidth / 2, uiHeight / 2), CV_8UC1, pData[1]);
			////cv::Mat mTemp(cv::Size(uiWidth / 2, uiHeight / 2), CV_8UC1, pBuffer + (frameType.iWidth*frameType.iHeight * 5) / 4);
			//cv::imshow("t", mTemp);
			//cv::waitKey(0);

			unsigned char* pDataY = (unsigned char*)(pData[0]);
			unsigned char* pDataU = (unsigned char*)(pData[1]);
			unsigned char* pDataV = (unsigned char*)(pData[2]);

			for (int iLineY = 0; iLineY < uiHeight; ++iLineY)
			{
				unsigned char* pSrc = pDataY + iLineY * uiStep_Byte[0];
				unsigned char* pDst = pBuffer + iLineY * uiWidth;
				std::copy(pSrc, pSrc + uiWidth, pDst);
			}

			unsigned int uiHalfWidth = uiWidth / 2;

			unsigned char* pUStart = pBuffer + uiWidth * uiHeight;
			for (int iLineU = 0; iLineU < uiHeight / 2; ++iLineU)
			{
				unsigned char* pSrc = pDataU + iLineU * uiStep_Byte[1];
				std::copy(pSrc, pSrc + uiHalfWidth, pUStart + iLineU * uiHalfWidth);
			}

			unsigned char* pVStart = pBuffer + (uiWidth * uiHeight * 5) / 4;
			for (int iLineV = 0; iLineV < uiHeight / 2; ++iLineV)
			{
				unsigned char* pSrc = pDataV + iLineV * uiStep_Byte[2];
				std::copy(pSrc, pSrc + uiHalfWidth, pVStart + iLineV * uiHalfWidth);
			}

			//std::copy(pDataU, pDataU + (uiWidth * uiHeight) / 4, pBuffer + uiWidth * uiHeight);
			//std::copy(pDataV, pDataV + (uiWidth * uiHeight) / 4, pBuffer + (uiWidth * uiHeight * 5) / 4);


			//cv::Mat mTemp(cv::Size(frameType.iWidth, frameType.iHeight), CV_8UC1, pSrcBuffer);
			//cv::Mat mTemp(cv::Size(uiWidth / 2, uiHeight / 2), CV_8UC1, pBuffer + frameType.iWidth*frameType.iHeight);
			//cv::Mat mTemp(cv::Size(uiWidth / 2, uiHeight / 2), CV_8UC1, pBuffer + (frameType.iWidth*frameType.iHeight * 5) / 4);
			//cv::imshow("t", mTemp);
			//cv::waitKey(0);
		}
		else if (eFrameFormatNV12 == m_Config.m_eInputFrameFormat)
		{
			unsigned char* pDataY = (unsigned char*)(pData[0]);
			unsigned char* pDataUV = (unsigned char*)(pData[1]);

			for (int iLineY = 0; iLineY < uiHeight; ++iLineY)
			{
				unsigned char* pSrc = pDataY + iLineY * uiStep_Byte[0];
				unsigned char* pDst = pBuffer + iLineY * uiWidth;
				std::copy(pSrc, pSrc + uiWidth, pDst);
			}

			unsigned char* pUVStart = pBuffer + uiWidth * uiHeight;
			for (int iLineUV = 0; iLineUV < uiHeight / 2; ++iLineUV)
			{
				unsigned char* pSrc = pDataUV + iLineUV * uiStep_Byte[1];
				std::copy(pSrc, pSrc + uiWidth, pUVStart + iLineUV * uiWidth);
			}
		}

		m_vPtrFrameQueues[iCameraIdx]->CompleteFrame();

		m_bDataArrived = true;

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::InputFrame(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::GetCurrentPanoType(int* pCurrentPanoType) const
{
	if (m_ePanoRenderState < ePanoRenderStateCreated)
	{
		GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetCurrentPanoType(). Not Ready.";
		return false;
	}

	switch (m_RenderParameterManager.GetPanoType())
	{
	case ePanoTypeImmersionSemiSphere:
		(*pCurrentPanoType) = G_ePanoTypeImmersionSemiSphere;
		break;
	case ePanoTypeUnwrappedCylinder180:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinder180;
		break;
	case ePanoTypeUnwrappedCylinder360:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinder360;
		break;
	case ePanoTypeImmersionCylinder360:
		(*pCurrentPanoType) = G_ePanoTypeImmersionCylinder360;
		break;
	case ePanoTypeImmersionCylinder360Inside:
		(*pCurrentPanoType) = G_ePanoTypeImmersionCylinder360Inside;
		break;
	case ePanoTypeUnwrappedCylinderSplited:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinderSplited;
		break;
	case ePanoTypeUnwrappedCylinderSplitedFrontBack:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinderSplitedFrontBack;
		break;
	case ePanoTypeUnwrappedSphere180:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedSphere180;
		break;
	case ePanoTypeUnwrappedSphere360:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedSphere360;
		break;
	case ePanoTypeUnwrappedSphere360Flip:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedSphere360Flip;
		break;
	case ePanoTypeUnwrappedCylinder180Flip:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinder180Flip;
		break;
	case ePanoTypeUnwrappedCylinder360Flip:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedCylinder360Flip;
		break;
	case ePanoTypeImmersionFullSphere:
		(*pCurrentPanoType) = G_ePanoTypeImmersionFullSphere;
		break;
	case ePanoTypeUnwrappedFullSphere360:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedFullSphere360;
		break;
	case ePanoTypeUnwrappedFullSphere360Flip:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedFullSphere360Flip;
		break;
	case ePanoTypeUnwrappedFullSphere360Binocular:
		(*pCurrentPanoType) = G_ePanoTypeUnwrappedFullSphere360Binocular;
		break;
	case ePanoTypeImmersionFullSphereInside:
		(*pCurrentPanoType) = G_ePanoTypeImmersionFullSphereInside;
		break;
	case ePanoTypeOriginal:
		(*pCurrentPanoType) = G_ePanoTypeOriginal;
		break;
	default:
		//todo.
		break;
	}

	return true;
}

bool GStitcherOGLWrapper_PanoRender::SwitchPanoType(G_enPanoType eNewPanoType)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SwitchPanoType(). Not Ready. ==>";
			return false;
		}

		enPanoType eCurrentPanoType = ePanoTypeImmersionSemiSphere;
		switch (eNewPanoType)
		{
		case G_ePanoTypeImmersionSemiSphere:
			eCurrentPanoType = ePanoTypeImmersionSemiSphere;
			break;
		case G_ePanoTypeUnwrappedCylinder180:
			eCurrentPanoType = ePanoTypeUnwrappedCylinder180;
			break;
		case G_ePanoTypeUnwrappedCylinder360:
			eCurrentPanoType = ePanoTypeUnwrappedCylinder360;
			break;
		case G_ePanoTypeImmersionCylinder360:
			eCurrentPanoType = ePanoTypeImmersionCylinder360;
			break;
		case G_ePanoTypeImmersionCylinder360Inside:
			eCurrentPanoType = ePanoTypeImmersionCylinder360Inside;
			break;
		case G_ePanoTypeUnwrappedCylinderSplited:
			eCurrentPanoType = ePanoTypeUnwrappedCylinderSplited;
			break;
		case G_ePanoTypeUnwrappedCylinderSplitedFrontBack:
			eCurrentPanoType = ePanoTypeUnwrappedCylinderSplitedFrontBack;
			break;
		case G_ePanoTypeUnwrappedSphere180:
			eCurrentPanoType = ePanoTypeUnwrappedSphere180;
			break;
		case  G_ePanoTypeUnwrappedSphere360:
			eCurrentPanoType = ePanoTypeUnwrappedSphere360;
			break;
		case  G_ePanoTypeUnwrappedSphere360Flip:
			eCurrentPanoType = ePanoTypeUnwrappedSphere360Flip;
			break;
		case  G_ePanoTypeUnwrappedCylinder180Flip:
			eCurrentPanoType = ePanoTypeUnwrappedCylinder180Flip;
			break;
		case  G_ePanoTypeUnwrappedCylinder360Flip:
			eCurrentPanoType = ePanoTypeUnwrappedCylinder360Flip;
			break;
		case G_ePanoTypeImmersionFullSphere:
			eCurrentPanoType = ePanoTypeImmersionFullSphere;
			break;
		case G_ePanoTypeUnwrappedFullSphere360:
			eCurrentPanoType = ePanoTypeUnwrappedFullSphere360;
			break;
		case G_ePanoTypeUnwrappedFullSphere360Flip:
			eCurrentPanoType = ePanoTypeUnwrappedFullSphere360Flip;
			break;
		case G_ePanoTypeUnwrappedFullSphere360Binocular:
			eCurrentPanoType = ePanoTypeUnwrappedFullSphere360Binocular;
			break;
		case G_ePanoTypeImmersionFullSphereInside:
			eCurrentPanoType = ePanoTypeImmersionFullSphereInside;
			break;
		case G_ePanoTypeOriginal:
			eCurrentPanoType = ePanoTypeOriginal;
			break;
		default:
			//todo.
			break;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			m_RenderParameterManager.SetPanoType(eCurrentPanoType);
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SwitchPanoType(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::GrabPano(unsigned int* puiWidth, unsigned int* puiHeight, unsigned char* pRGBData)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateSceneCreated)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GrabPano(). Not Ready. ==>";
			return false;
		}

		GGrabCallBackContext grabContext;
		grabContext.uiWidth = *puiWidth;
		grabContext.uiHeight = *puiHeight;
		grabContext.pContext = pRGBData;

		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);

			GWriteOutCallBack callBack;
			callBack.pCallBackFunc = GrabCallBack;
			callBack.iCallBackTimes = 1;
			callBack.fScale = 1.0f;
			callBack.pContext = &grabContext;
			m_mpWriteOutCallBack["GrabPano"] = callBack;
		}

		std::unique_lock<std::mutex> lk(grabContext.mtTaskMutex);
		grabContext.cvTaskDone.wait(lk, [&grabContext]()
		{
			return grabContext.bTaskDone;
		});

		if (nullptr == grabContext.pContext)
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GrabPano(). No enough storage space. ==>";
			return false;
		}

		*puiWidth = grabContext.uiWidth;
		*puiHeight = grabContext.uiHeight;

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GrabPano(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::SetOutputCallBack(G_tOutputCallBack outputCallBackFunc, float fScale, void* pContext)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SetOutputCallBack(). Not Ready. ==>";
			return false;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);

			if (nullptr != outputCallBackFunc)
			{
				GWriteOutCallBack callBack;
				callBack.pCallBackFunc = outputCallBackFunc;
				callBack.iCallBackTimes = -1;
				callBack.fScale = fScale;
				callBack.pContext = pContext;
				m_mpWriteOutCallBack["OutputCallBack"] = callBack;
			}
			else
			{
				m_mpWriteOutCallBack.erase("OutputCallBack");
			}

			return true;

		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SetOutputCallBack(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::SetPreRenderCallBack(G_tRenderCallBack renderCallBackFunc, void* pContext)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SetPreRenderCallBack(). Not Ready. ==>";
			return false;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);
			m_stitcherOGL.SetPreRenderCallBack((tRenderCallBack)(renderCallBackFunc), pContext);
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SetPreRenderCallBack(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::SetRenderCallBack(G_tRenderCallBack renderCallBackFunc, void* pContext)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::SetRenderCallBack(). Not Ready. ==>";
			return false;
		}

		{
			std::lock_guard<std::mutex> lockGuard(m_mtStitcherOGLMutex);
			m_stitcherOGL.SetRenderCallBack((tRenderCallBack)(renderCallBackFunc), pContext);
		}

		return true;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::SetRenderCallBack(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::UnProject(G_tNormalizedPixelCoordinate* pPoints, unsigned int uiPointsSize, G_tPhysicalDirection* pPhysicalDirection)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateSceneCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::UnProject(). Not Ready. ==>";
			return false;
		}

		m_UnprojectContext.vfInputPoints.clear();
		m_UnprojectContext.vfOutputPoints.clear();

		for (int i = 0; i < uiPointsSize; ++i)
		{
			cv::Point2f point2f;
			point2f.x = pPoints[i].fX;
			point2f.y = pPoints[i].fY;

			if (point2f.x < 0.0 || point2f.x > 1.0 || point2f.y < 0.0 || point2f.y > 1.0)
			{
				GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::UnProject(). Error: Invalid normalized pixel coordinate value. ==>";
				return false;
			}

			m_UnprojectContext.vfInputPoints.push_back(point2f);
		}

		m_UnprojectContext.vfOutputPoints.resize(uiPointsSize);
		m_UnprojectContext.bNeedUnProject = true;

		std::unique_lock<std::mutex> lk(m_UnprojectContext.mtTaskMutex);
		m_UnprojectContext.cvTaskDone.wait(lk, [this]()
		{
			return (!this->m_UnprojectContext.bNeedUnProject);
		});

		//for pix with a infinit depth, return FLT_MAX.
		for (int i = 0; i < uiPointsSize; ++i)
		{
			if (FLT_MAX == m_UnprojectContext.vfOutputPoints[i].x &&
				FLT_MAX == m_UnprojectContext.vfOutputPoints[i].y &&
				FLT_MAX == m_UnprojectContext.vfOutputPoints[i].z)
			{
				pPhysicalDirection[i].fTheta = FLT_MAX;
				pPhysicalDirection[i].fPhi = FLT_MAX;
				return true;
			}
		}

		//convert 3D coordinate to direction.
		//theta: start from +z.
		//phi: start from +x, x -> y.
		for (int i = 0; i < uiPointsSize; ++i)
		{
			cv::Point3f point3f;
			point3f.x = m_UnprojectContext.vfOutputPoints[i].x;
			point3f.y = m_UnprojectContext.vfOutputPoints[i].y;
			point3f.z = m_UnprojectContext.vfOutputPoints[i].z;

			pPhysicalDirection[i].fTheta = atan2f(sqrtf(point3f.x*point3f.x + point3f.y*point3f.y), point3f.z);
			pPhysicalDirection[i].fPhi = atan2f(point3f.y, point3f.x);
		}

		return m_UnprojectContext.bRet;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::UnProject(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::CalcViewCamera(G_tPhysicalDirection pPhysicalDirection, G_tViewCamera* pViewCamera)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateSceneCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::CalcViewCamera(). Not Ready. ==>";
			return false;
		}

		GInteractionControllerDOF dof;
		dof.fZoom = pViewCamera->fZoom;

		bool bRet = false;
		{
			std::lock_guard<std::mutex> lockGuard(m_RenderParameterManager.m_mtManagerMutex);
			bRet = m_RenderParameterManager.CalcInteractionControllerDOF(pPhysicalDirection.fTheta, pPhysicalDirection.fPhi, dof);
		}

		pViewCamera->fHorizontal = dof.fHorizontal;
		pViewCamera->fVertical = dof.fVertical;
		pViewCamera->fZoom = dof.fZoom;

		return bRet;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::CalcViewCamera(). EXCEPTION. ==>";
		return false;
	}
}

bool GStitcherOGLWrapper_PanoRender::GetCameraParameters(int iCameraIdx, char* sCameraMode, float* fK, float* fDistortion, float* fR, G_enCameraModelType* peCameraModelType)
{
	try
	{
		if (m_ePanoRenderState < ePanoRenderStateCreated)
		{
			GLOGGER(info) << "GStitcherOGLWrapper_PanoRender::GetCameraParameters(). Not Ready. ==>";
			return false;
		}

		bool bRet = false;
		if ((iCameraIdx >= 0) && (iCameraIdx < m_vCameras.size()))
		{
			cv::Mat mK(cv::Size(3, 3), CV_32FC1, fK);
			cv::Mat mR(cv::Size(3, 3), CV_32FC1, fR);

			if (eCameraTypeNormal == m_vCameras[iCameraIdx].eCameraType)
			{
				cv::Mat mDist(cv::Size(5, 1), CV_32FC1, fDistortion);
				m_vCameras[iCameraIdx].distorsionCoefficients.convertTo(mDist, CV_32FC1);
				*peCameraModelType = G_eCameraModelTypeNormal2r2t1r;
			}
			else if (eCameraTypeFisheye == m_vCameras[iCameraIdx].eCameraType)
			{
				cv::Mat mDist(cv::Size(4, 1), CV_32FC1, fDistortion);
				m_vCameras[iCameraIdx].distorsionCoefficients.convertTo(mDist, CV_32FC1);
				*peCameraModelType = G_eCameraModelTypeFisheye4theta;
			}
			else if (eCameraTypeFisheyeAnglePoly5 == m_vCameras[iCameraIdx].eCameraType)
			{
				cv::Mat mDist(cv::Size(5, 1), CV_32FC1, fDistortion);
				m_vCameras[iCameraIdx].distorsionCoefficients.convertTo(mDist, CV_32FC1);
				*peCameraModelType = G_eCameraModelTypeFisheyeAnglePoly5;
			}

			m_vCameras[iCameraIdx].m_mpMultiK0Map[std::string(sCameraMode)].convertTo(mK, CV_32FC1);

			cv::Mat mTemp;
			m_vCameras[iCameraIdx].R.convertTo(mTemp, CV_32FC1);

			cv::SVD svd;
			svd(mTemp, cv::SVD::FULL_UV);
			mTemp = svd.u * svd.vt;
			if (cv::determinant(mTemp) < 0)
			{
				mTemp *= -1;
			}
			mTemp.convertTo(mR, CV_32FC1);

			bRet = true;
		}
		else
		{
			GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetCameraParameters(). ERROR: Invalid iCameraIdx: " << iCameraIdx << ". ==>";
			return false;
		}
		return bRet;
	}
	catch (...)
	{
		m_ePanoRenderState = ePanoRenderStateError;
		DestroyPanoRender();
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::GetCameraParameters(). EXCEPTION. ==>";
		return false;
	}
}

void GStitcherOGLWrapper_PanoRender::DestroyPanoRender()
{
	try
	{
		{
			std::lock_guard<std::mutex> lk(m_mtHWNDMutex);
			m_bRendering = false;
		}
		m_cvInitFlag.notify_one();
		m_tdRendering.join();
		return;
	}
	catch (...)
	{
		GLOGGER(error) << "GStitcherOGLWrapper_PanoRender::DestroyPanoRender(). Unexpected EXCEPTION encountered when destroying PanoRender. Process would be Terminated. ==>";
		std::exit(EXIT_FAILURE);
		return;
	}
}


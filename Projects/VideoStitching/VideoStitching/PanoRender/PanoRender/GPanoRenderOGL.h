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
 
 
 
#ifndef G_PANO_RENDER_OGL_H_
#define G_PANO_RENDER_OGL_H_

#include "PanoRender/GPanoRenderOGLIF.h"

#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "opencv2/core/core.hpp"

#include "PanoRender/GFrameQueueIF.h"
#include "GRender.h"
#include "GRenderParaManager.h"
#include "GDataTransfer.h"

#if defined WIN32
#include "Windows.h"
#elif __APPLE__
#elif __ANDROID__
#elif __linux__
#endif // WIN32

class GStitcherOGLWrapper_PanoRender
{
public:
	GStitcherOGLWrapper_PanoRender() :
		m_Config(GStitcherConfig())
		, m_ePanoRenderState(ePanoRenderStateUnCreated)
		, m_bDataArrived(false)
		, m_bRendering(true)
		, m_iFPS(0)
		, m_iPanoOriginalWidth(0)
		, m_iPanoOriginalHeight(0)
		, m_HWND(nullptr)
		, m_bInitCalled(false)
#ifdef WIN32
		, m_HDC(nullptr)
		, m_GLContext(nullptr)
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
		, m_Display(EGL_NO_DISPLAY)
		, m_Surface(EGL_NO_SURFACE)
		, m_GLContext(EGL_NO_CONTEXT)
#endif // WIN32
	{}

	bool Create(G_tPanoRenderConfig panoRenderConfig, const char* pModel, unsigned int uiSize_Byte);

	bool InitPanoRender(void* hWnd);

	bool GetSupportedPanoTypeList(std::vector<G_enPanoType>& vSupportedPanoTypeList);

	bool GetViewCamera(G_tViewCamera& viewCamera);

	bool GetOriginalPanoSize(unsigned int& uiWidth, unsigned int& uiHeight);

	bool SetViewCamera(G_tViewCamera viewCamera);

	bool SetDrawingRegion(unsigned int uiX0, unsigned int uiY0, unsigned int uiWidth, unsigned int uiHeight);

	bool InputFrame(int iCameraIdx
		, unsigned int uiWidth, unsigned int uiHeight, void* pData[3], unsigned int uiStep_Byte[3]);

	bool GetCurrentPanoType(int* pCurrentPanoType) const;

	bool SwitchPanoType(G_enPanoType eNewPanoType);

	bool GrabPano(unsigned int* puiWidth, unsigned int* puiHeight, unsigned char* pRGBData);

	bool SetOutputCallBack(G_tOutputCallBack outputCallBackFunc, float fScale, void* pContext);

	bool SetPreRenderCallBack(G_tRenderCallBack renderCallBackFunc, void* pContext);

	bool SetRenderCallBack(G_tRenderCallBack renderCallBackFunc, void* pContext);

	bool UnProject(G_tNormalizedPixelCoordinate* pPoints, unsigned int uiPointsSize, G_tPhysicalDirection* pPhysicalDirection);

	bool CalcViewCamera(G_tPhysicalDirection pPhysicalDirection, G_tViewCamera* pViewCamera);

	bool GetCameraParameters(int iCameraIdx, char* sCameraMode, float* fK, float* fDistortion, float* fR, G_enCameraModelType* peCameraModelType);

	void DestroyPanoRender();

private:
	//methods just for Rendering thread.

	static void RenderingLoop(GStitcherOGLWrapper_PanoRender* pPanoRender);

	unsigned long GetTasks();

#ifdef WIN32
	bool InitOpenGL();
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
	bool GetDisplay(void* pNativeWindow = nullptr);
	bool GetConfig(EGLConfig& eglConfig);
	bool GetSurface(EGLConfig eglConfig, void* pNativeWindow = nullptr);
	bool InitOpenGL();
#endif // WIN32

	bool InitStitcher();

	bool UploadTextures();

	bool Render();

	bool RenderOnly(const glm::mat4& matP, const glm::mat4& matV);

	bool RenderWithOutput(const glm::mat4& matP, const glm::mat4& matV);

	bool RenderWithOutput4EarlyVersion(const glm::mat4& matP, const glm::mat4& matV);

	void OutputCallBack(unsigned char* pData, int iWidth, int iHeight);

	void OutputCallBackwithScale(GFrameDownloader* pDownloader);

	void Release();

private:
	enum  enPanoRenderState
	{
		ePanoRenderStateError = 0,
		ePanoRenderStateUnCreated,
		ePanoRenderStateCreated,
		ePanoRenderStateInited,
		ePanoRenderStateSceneCreated,
	};

	enum enPanoRenderTask
	{
		ePanoRenderTaskInit = 0,
		ePanoRenderTaskCreateScene,
		ePanoRenderTaskSetViewPort,
		ePanoRenderTaskSetOriginalPanoSize,
		ePanoRenderTaskUnProject,
		ePanoRenderTaskRender,
		ePanoRenderTaskDestroy,
	};

	struct tUnprojectContext
	{
		tUnprojectContext() :
			bNeedUnProject(false)
		{}

		bool bNeedUnProject;
		bool bRet;
		std::vector<cv::Point2f> vfInputPoints;
		std::vector<cv::Point3f> vfOutputPoints;

		std::mutex mtTaskMutex;
		std::condition_variable cvTaskDone;
	};

	struct GWriteOutCallBack
	{
		//iCallBackTimes:
		//iCallBackTimes >= 0: call back n times.
		//iCallBackTimes < 0: call back until callback function reseted.

		G_tOutputCallBack pCallBackFunc;
		int iCallBackTimes;
		float fScale;
		void* pContext;
	};

private:
	GRenderParameterManager m_RenderParameterManager;
	GStitcherConfig m_Config;
	
	GTextureGroup m_textureGroup;

	GTextureGroupUploader m_uploader;

	std::vector<std::shared_ptr<GFrameQueueIF>> m_vPtrFrameQueues;
	GStitcherOGL m_stitcherOGL;
	int m_iPanoOriginalWidth;
	int m_iPanoOriginalHeight;
	GOpenGLFrameBuffer m_FBODisplay;
	GTaskQueue<GFrameDownloader> m_downloaders;
	cv::Mat m_mPanoBuffer;
	std::map <std::string, GWriteOutCallBack> m_mpWriteOutCallBack;
	std::mutex m_mtStitcherOGLMutex;

	std::vector<CameraModel> m_vCameras;

	bool m_bDataArrived;
	enPanoRenderState m_ePanoRenderState;

	std::thread m_tdRendering;

	void* m_HWND;
	bool m_bInitCalled;
#ifdef WIN32
	HDC m_HDC;
	HGLRC m_GLContext;
#elif __APPLE__
#elif (defined __ANDROID__) || (defined __linux__)
	EGLDisplay m_Display;
	EGLSurface m_Surface;
	EGLContext m_GLContext;
#endif // WIN32

	std::mutex m_mtHWNDMutex;
	std::condition_variable m_cvInitFlag;

	bool m_bRendering;

	int m_iFPS;

	tUnprojectContext m_UnprojectContext;
};


#endif //G_PANO_RENDER_OGL_H_
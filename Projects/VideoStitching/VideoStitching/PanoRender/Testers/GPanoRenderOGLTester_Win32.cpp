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
 
 
 
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#include "GGLHeaders.h"

#include <vector>
#include <fstream>
#include <chrono>
#include <thread>
#include <map>

#include "boost/filesystem.hpp"
#include "boost/range/iterator_range.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Common/Exception/GiraffeChecker.h"
#include "GTesterConfig.h"
#include "PanoRender/GPanoRenderOGLIF.h"
#include "VideoSource/GCVideoSourceIF.h"


using namespace boost;
using namespace boost::filesystem;

#pragma comment (lib, "opengl32.lib")

GTesterConfig global_config;

int iPanoRenderIdx[2];
bool bFeedInFlag = true;
std::thread tdFeedFrame;

bool global_bMouseLeftDown = false;
int global_iX0 = 0;
int global_iY0 = 0;
int global_iWidth = 0;
int global_iHeight = 0;

cv::VideoWriter global_Writer;

// enable optimus.
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

void G_CALL_CONVENTION WriteOutCallBack(unsigned int uiWidth, unsigned int uiHeight, const unsigned char* pRGBData, void* pContext)
{
	if (0 == uiWidth || 0 == uiHeight || nullptr == pRGBData)
	{
		return;
	}

	cv::VideoWriter* pWriter = (cv::VideoWriter*)pContext;
	if (!pWriter->isOpened())
	{
		pWriter->open("pano.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25
			, cv::Size(static_cast<int>(uiWidth), static_cast<int>(uiHeight)), true);
	}

	cv::Mat mTemp(cv::Size(static_cast<int>(uiWidth), static_cast<int>(uiHeight)), CV_8UC3, (void*)(pRGBData));
	((cv::VideoWriter*)pContext)->write(mTemp);
}

void G_CALL_CONVENTION RenderCallBack(void* pContext)
{
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//glOrtho(-1.0, 1.0, 1.0, -1.0, 0.0, 10000.0);

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//gluLookAt(0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	//glBegin(GL_TRIANGLES);
	//glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	//glVertex3f(0.5f, 0.0f, 992.0f);
	//glVertex3f(0.5f, 0.75f, 992.0f);
	//glVertex3f(1.25f, 0.0f, 992.0f);

	//glEnd();
}

void GetFileList(const std::string& sPath, const std::vector<std::string>& vFileTypes, std::vector<path>& vFileList)
{
	//find the source files under specified path.
	path pWorkPath(sPath);
	if (!is_directory(pWorkPath))
	{
		return;
	}

	for (auto& subPath : boost::make_iterator_range(directory_iterator(pWorkPath), directory_iterator()))
	{
		if (vFileTypes.end() != std::find(vFileTypes.begin(), vFileTypes.end(), subPath.path().extension().string()))
		{
			vFileList.push_back(system_complete(subPath));
		}
	}

	//read images and construct dataGraph.
	std::sort(vFileList.begin(), vFileList.end(), [](const path& p1, const path& p2)
	{
		return p1.stem().string() < p2.stem().string();
	});
}

void FeedFrame()
{
	std::map<std::string, int> mpVideoGroupSource;

	for (int i = 0; i < global_config.vURLs.size(); ++i)
	{
		G_tVideoSourceCfg cfg;
		cfg.eVideoSourceType = G_eVideoSourceTypeURL;
		cfg.unConfig.pcVideoSourceCfgURL = global_config.vURLs[i].c_str();

		int iNewVideoSourceHandle = -1;
		G_VideoSourceCreate(&iNewVideoSourceHandle, &cfg);

		mpVideoGroupSource[std::to_string(i)] = iNewVideoSourceHandle;
	}

	g_action_on_event(0 == mpVideoGroupSource.size(), return;);


	while (bFeedInFlag)
	{
		auto start = std::chrono::high_resolution_clock::now();

		for (auto it = mpVideoGroupSource.begin(); it != mpVideoGroupSource.end(); ++it)
		{
			G_tVideoFrame frame;
			if (G_eVideoSourceErrorNone != G_VideoSourceGrab(it->second, &frame))
			{
				continue;
			}

			void* pData[3] = { (void*)(frame.pData[0]), (void*)(frame.pData[1]), (void*)(frame.pData[2]) };
			unsigned int Step[3] = { frame.uiLineStep_Byte[0], frame.uiLineStep_Byte[1], frame.uiLineStep_Byte[2]};

			std::string sText = it->first;
			cv::Mat mTU(frame.uiHeight / 2, frame.uiWidth / 2, CV_8UC1, frame.pData[1]);
			cv::Mat mTV(frame.uiHeight / 2, frame.uiWidth / 2, CV_8UC1, frame.pData[2]);
			cv::putText(mTU, sText, cv::Point(mTU.cols * 0.5, mTU.rows * 0.5), 0, 4, cv::Scalar(0), 5);
			cv::putText(mTV, sText, cv::Point(mTV.cols * 0.5, mTV.rows * 0.5), 0, 4, cv::Scalar(0), 5);

			for (int iPanoRender = 0; iPanoRender < (("double" == global_config.sWorkMode) ? 2 : 1); ++iPanoRender)
			{
				G_InputFrame(iPanoRenderIdx[iPanoRender], atoi(it->first.c_str()), frame.uiWidth, frame.uiHeight
					, pData, Step);
			}

			G_VideoSourceDrop(it->second);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto sleepFor = (std::chrono::milliseconds(30) - std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count();
		sleepFor = (sleepFor < 0) ? 0 : sleepFor;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
	}

	if (0 != mpVideoGroupSource.size())
	{
		for (auto it = mpVideoGroupSource.begin(); it != mpVideoGroupSource.end(); ++it)
		{
			G_VideoSourceRelease(it->second);
		}
	}
}

int CreatePanoRender(HWND hWnd)
{
	std::filebuf* pBuf;
	std::ifstream ifs(global_config.sProfilePath, std::ios::in | std::ios::binary);
	if (!ifs.good())
	{
		return 0;
	}

	pBuf = ifs.rdbuf();
	int iFileSize_Byte = pBuf->pubseekoff(0, std::ios::end, std::ios::in);
	pBuf->pubseekpos(0, std::ios::in);
	std::vector<char> vModelFile(iFileSize_Byte);
	pBuf->sgetn(vModelFile.data(), iFileSize_Byte);
	ifs.close();

	G_tPanoRenderConfig config;
	config.eInputFormat = G_eInputFormatYUV420P;

	config.eRenderingMode = G_eRenderingModeOnScreen;
	if ("online" == global_config.sRenderingMode)
	{
		config.eRenderingMode = G_eRenderingModeOnScreen;
	}
	else if ("offline" == global_config.sRenderingMode)
	{
		config.eRenderingMode = G_eRenderingModeOffScreen;
	}
	else if ("offlinert" == global_config.sRenderingMode)
	{
		config.eRenderingMode = G_eRenderingModeOffScreenRealTime;
	}

	int iPanoRenderIndex = 0;
	G_CreatePanoRender(config, vModelFile.data(), iFileSize_Byte, &iPanoRenderIndex);
	G_InitPanoRender(iPanoRenderIndex, hWnd);
	return iPanoRenderIndex;
}

void SetPixelFormat(HWND hWnd)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
		32,                        //Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                        //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};

	HDC hdc = GetDC(hWnd);
	if (nullptr == hdc)
	{
		return;
	}

	int iPixFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, iPixFormat, &pfd);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	MSG msg = { 0 };
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = "PanoRenderTester_Win32";
	wc.style = CS_OWNDC;
	if (!RegisterClass(&wc))
		return 1;
	CreateWindow(wc.lpszClassName, "PanoRenderTester_Win32", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);

	while (GetMessage(&msg, NULL, 0, 0) > 0)
		DispatchMessage(&msg);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwnds[2] = { nullptr, nullptr };

	switch (message)
	{
	case WM_CREATE:
	{
		hwnds[0] = CreateWindowW(L"STATIC", L"pano1", WS_CHILD | WS_VISIBLE, 0, 0, 800, 800, hWnd, NULL, NULL, NULL);
		if ("double" == global_config.sWorkMode)
		{
			hwnds[1] = CreateWindowW(L"STATIC", L"pano2", WS_CHILD | WS_VISIBLE, 0, 0, 800, 800, hWnd, NULL, NULL, NULL);
		}

		//global_config.SaveConfigFile("PanoRenderOGLTesterConfig.yml");
		global_config.ReadConfigFile("PanoRenderOGLTesterConfig.yml");

		SetPixelFormat(hwnds[0]);
		if ("double" == global_config.sWorkMode)
		{
			SetPixelFormat(hwnds[1]);
		}

		iPanoRenderIdx[0] = CreatePanoRender(hwnds[0]);
		//G_SwitchPanoType(iPanoRenderIdx[0], G_ePanoTypeUnwrappedFullSphere360Binocular);
		if ("double" == global_config.sWorkMode)
		{
			iPanoRenderIdx[1] = CreatePanoRender(hwnds[1]);
			//G_SwitchPanoType(iPanoRenderIdx[1], G_ePanoTypeUnwrappedSphere360);
		}

		//G_SetRenderCallBack(iPanoRenderIdx[0], RenderCallBack, nullptr);

		//G_SwitchPanoType(iPanoRenderIdx[0], G_ePanoTypeUnwrappedFullSphere360);

		if ("online" != global_config.sRenderingMode)
		{
			G_SetOutputCallBack(iPanoRenderIdx[0], WriteOutCallBack, 0.5, &global_Writer);
		}


		tdFeedFrame = std::thread(FeedFrame);
	}
	break;
	case WM_SIZE:
	{
		UINT uiWidth = LOWORD(lParam);
		UINT uiHeight = HIWORD(lParam);

		if ("double" == global_config.sWorkMode)
		{
			SetWindowPos(hwnds[0], 0, 0, 0, uiWidth, uiHeight / 2 - 2, 0);
			SetWindowPos(hwnds[1], 0, 0, uiHeight / 2 + 2, uiWidth, uiHeight / 2 - 2, 0);

			for (int iPanoRender = 0; iPanoRender < 2; ++iPanoRender)
			{
				RECT drawRegion;
				GetClientRect(hwnds[0], &drawRegion);
				G_SetDrawingRegion(iPanoRenderIdx[iPanoRender], drawRegion.left, drawRegion.top
					, drawRegion.right - drawRegion.left, drawRegion.bottom - drawRegion.top);
				global_iWidth = drawRegion.right - drawRegion.left;
				global_iHeight = drawRegion.bottom - drawRegion.top;
			}
		}
		else
		{
			SetWindowPos(hwnds[0], 0, 0, 0, uiWidth, uiHeight, 0);

			RECT drawRegion;
			GetClientRect(hwnds[0], &drawRegion);
			G_SetDrawingRegion(iPanoRenderIdx[0], drawRegion.left, drawRegion.top
				, drawRegion.right - drawRegion.left, drawRegion.bottom - drawRegion.top);
			global_iWidth = drawRegion.right - drawRegion.left;
			global_iHeight = drawRegion.bottom - drawRegion.top;
		}
	}
	break;
	case WM_LBUTTONDOWN:
	{
		global_bMouseLeftDown = true;
		global_iX0 = static_cast<int>(GET_X_LPARAM(lParam));
		global_iY0 = static_cast<int>(GET_Y_LPARAM(lParam));
	}
	break;
	case WM_LBUTTONUP:
	{
		global_bMouseLeftDown = false;
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if ("double" == global_config.sWorkMode)
		{
			std::vector<G_tNormalizedPixelCoordinate> vPoints;
			G_tNormalizedPixelCoordinate tPoint;
			int iX = GET_X_LPARAM(lParam);
			int iY = GET_Y_LPARAM(lParam);
			tPoint.fX = static_cast<float>(static_cast<float>(iX) / static_cast<float>(global_iWidth));
			tPoint.fY = static_cast<float>(static_cast<float>(iY) / static_cast<float>(global_iHeight));
			vPoints.push_back(tPoint);
			
			std::vector<G_tPhysicalDirection> vDir(vPoints.size());

			G_UnProject(iPanoRenderIdx[0], &vPoints[0],static_cast<unsigned int>(vPoints.size()), &vDir[0]);

			G_tViewCamera cam;
			cam.fZoom = 4.0f;
			G_CalcViewCamera(iPanoRenderIdx[1], vDir[0], &cam);

			G_SetViewCamera(iPanoRenderIdx[1], cam);
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		if (global_bMouseLeftDown)
		{
			int iX = static_cast<int>(GET_X_LPARAM(lParam));
			int iY = static_cast<int>(GET_Y_LPARAM(lParam));
			G_tViewCamera viewCamera;
			if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(iPanoRenderIdx[0], &viewCamera) ||
				(global_iWidth == 0) || (global_iHeight == 0))
			{
				break;
			}
			viewCamera.fHorizontal += 0.5 * static_cast<float>(iX - global_iX0) / (static_cast<float>((global_iWidth + global_iHeight) / 2));
			viewCamera.fVertical += 0.5 * static_cast<float>(iY - global_iY0) / (static_cast<float>((global_iWidth + global_iHeight) / 2));

			global_iX0 = iX;
			global_iY0 = iY;

			G_SetViewCamera(iPanoRenderIdx[0], viewCamera);
		}

		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof(csTME);
		csTME.dwFlags = TME_LEAVE | TME_HOVER;
		csTME.hwndTrack = hWnd;
		csTME.dwHoverTime = 10;
		::_TrackMouseEvent(&csTME);
	}
	break;
	case WM_MOUSELEAVE:
	{
		global_bMouseLeftDown = false;
	}
	break;
	case WM_MOUSEWHEEL:
	{
		G_tViewCamera viewCamera;
		if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(iPanoRenderIdx[0], &viewCamera))
		{
			break;
		}

		if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			viewCamera.fZoom -= 0.1;
		}
		else
		{
			viewCamera.fZoom += 0.1;
		}
		G_SetViewCamera(iPanoRenderIdx[0], viewCamera);
	}
	break;
	case WM_KEYDOWN:
	{
		if (48 <= wParam && 57 >= wParam)
		{
			G_enPanoType supportedPanoTypeList[G_enPanoType::G_ePanoTypeMaxNum];
			int iSize = G_enPanoType::G_ePanoTypeMaxNum;
			G_GetSupportedPanoTypeList(iPanoRenderIdx[0], (int*)(&(supportedPanoTypeList[0])), &iSize);

			if (wParam - 48 < iSize)
			{
				G_SwitchPanoType(iPanoRenderIdx[0], supportedPanoTypeList[wParam - 48]);
			}
		}

		if (VK_SPACE == wParam)
		{
			unsigned int uiPanoWidth, uiPanoHeight;
			if (G_ePanoRenderErrorCodeNone != G_GetOriginalPanoSize(iPanoRenderIdx[0], &uiPanoWidth, &uiPanoHeight))
			{
				break;;
			}
			cv::Mat mPano(uiPanoHeight, uiPanoWidth, CV_8UC3);
			if (G_ePanoRenderErrorCodeNone == G_GrabPano(iPanoRenderIdx[0], &uiPanoWidth, &uiPanoHeight, mPano.data))
			{
				cv::imwrite("panograb.jpg", cv::Mat(uiPanoHeight, uiPanoWidth, CV_8UC3, mPano.data));
			}
			else
			{
				cv::imwrite("panograb.jpg", cv::Mat());
			}
		}

		if (VK_NEXT == wParam)
		{
			G_DestroyPanoRender(iPanoRenderIdx[0]);
			iPanoRenderIdx[0] = CreatePanoRender(hWnd);

			RECT drawRegion;
			GetClientRect(hWnd, &drawRegion);
			G_SetDrawingRegion(iPanoRenderIdx[0], drawRegion.left, drawRegion.top
				, drawRegion.right - drawRegion.left, drawRegion.bottom - drawRegion.top);
			global_iWidth = drawRegion.right - drawRegion.left;
			global_iHeight = drawRegion.bottom - drawRegion.top;

			G_SetRenderCallBack(iPanoRenderIdx[0], RenderCallBack, nullptr);
			G_SetOutputCallBack(iPanoRenderIdx[0], WriteOutCallBack, 1.0, &global_Writer);
		}

	}
	break;
	case WM_CLOSE:
	{
		bFeedInFlag = false;
		tdFeedFrame.join();
		G_DestroyPanoRender(iPanoRenderIdx[0]);
		if ("double" == global_config.sWorkMode)
		{
			G_DestroyPanoRender(iPanoRenderIdx[1]);
		}
		global_Writer.release();
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
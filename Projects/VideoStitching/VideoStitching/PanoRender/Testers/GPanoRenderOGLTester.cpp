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
 
 
 
#ifdef WIN32
#include <windows.h>
#endif // WIN32

#include "PanoRender/GPanoRenderOGLIF.h"

#include <thread>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

#include "boost/filesystem.hpp"
#include "boost/range/iterator_range.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "GGLHeaders.h"
#include <GL/freeglut.h>

#include "VideoSource/GCVideoSourceIF.h"
#include "Common/Exception/GiraffeChecker.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"


using namespace boost;
using namespace boost::filesystem;

int iPanoRenderIdx;

int global_iX0;
int global_iY0;
bool global_bLeftDown;

int global_iWidth;
int global_iHeight;

cv::VideoWriter global_writer;

std::vector<path> global_vFileList;

bool global_bRendering = true;
std::thread global_tdFeedFrame;

#ifdef WIN32
// enable optimus.
extern "C"
{
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif // WIN32

void G_CALL_CONVENTION WriteOutCallBack(unsigned int uiWidth, unsigned int uiHeight, const unsigned char* pRGBData, void* pContext)
{
	cv::VideoWriter* pWriter = (cv::VideoWriter*)pContext;
	if (!pWriter->isOpened())
	{
		pWriter->open("pano.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 25
			, cv::Size(static_cast<int>(uiWidth), static_cast<int>(uiHeight)), true);
	}

	cv::Mat mTemp(cv::Size(static_cast<int>(uiWidth), static_cast<int>(uiHeight)), CV_8UC3, (void*)(pRGBData));
	((cv::VideoWriter*)pContext)->write(mTemp);
}

void FeedFrame()
{
	std::vector<cv::VideoCapture> vCaps;
	vCaps.resize(global_vFileList.size());
	for (int i = 0; i < global_vFileList.size(); ++i)
	{
		if (!vCaps[i].open(global_vFileList[i].string()))
		{
			std::cout << "CAN NOT open video file: " << global_vFileList[i] << std::endl;
			return;//to check.
		}
	}

	std::vector<cv::Mat> vImages;
	vImages.resize(vCaps.size());

	while (global_bRendering)
	{
		auto start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < vCaps.size(); ++i)
		{
			while (true)
			{
				vCaps[i] >> vImages[i];
				if (!vImages[i].data)
				{
					for (int iCap = 0; iCap < vCaps.size(); ++iCap)
					{
						vCaps[iCap].release();
					}

					for (int iFile = 0; iFile < global_vFileList.size(); ++iFile)
					{
						if (!vCaps[iFile].open(global_vFileList[iFile].string()))
						{
							std::cout << "CAN NOT open video file: " << global_vFileList[iFile] << std::endl;
							continue;;
						}
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					continue;
				}
				else
				{
					void* pData[3] = { (void*)(vImages[i].data), 0, 0 };
					unsigned int Step[3] = { vImages[i].step[0], 0, 0 };

					G_InputFrame(iPanoRenderIdx, i, vImages[i].cols, vImages[i].rows
						, pData, Step);

					break;
				}
			}
			//cv::cvtColor(vImages[i], vImages[i], CV_BGR2RGB);
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto sleepFor = (std::chrono::milliseconds(25) - std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count();
		sleepFor = (sleepFor < 0) ? 0 : sleepFor;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
	}
}


void FeedFrameYUV420P()
{
	std::map<std::string, int> mpVideoGroupSource;

	for (int i = 0; i < global_vFileList.size(); ++i)
	{
		G_tVideoSourceCfg cfg;
		cfg.eVideoSourceType = G_eVideoSourceTypeURL;
		cfg.unConfig.pcVideoSourceCfgURL = global_vFileList[i].string().c_str();

		int iNewVideoSourceHandle = -1;
		G_VideoSourceCreate(&iNewVideoSourceHandle, &cfg);

		mpVideoGroupSource[std::to_string(i)] = iNewVideoSourceHandle;
	}

	g_action_on_event(0 == mpVideoGroupSource.size(), return;);


	while (global_bRendering)
	{
		auto start = std::chrono::high_resolution_clock::now();

		for (auto it = mpVideoGroupSource.begin(); it != mpVideoGroupSource.end(); ++it)
		{
			G_tVideoFrame frame;
			G_VideoSourceGrab(it->second, &frame);
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

			G_InputFrame(iPanoRenderIdx, atoi(it->first.c_str()), frame.uiWidth, frame.uiHeight
				, pData, Step);

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

void Display()
{
}

void Reshape(int w, int h)
{
	global_iWidth = w;
	global_iHeight = h;
	G_SetDrawingRegion(iPanoRenderIdx, 0, 0, w, h);
}

void OnMouse(int button, int state, int x, int y)
{
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		global_bLeftDown = true;
		global_iX0 = x;
		global_iY0 = y;
	}

	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
	{
		global_bLeftDown = false;
	}
}

void OnMouseDrag(int x, int y)
{
	if (global_bLeftDown)
	{
		G_tViewCamera viewCamera;
		if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(iPanoRenderIdx, &viewCamera) ||
			global_iWidth == 0 ||
			global_iHeight == 0)
		{
			return;
		}
		viewCamera.fHorizontal -= 0.7 * static_cast<float>(x - global_iX0) / (static_cast<float>((global_iWidth + global_iHeight) / 2));
		viewCamera.fVertical -= 0.7 * static_cast<float>(y - global_iY0) / (static_cast<float>((global_iWidth + global_iHeight) / 2));

		G_SetViewCamera(iPanoRenderIdx, viewCamera);

		global_iX0 = x;
		global_iY0 = y;
	}
}

void OnMouseScrolling(int button, int dir, int x, int y)
{
	G_tViewCamera viewCamera;
	if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(iPanoRenderIdx, &viewCamera))
	{
		return;
	}

	if (dir > 0)
	{
		viewCamera.fZoom -= 0.1;
	}
	else
	{
		viewCamera.fZoom += 0.1;
	}
	G_SetViewCamera(iPanoRenderIdx, viewCamera);
}

void OnKeyPressed(unsigned char ucKey, int iX, int iY)
{
	if (48 <= ucKey && ucKey <= 57)
	{
		G_enPanoType supportedPanoTypeList[G_enPanoType::G_ePanoTypeMaxNum];
		int iSize = G_enPanoType::G_ePanoTypeMaxNum;
		G_GetSupportedPanoTypeList(iPanoRenderIdx, (int*)(&(supportedPanoTypeList[0])), &iSize);

		if (ucKey - 48 < iSize)
		{
			G_SwitchPanoType(iPanoRenderIdx, supportedPanoTypeList[ucKey - 48]);
		}
	}

	if (' ' == ucKey)
	{
		unsigned int uiPanoWidth, uiPanoHeight;
		if (G_ePanoRenderErrorCodeNone != G_GetOriginalPanoSize(iPanoRenderIdx, &uiPanoWidth, &uiPanoHeight))
		{
			return;
		}
		cv::Mat mPano(uiPanoHeight, uiPanoWidth, CV_8UC3);
		G_GrabPano(iPanoRenderIdx, &uiPanoWidth, &uiPanoHeight, mPano.data);
		cv::imwrite("panograb.jpg", mPano);
	}
}

void OnClose()
{
	global_bRendering = false;
	global_tdFeedFrame.join();

	G_DestroyPanoRender(iPanoRenderIdx);
	global_writer.release();
}

void GetFileList(const std::string& sPath, const std::vector<std::string>& vFileTypes, std::vector<path>& vFileList)
{
	//find the source files under specified path.
	path pWorkPath(sPath);
	if (!is_directory(pWorkPath))
	{
		std::cout << "GetFileList: path is not correctly specified." << std::endl;
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

int main(int argc, char** argv)
{
	//try
	//{
		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
		glutInitWindowSize(600, 600);
		glutInitWindowPosition(100, 100);

		//glewExperimental = GL_TRUE;
		glutInitContextVersion(2, 1);
		glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
		//glutInitContextFlags(GLUT_DEBUG);

		const std::string sWindowTitle = "PanoRenderTester";
		glutCreateWindow(sWindowTitle.c_str());

		//find the video files under specified path.
		GetFileList((argc >= 2) ? std::string(argv[1]) : "."
			, std::vector<std::string>({ ".avi", ".mp4", ".mkv", ".ts" }), global_vFileList);

		std::filebuf* pBuf;
		std::ifstream ifs("panoCamProfile.bin", std::ios::in | std::ios::binary);
		if (!ifs.good())
		{
			std::cout << "file not exist." << std::endl;
			return 0;
		}

		pBuf = ifs.rdbuf();
		int iFileSize_Byte = pBuf->pubseekoff(0, std::ios::end, std::ios::in);
		pBuf->pubseekpos(0, std::ios::in);

		std::vector<char> vModelFile(iFileSize_Byte);

		pBuf->sgetn(vModelFile.data(), iFileSize_Byte);

		ifs.close();

#ifdef WIN32
		HWND hwnd = FindWindow("freeglut", sWindowTitle.c_str());
#endif // WIN32

		G_tPanoRenderConfig config;
		config.eInputFormat = G_eInputFormatYUV420P;
		G_CreatePanoRender(config, vModelFile.data(), iFileSize_Byte, &iPanoRenderIdx);
		G_InitPanoRender(iPanoRenderIdx, hwnd);
		G_SetOutputCallBack(iPanoRenderIdx, WriteOutCallBack, 1.0, &global_writer);

		glutReshapeFunc(Reshape);
		glutMouseFunc(OnMouse);
		glutMotionFunc(OnMouseDrag);
		glutMouseWheelFunc(OnMouseScrolling);
		glutKeyboardFunc(OnKeyPressed);
		glutCloseFunc(OnClose);
		glutDisplayFunc(Display);

		//std::thread tdFeedFrame(FeedFrame);
		global_tdFeedFrame = std::thread(FeedFrameYUV420P);
		glutMainLoop();


		return 0;
	//}
	//catch (...)
	//{
	//	return 0;
	//}
}

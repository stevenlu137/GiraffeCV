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
 
 
 
#include "GPanoRenderQWidget.h"

#include <filesystem>
#include <iostream>
#include <fstream>

#if defined WIN32
#include "Windows.h"
#endif // WIN32

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/videoio/videoio.hpp"

#include "VideoSource/GCVideoSourceIF.h"
#include "PanoRender/GPanoRenderOGLIF.h"


void GPanoRenderQWidget::FeedIn(void *pPanoRenderQWidgetObj, int iCameraIdx)
{
	GPanoRenderQWidget* pObj = (GPanoRenderQWidget*)(pPanoRenderQWidgetObj);

	G_tVideoSourceCfg cfg;
	cfg.eVideoSourceType = G_eVideoSourceTypeURL;
	cfg.unConfig.pcVideoSourceCfgURL = pObj->m_vVideoPathes[iCameraIdx].c_str();

	int iNewVideoSourceHandle = -1;
	G_VideoSourceCreate(&iNewVideoSourceHandle, &cfg);

	if (-1 == iNewVideoSourceHandle)
	{
		return;
	}

	while (pObj->m_bFeedingFlag)
	{
		auto start = std::chrono::high_resolution_clock::now();

		G_tVideoFrame frame;
		if (G_eVideoSourceErrorNone != G_VideoSourceGrab(iNewVideoSourceHandle, &frame))
		{
			continue;
		}

		void* pData[3] = { (void*)(frame.pData[0]), (void*)(frame.pData[1]), (void*)(frame.pData[2]) };
		unsigned int Step[3] = { frame.uiLineStep_Byte[0], frame.uiLineStep_Byte[1], frame.uiLineStep_Byte[2] };

		G_InputFrame(pObj->m_iPanoRenderHandle, iCameraIdx, frame.uiWidth, frame.uiHeight
			, pData, Step);

		G_VideoSourceDrop(iNewVideoSourceHandle);

		auto end = std::chrono::high_resolution_clock::now();
		auto sleepFor = (std::chrono::milliseconds(30) - std::chrono::duration_cast<std::chrono::milliseconds>(end - start)).count();
		sleepFor = (sleepFor < 0) ? 0 : sleepFor;
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
	}

	G_VideoSourceRelease(iNewVideoSourceHandle);
}

GPanoRenderQWidget::GPanoRenderQWidget(QWidget *parent) :
	QWidget(parent)
      , m_bFeedingFlag(true)
      , m_iVideoSourceHandle(-1)
      , m_iPanoRenderHandle(-1)
      , m_bMouseLeftDown(false)
      , m_iX0(0)
      , m_iY0(0)
      , m_iWidth(0)
      , m_iHeight(0)
{
	//todo.
	//setAttribute(Qt::WA_NativeWindow);
	//setAttribute(Qt::WA_PaintOnScreen);
	//setAttribute(Qt::WA_NoSystemBackground);
	//setAutoFillBackground(true);
	SetupPixelFormat((void*)(this->winId()));

	this->setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);

	m_mpPanoType2String[G_ePanoTypeImmersionSemiSphere] = "ImmersionSemiSphere";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinder180] = "UnwrappedCylinder180";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinderSplited] = "UnwrappedCylinderSplited";
	m_mpPanoType2String[G_ePanoTypeUnwrappedSphere180] = "UnwrappedSphere180";
	m_mpPanoType2String[G_ePanoTypeUnwrappedSphere360] = "UnwrappedSphere360";
	m_mpPanoType2String[G_ePanoTypeUnwrappedSphere360Flip] = "UnwrappedSphere360Flip";
	m_mpPanoType2String[G_ePanoTypeOriginal] = "Original";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinder180Flip] = "UnwrappedCylinder180Flip";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinder360Flip] = "UnwrappedCylinder360Flip";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinderSplitedFrontBack] = "UnwrappedCylinderSplitedFrontBack";
	m_mpPanoType2String[G_ePanoTypeUnwrappedCylinder360] = "UnwrappedCylinder360";
	m_mpPanoType2String[G_ePanoTypeImmersionCylinder360] = "ImmersionCylinder360";
	m_mpPanoType2String[G_ePanoTypeImmersionCylinder360Inside] = "ImmersionCylinder360Inside";
	m_mpPanoType2String[G_ePanoTypeImmersionFullSphere] = "ImmersionFullSphere";
	m_mpPanoType2String[G_ePanoTypeUnwrappedFullSphere360] = "UnwrappedFullSphere360";
	m_mpPanoType2String[G_ePanoTypeUnwrappedFullSphere360Binocular] = "UnwrappedFullSphere360Binocular";
	m_mpPanoType2String[G_ePanoTypeImmersionFullSphereInside] = "ImmersionFullSphereInside";
	m_mpPanoType2String[G_ePanoTypeUnwrappedFullSphere360Flip] = "UnwrappedFullSphere360Flip";

	m_mpString2PanoType["ImmersionSemiSphere"] = G_ePanoTypeImmersionSemiSphere;
	m_mpString2PanoType["UnwrappedCylinder180"] = G_ePanoTypeUnwrappedCylinder180;
	m_mpString2PanoType["UnwrappedCylinderSplited"] = G_ePanoTypeUnwrappedCylinderSplited;
	m_mpString2PanoType["UnwrappedSphere180"] = G_ePanoTypeUnwrappedSphere180;
	m_mpString2PanoType["UnwrappedSphere360"] = G_ePanoTypeUnwrappedSphere360;
	m_mpString2PanoType["UnwrappedSphere360Flip"] = G_ePanoTypeUnwrappedSphere360Flip;
	m_mpString2PanoType["Original"] = G_ePanoTypeOriginal;
	m_mpString2PanoType["UnwrappedCylinder180Flip"] = G_ePanoTypeUnwrappedCylinder180Flip;
	m_mpString2PanoType["UnwrappedCylinder360Flip"] = G_ePanoTypeUnwrappedCylinder360Flip;
	m_mpString2PanoType["UnwrappedCylinderSplitedFrontBack"] = G_ePanoTypeUnwrappedCylinderSplitedFrontBack;
	m_mpString2PanoType["UnwrappedCylinder360"] = G_ePanoTypeUnwrappedCylinder360;
	m_mpString2PanoType["ImmersionCylinder360"] = G_ePanoTypeImmersionCylinder360;
	m_mpString2PanoType["ImmersionCylinder360Inside"] = G_ePanoTypeImmersionCylinder360Inside;
	m_mpString2PanoType["ImmersionFullSphere"] = G_ePanoTypeImmersionFullSphere;
	m_mpString2PanoType["UnwrappedFullSphere360"] = G_ePanoTypeUnwrappedFullSphere360;
	m_mpString2PanoType["UnwrappedFullSphere360Binocular"] = G_ePanoTypeUnwrappedFullSphere360Binocular;
	m_mpString2PanoType["ImmersionFullSphereInside"] = G_ePanoTypeImmersionFullSphereInside;
	m_mpString2PanoType["UnwrappedFullSphere360Flip"] = G_ePanoTypeUnwrappedFullSphere360Flip;

}

QPaintEngine *GPanoRenderQWidget::paintEngine() const
{
	return nullptr;
}

void GPanoRenderQWidget::mouseMoveEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		int iX=event->pos().x();
		int iY=event->pos().y();
		G_tViewCamera viewCamera;
		if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(m_iPanoRenderHandle, &viewCamera) ||
				(m_iWidth == 0) || (m_iHeight == 0))
		{
			return;
		}
		viewCamera.fHorizontal += 0.5 * static_cast<float>(iX - m_iX0) / (static_cast<float>((m_iWidth + m_iHeight) / 2));
		viewCamera.fVertical += 0.5 * static_cast<float>(iY - m_iY0) / (static_cast<float>((m_iWidth + m_iHeight) / 2));

		m_iX0 = iX;
		m_iY0 = iY;

		G_SetViewCamera(m_iPanoRenderHandle, viewCamera);
	}
}

void GPanoRenderQWidget::mousePressEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{
		m_iX0=event->pos().x();
		m_iY0=event->pos().y();
	}
}

void GPanoRenderQWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->buttons() & Qt::LeftButton)
	{}
}

void GPanoRenderQWidget::wheelEvent(QWheelEvent *event)
{
	G_tViewCamera viewCamera;
	if (G_ePanoRenderErrorCodeNone != G_GetViewCamera(m_iPanoRenderHandle, &viewCamera))
	{
		return;
	}

	if (event->angleDelta().y() > 0)
	{
		viewCamera.fZoom -= 0.1;
	}
	else
	{
		viewCamera.fZoom += 0.1;
	}
	G_SetViewCamera(m_iPanoRenderHandle, viewCamera);
}

void GPanoRenderQWidget::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);

	G_SetDrawingRegion(m_iPanoRenderHandle, 0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
	m_iWidth=this->width();
	m_iHeight=this->height();
}

void GPanoRenderQWidget::SetupPixelFormat(void* hWnd)
{
#if defined WIN32
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

	HDC hdc = GetDC((HWND)hWnd);
	if (nullptr == hdc)
	{
		return;
	}

	int iPixFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, iPixFormat, &pfd);
#endif // WIN32
}

void GPanoRenderQWidget::CreatePanoRender()
{
	std::filebuf* pBuf;
	std::ifstream ifs(m_sProfilePath, std::ios::in | std::ios::binary);
	if (!ifs.good())
	{
		return;
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

	G_CreatePanoRender(config, vModelFile.data(), iFileSize_Byte, &m_iPanoRenderHandle);
	G_InitPanoRender(m_iPanoRenderHandle, (void*)(this->winId()));
}

void GPanoRenderQWidget::SetProfilePath(const QString &sPath)
{
	m_sProfilePath = sPath.toStdString();
}

bool GPanoRenderQWidget::OpenVideos(const std::vector<QString>& vVideoPathes)
{
	Release();

	for (auto it=vVideoPathes.begin(); it!=vVideoPathes.end(); ++it)
	{
		m_vVideoPathes.push_back(it->toStdString());
	}

	CreatePanoRender();
	m_bFeedingFlag = true;
	for (int i = 0; i < m_vVideoPathes.size(); ++i)
	{
		m_vtdFeedIn.push_back(std::thread(GPanoRenderQWidget::FeedIn,this,i));
	}

	G_SetDrawingRegion(m_iPanoRenderHandle, 0, 0, this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
	m_iWidth=this->width();
	m_iHeight=this->height();

	m_vSupportedPanoTypeList.resize(G_enPanoType::G_ePanoTypeMaxNum);
	int iPanoTypeListSize = m_vSupportedPanoTypeList.size();
	G_GetSupportedPanoTypeList(m_iPanoRenderHandle, &(m_vSupportedPanoTypeList[0]), &iPanoTypeListSize);
	m_vSupportedPanoTypeList.resize(iPanoTypeListSize);

	return true;
}

std::vector<QString> GPanoRenderQWidget::GetPanoTypeList()
{
	std::vector<QString> vPanoTypeList;
	for (auto p : m_vSupportedPanoTypeList)
	{
		vPanoTypeList.push_back(QString::fromStdString(m_mpPanoType2String[p]));
	}

	return vPanoTypeList;
}

bool GPanoRenderQWidget::SetPanoType(const QString &sPanoType)
{
	return (G_ePanoRenderErrorCodeNone == G_SwitchPanoType(m_iPanoRenderHandle, m_mpString2PanoType[sPanoType.toStdString()]));
}

bool GPanoRenderQWidget::SnapShot()
{
	unsigned int uiWidth = 0;
	unsigned int uiHeight = 0;
	G_GetOriginalPanoSize(m_iPanoRenderHandle, &uiWidth, &uiHeight);

	cv::Mat mPano(uiHeight, uiWidth, CV_8UC3);
	bool bRet = (G_ePanoRenderErrorCodeNone == G_GrabPano(m_iPanoRenderHandle, &uiWidth, &uiHeight, mPano.data));
	cv::imwrite("pano.jpg", mPano);

	return bRet;
}

void GPanoRenderQWidget::Release()
{
	m_bFeedingFlag = false;
	for (auto it = m_vtdFeedIn.begin(); it != m_vtdFeedIn.end(); ++it)
	{
		if ((*it).joinable())
		{
			(*it).join();
		}
	}
	m_vtdFeedIn.clear();
	m_vVideoPathes.clear();

	G_DestroyPanoRender(m_iPanoRenderHandle);
	m_iPanoRenderHandle = -1;
}

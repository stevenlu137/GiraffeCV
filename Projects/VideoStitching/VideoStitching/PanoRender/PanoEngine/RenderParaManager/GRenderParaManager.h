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
 
 
 
#ifndef G_RENDER_PARA_MANAGER_H_
#define G_RENDER_PARA_MANAGER_H_

#include <mutex>

#include "PanoRender/GPanoRenderCommonIF.h"
#include "GPanoCameraInfo.h"
#include "GViewCamera.h"
#include "GPanoTypeStaticParameters.h"

struct GInteractionControllerDOF
{
	GInteractionControllerDOF() :
	fHorizontal(0.0)
	, fVertical(0.0)
	, fZoom(1.0)
	{}

	GInteractionControllerDOF(float fH, float fV, float fZ) :
		fHorizontal(fH)
		, fVertical(fV)
		, fZoom(fZ)
	{}

	float fHorizontal;
	float fVertical;
	float fZoom;
};

class GRenderParameterManager
{
public:
	GRenderParameterManager():
		m_bShouldCreateScene(false)
		, m_bShouldSetViewPort(false)
		, m_bShouldSetOriginalPanoSize(false)
		, m_iNumOfInputs(0)
	{}
	//for ExternalControl.
	void Init(const std::vector<GPanoTypeParas>& vPanoTypeList, int iNumOfInputs);
	void SetInputFrameSize(int iWidth, int iHeight);
	void SetInteractionControllerDOF(const GInteractionControllerDOF& dof);
	void SetPanoType(enPanoType panoType);
	void SetDrawRegion(int iX0, int iY0, int iWidth, int iHeight);

	void ApplyParameters();

	//for Render.
	enPanoType GetPanoType() const;
	std::vector<enPanoType>	GetSupportedPanoTypeList() const;
	bool IsPanoTypeSupported(enPanoType ePanoType) const;
	enViewCameraType GetViewCameraType() const;
	GInteractionControllerDOF ReadInteractionControllerDOF() const;
	cv::Size ReadOriginalPanoSize() const;
	cv::Rect_<float> GetPanoRectOnCanvas() const;
	cv::Rect GetDrawRegion() const;
	//v4fCropRatios = {left,right,top,down}.
	Eigen::Vector4f GetCropRatios() const;

	std::string GetK0Name() const;

	GViewCamera4RenderIF* GetViewCamera();

	bool ShouldCreateScene() const;
	bool ShouldSetViewPort() const;
	bool ShouldSetOriginalPanoSize() const;

	bool CalcInteractionControllerDOF(float fTheta, float fPhi, GInteractionControllerDOF& dof) const;

	//for ExternalReader.
	std::mutex m_mtManagerMutex;
private:
	struct tRenderParameters
	{
		tRenderParameters():
		m_fInteractionControllerDOF1(0.0)
		, m_fInteractionControllerDOF2(0.0)
		, m_fInteractionControllerDOFFOV(0.0)
		, m_iCurrentInputFrameWidth(0)
		, m_iCurrentInputFrameHeight(0)
		, m_eCurrentPanoType(ePanoTypeImmersionSemiSphere)
		, m_iCurrentDrawRegionX0(0)
		, m_iCurrentDrawRegionY0(0)
		, m_iCurrentDrawRegionWidth(0)
		, m_iCurrentDrawRegionHeight(0)
		{}

		//view camera.
		float m_fInteractionControllerDOF1;
		float m_fInteractionControllerDOF2;
		float m_fInteractionControllerDOFFOV;

		//input size.
		int m_iCurrentInputFrameWidth;
		int m_iCurrentInputFrameHeight;

		//pano type.
		enPanoType m_eCurrentPanoType;

		//Draw region.
		int m_iCurrentDrawRegionX0;
		int m_iCurrentDrawRegionY0;
		int m_iCurrentDrawRegionWidth;
		int m_iCurrentDrawRegionHeight;
	};

	GPanoTypeParas GetPanoTypeInfo(enPanoType ePanoType) const;

	void InitViewCamera(tRenderParameters& paras);

	cv::Rect_<float> GetPanoRectOnCanvas(enPanoType ePanoType) const;

	bool IsNewDOF() const;

	bool IsNewInputFrameSize() const;

	bool IsNewPanoType() const;
	
	bool IsNewDrawRegion() const;

	void FixDependencyPanoTypeSupport(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp);
	
	void FixDependencyZoomBound(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp);

	void FixDependencyPanoTypeSwitch(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp);

	void FixDependencyEyeLocationBound(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp);

private:
	std::vector<GPanoTypeParas> m_vPanoTypeInfo;
	tRenderParameters m_currentRenderParas;
	tRenderParameters m_tempRenderParas;

	int m_iNumOfInputs;

	bool m_bShouldCreateScene;
	bool m_bShouldSetViewPort;
	bool m_bShouldSetOriginalPanoSize;

	GViewCameraPerspectiveOutside m_viewCameraPerspectiveOutside;
	GViewCameraPerspectiveInside m_viewCameraPerspectiveInside;
	GViewCameraPerspectiveOutsideSurround m_viewCameraPerspectiveOutsideSurround;
	GViewCameraPerspectiveInsideSurround m_viewCameraPerspectiveInsideSurround;
	GViewCameraOrthogonal m_viewCameraOrthogonal;
};

#endif
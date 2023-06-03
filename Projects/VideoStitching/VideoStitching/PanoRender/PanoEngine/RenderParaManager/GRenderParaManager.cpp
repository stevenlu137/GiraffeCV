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
 
 
 
#include "GRenderParaManager.h"

#include "Common/Exception/GiraffeException.h"
#include "GGlobalParametersManager.h"
#include "PanoRender/GPanoSceneIF.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "Common/Math/GMathConstants.h"


void GRenderParameterManager::Init(const std::vector<GPanoTypeParas>& vPanoTypeList, int iNumOfInputs)
{
	m_vPanoTypeInfo = vPanoTypeList;
	m_iNumOfInputs = iNumOfInputs;

	//todo.
	m_currentRenderParas.m_eCurrentPanoType = vPanoTypeList[0].ePanoType;
	m_tempRenderParas.m_eCurrentPanoType = vPanoTypeList[0].ePanoType;

	InitViewCamera(m_tempRenderParas);
	InitViewCamera(m_currentRenderParas);
}

void GRenderParameterManager::InitViewCamera(tRenderParameters& paras)
{
	enViewCameraType eViewCameraType = GPanoTypeStaticParameters::GetDefaultViewCameraType(paras.m_eCurrentPanoType);
	if (eViewCameraTypePerspectiveOutside == eViewCameraType)
	{
		paras.m_fInteractionControllerDOF1 = 0.0;
		paras.m_fInteractionControllerDOF2 = 0.0;
		paras.m_fInteractionControllerDOFFOV = 1.0;
	}
	else if (eViewCameraTypePerspectiveInside == eViewCameraType)
	{
		paras.m_fInteractionControllerDOF1 = 0.0;
		paras.m_fInteractionControllerDOF2 = 0.0;
		paras.m_fInteractionControllerDOFFOV = 1.0;
	}
	else if (eViewCameraTypePerspectiveOutsideSurround == eViewCameraType)
	{
		paras.m_fInteractionControllerDOF1 = 0.0;
		paras.m_fInteractionControllerDOF2 = 0.0;
		paras.m_fInteractionControllerDOFFOV = 1.0;
	}
	else if (eViewCameraTypePerspectiveInsideSurround == eViewCameraType)
	{
		paras.m_fInteractionControllerDOF1 = 0.0;
		paras.m_fInteractionControllerDOF2 = 0.0;
		paras.m_fInteractionControllerDOFFOV = 1.0;
	}
	else if (eViewCameraTypeOrthogonal == eViewCameraType)
	{
		cv::Rect_<float> rect = GetPanoRectOnCanvas(paras.m_eCurrentPanoType);
		paras.m_fInteractionControllerDOF1 = ((rect.tl().x + rect.br().x) / (2.0 * rect.width)) + 0.5;
		paras.m_fInteractionControllerDOF2 = ((rect.tl().y + rect.br().y) / (2.0 * rect.height)) + 0.5;
		paras.m_fInteractionControllerDOFFOV = 1.0;
	}
	else
	{
		//exception.
	}
}

void GRenderParameterManager::SetInputFrameSize(int iWidth, int iHeight)
{
	m_tempRenderParas.m_iCurrentInputFrameWidth = iWidth;
	m_tempRenderParas.m_iCurrentInputFrameHeight = iHeight;
}

void GRenderParameterManager::SetInteractionControllerDOF(const GInteractionControllerDOF& dof)
{
	m_tempRenderParas.m_fInteractionControllerDOF1 = dof.fHorizontal;
	m_tempRenderParas.m_fInteractionControllerDOF2 = dof.fVertical;
	m_tempRenderParas.m_fInteractionControllerDOFFOV = dof.fZoom;
}

void GRenderParameterManager::SetPanoType(enPanoType panoType)
{
	m_tempRenderParas.m_eCurrentPanoType = panoType;
}

void GRenderParameterManager::SetDrawRegion(int iX0, int iY0, int iWidth, int iHeight)
{
	m_tempRenderParas.m_iCurrentDrawRegionX0 = iX0;
	m_tempRenderParas.m_iCurrentDrawRegionY0 = iY0;
	m_tempRenderParas.m_iCurrentDrawRegionWidth = iWidth;
	m_tempRenderParas.m_iCurrentDrawRegionHeight = iHeight;
}

void GRenderParameterManager::FixDependencyPanoTypeSupport(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp)
{
	if (IsNewPanoType())
	{
		if (!IsPanoTypeSupported(temp.m_eCurrentPanoType))
		{
			temp.m_eCurrentPanoType = current.m_eCurrentPanoType;
			GLOGGER(info) << "GRenderParameterManager::FixDependencyPanoTypeSupport(). pano type NOT SUPPORTED. just omit.";
		}
	}
}

void GRenderParameterManager::FixDependencyZoomBound(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp)
{
	temp.m_fInteractionControllerDOFFOV = (temp.m_fInteractionControllerDOFFOV < 0.3) ? 0.3 : temp.m_fInteractionControllerDOFFOV;
	temp.m_fInteractionControllerDOFFOV = (temp.m_fInteractionControllerDOFFOV > 10.0) ? 10.0 : temp.m_fInteractionControllerDOFFOV;
}

void GRenderParameterManager::FixDependencyPanoTypeSwitch(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp)
{
	if (IsNewPanoType())
	{
		if (!IsNewDOF())
		{
			InitViewCamera(temp);
		}
	}
}

void GRenderParameterManager::FixDependencyEyeLocationBound(const std::vector<GPanoTypeParas>& vPanoTypeInfo, tRenderParameters& current, tRenderParameters& temp)
{
	float fFOVZoom = 1.0 / temp.m_fInteractionControllerDOFFOV;
	if (eViewCameraTypeOrthogonal == GPanoTypeStaticParameters::GetDefaultViewCameraType(temp.m_eCurrentPanoType))
	{
		cv::Rect_<float> panoRect = GetPanoRectOnCanvas(temp.m_eCurrentPanoType);
		//calculate free domain.
		float fViewDomainWidth = fFOVZoom;
		float fViewDomainHeight = fFOVZoom
			* (static_cast<float>(temp.m_iCurrentDrawRegionHeight) / static_cast<float>(temp.m_iCurrentDrawRegionWidth))
			* (panoRect.width / panoRect.height);
		float fFreeDomainTLX = 0.0 + fViewDomainWidth / 2.0;
		float fFreeDomainTLY = 0.0 + fViewDomainHeight / 2.0;
		float fFreeDomainBRX = 1.0 - fViewDomainWidth / 2.0;
		float fFreeDomainBRY = 1.0 - fViewDomainHeight / 2.0;

		if (fFreeDomainTLX > fFreeDomainBRX)
		{
			fFreeDomainTLX = (fFreeDomainTLX + fFreeDomainBRX) / 2.0;
			fFreeDomainBRX = fFreeDomainTLX;
		}

		if (fFreeDomainTLY > fFreeDomainBRY)
		{
			fFreeDomainTLY = (fFreeDomainTLY + fFreeDomainBRY) / 2.0;
			fFreeDomainBRY = fFreeDomainTLY;
		}

		//restrict dof to free domain.
		temp.m_fInteractionControllerDOF1 = (temp.m_fInteractionControllerDOF1 < fFreeDomainTLX) ?
		fFreeDomainTLX : temp.m_fInteractionControllerDOF1;
		temp.m_fInteractionControllerDOF1 = (temp.m_fInteractionControllerDOF1 > fFreeDomainBRX) ?
		fFreeDomainBRX : temp.m_fInteractionControllerDOF1;
		temp.m_fInteractionControllerDOF2 = (temp.m_fInteractionControllerDOF2 < fFreeDomainTLY) ?
		fFreeDomainTLY : temp.m_fInteractionControllerDOF2;
		temp.m_fInteractionControllerDOF2 = (temp.m_fInteractionControllerDOF2 > fFreeDomainBRY) ?
		fFreeDomainBRY : temp.m_fInteractionControllerDOF2;
	}
}

bool GRenderParameterManager::IsNewDOF() const
{
	return (m_tempRenderParas.m_fInteractionControllerDOF1 != m_currentRenderParas.m_fInteractionControllerDOF1) ||
		(m_tempRenderParas.m_fInteractionControllerDOF2 != m_currentRenderParas.m_fInteractionControllerDOF2) ||
		(m_tempRenderParas.m_fInteractionControllerDOFFOV != m_currentRenderParas.m_fInteractionControllerDOFFOV);
}

bool GRenderParameterManager::IsNewInputFrameSize() const
{
	return (m_tempRenderParas.m_iCurrentInputFrameWidth != m_currentRenderParas.m_iCurrentInputFrameWidth) ||
		(m_tempRenderParas.m_iCurrentInputFrameHeight != m_currentRenderParas.m_iCurrentInputFrameHeight);
}

bool GRenderParameterManager::IsNewPanoType() const
{
	return (m_tempRenderParas.m_eCurrentPanoType != m_currentRenderParas.m_eCurrentPanoType);
}

bool GRenderParameterManager::IsNewDrawRegion() const
{
	return (m_tempRenderParas.m_iCurrentDrawRegionX0 != m_currentRenderParas.m_iCurrentDrawRegionX0) ||
		(m_tempRenderParas.m_iCurrentDrawRegionY0 != m_currentRenderParas.m_iCurrentDrawRegionY0) ||
		(m_tempRenderParas.m_iCurrentDrawRegionWidth != m_currentRenderParas.m_iCurrentDrawRegionWidth) ||
		(m_tempRenderParas.m_iCurrentDrawRegionHeight != m_currentRenderParas.m_iCurrentDrawRegionHeight);
}

void GRenderParameterManager::ApplyParameters()
{
	//check dependency.
	FixDependencyPanoTypeSupport(m_vPanoTypeInfo, m_currentRenderParas, m_tempRenderParas);
	FixDependencyZoomBound(m_vPanoTypeInfo, m_currentRenderParas, m_tempRenderParas);
	FixDependencyPanoTypeSwitch(m_vPanoTypeInfo, m_currentRenderParas, m_tempRenderParas);
	FixDependencyEyeLocationBound(m_vPanoTypeInfo, m_currentRenderParas, m_tempRenderParas);

	//action flag.
	m_bShouldCreateScene = (IsNewInputFrameSize() || IsNewPanoType()) && (0 != m_tempRenderParas.m_iCurrentInputFrameWidth) && (0 != m_tempRenderParas.m_iCurrentInputFrameHeight);

	m_bShouldSetViewPort = IsNewDrawRegion();

	m_bShouldSetOriginalPanoSize = IsNewDrawRegion() || IsNewInputFrameSize();

	//apply paras.
	m_currentRenderParas = m_tempRenderParas;
}

enPanoType GRenderParameterManager::GetPanoType() const
{
	return m_currentRenderParas.m_eCurrentPanoType;
}

bool GRenderParameterManager::IsPanoTypeSupported(enPanoType ePanoType) const
{
	for (int i = 0; i < m_vPanoTypeInfo.size(); ++i)
	{
		if (ePanoType == m_vPanoTypeInfo[i].ePanoType)
		{
			return true;
		}
	}

	return false;
}

cv::Rect_<float> GRenderParameterManager::GetPanoRectOnCanvas() const
{
	return GetPanoRectOnCanvas(GetPanoType());
}

cv::Rect_<float> GRenderParameterManager::GetPanoRectOnCanvas(enPanoType ePanoType) const
{
	return GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoType, GetCropRatios());
}

cv::Rect GRenderParameterManager::GetDrawRegion() const
{
	return cv::Rect(m_currentRenderParas.m_iCurrentDrawRegionX0, m_currentRenderParas.m_iCurrentDrawRegionY0
		, m_currentRenderParas.m_iCurrentDrawRegionWidth, m_currentRenderParas.m_iCurrentDrawRegionHeight);
}

enViewCameraType GRenderParameterManager::GetViewCameraType() const
{
	return GPanoTypeStaticParameters::GetDefaultViewCameraType(GetPanoType());
}

std::string GRenderParameterManager::GetK0Name() const
{
	return Size2K0Name(m_currentRenderParas.m_iCurrentInputFrameWidth, m_currentRenderParas.m_iCurrentInputFrameHeight);
}

bool GRenderParameterManager::ShouldCreateScene() const
{
	return m_bShouldCreateScene;
}

bool GRenderParameterManager::ShouldSetViewPort() const
{
	return m_bShouldSetViewPort;
}

bool GRenderParameterManager::ShouldSetOriginalPanoSize() const
{
	return m_bShouldSetOriginalPanoSize;
}

bool GRenderParameterManager::CalcInteractionControllerDOF(float fTheta, float fPhi, GInteractionControllerDOF& dof) const
{
	if (ePanoTypeImmersionSemiSphere == GetPanoType() ||
		ePanoTypeImmersionFullSphere == GetPanoType() ||
		ePanoTypeImmersionFullSphereInside == GetPanoType())
	{
		//for immersion panotype.
		//generate optimal view camera according to this physical direction.
		dof.fZoom = (0.0f == dof.fZoom) ? 1.0f : dof.fZoom;
		dof.fHorizontal = (3 * M_PI / 2.0 - fPhi) / (2.0f * M_PI);
		dof.fVertical = (fTheta / M_PI);
		//return false;
	}
	else if (ePanoTypeImmersionCylinder360 == GetPanoType() ||
		ePanoTypeImmersionCylinder360Inside == GetPanoType())
	{
		float fX = sinf(fTheta)*cosf(fPhi);
		float fY = sinf(fTheta)*sinf(fPhi);
		float fZ = cosf(fTheta);

		dof.fZoom = (0.0f == dof.fZoom) ? 1.0f : dof.fZoom;
		dof.fHorizontal = atan2f(fX, fZ) / (2.0f * M_PI);
		dof.fVertical = atan2f(-fY, sqrtf(fX*fX + fZ*fZ)) / M_PI;
	}
	else
	{
		//for unwrapped panotype.
		//physical direction -> scene point.
		Eigen::Vector3f v3fPhysical(sinf(fTheta)*cosf(fPhi), sinf(fTheta)*sinf(fPhi), cosf(fTheta));

		//scene point -> pix.
		std::shared_ptr<GPanoSceneIF> ptrRenderScene = GPanoSceneFactory::CreatePanoScene(GetPanoType());

		Eigen::Vector3f v3fScene;
		ptrRenderScene->Project(v3fPhysical, v3fScene);

		//generate optimal view camera according to this pixel coordinate.
		cv::Rect_<float> rtPanoRect = GetPanoRectOnCanvas();
		dof.fHorizontal = static_cast<float>(v3fScene[0] + (rtPanoRect.width / 2.0)) / rtPanoRect.width;
		dof.fVertical = static_cast<float>(v3fScene[1] + (rtPanoRect.height / 2.0)) / rtPanoRect.height;
		dof.fZoom = (0.0 == dof.fZoom) ? 1.0f : dof.fZoom;
	}

	return true;
}

GInteractionControllerDOF GRenderParameterManager::ReadInteractionControllerDOF() const
{
	return GInteractionControllerDOF(m_tempRenderParas.m_fInteractionControllerDOF1
		, m_tempRenderParas.m_fInteractionControllerDOF2
		, m_tempRenderParas.m_fInteractionControllerDOFFOV);
}

cv::Size GRenderParameterManager::ReadOriginalPanoSize() const
{
	if (opengl_compatible(3,0))
	{
		float fEffectivePixels =
			static_cast<float>(m_currentRenderParas.m_iCurrentInputFrameWidth * m_currentRenderParas.m_iCurrentInputFrameHeight
			* m_iNumOfInputs);

		int iDrawWidth = m_currentRenderParas.m_iCurrentDrawRegionWidth;
		int iDrawHeight = m_currentRenderParas.m_iCurrentDrawRegionHeight;

		if ((0 == iDrawWidth) || (0 == iDrawHeight))
		{
			return cv::Size(0, 0);
		}

		int iK = static_cast<int>(sqrt(fEffectivePixels / static_cast<float>(iDrawWidth * iDrawHeight)) + 1.0);
		iK = ((iK <= 0) ? 1 : iK);
	
		int iMaxRenderBufferSize = 0;
		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &iMaxRenderBufferSize);

		if ((eGGPUVendorNvidia != GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGPUVendor)) &&
			(eGGPUVendorAMD != GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGPUVendor)))
		{
			iMaxRenderBufferSize = (iMaxRenderBufferSize > 4096) ? 4096 : iMaxRenderBufferSize;
		}

		int iMaxKW = static_cast<int>(iMaxRenderBufferSize) / iDrawWidth;
		int iMaxKH = static_cast<int>(iMaxRenderBufferSize) / iDrawHeight;
		iMaxKW = (iMaxKW <= 0) ? 1 : iMaxKW;
		iMaxKH = (iMaxKH <= 0) ? 1 : iMaxKH;
		iK = (iK > iMaxKW) ? iMaxKW : iK;
		iK = (iK > iMaxKH) ? iMaxKH : iK;

		return cv::Size(iK * iDrawWidth, iK * iDrawHeight);
	}
	else
	{
		return cv::Size(m_currentRenderParas.m_iCurrentDrawRegionWidth, m_currentRenderParas.m_iCurrentDrawRegionHeight);
	}
}

Eigen::Vector4f GRenderParameterManager::GetCropRatios() const
{
	Eigen::Vector4f v4fCropRatios;
	v4fCropRatios[0] = GetPanoTypeInfo(GetPanoType()).fLeftCropRatio;
	v4fCropRatios[1] = GetPanoTypeInfo(GetPanoType()).fRightCropRatio;
	v4fCropRatios[2] = GetPanoTypeInfo(GetPanoType()).fTopCropRatio;
	v4fCropRatios[3] = GetPanoTypeInfo(GetPanoType()).fBottomCropRatio;
	return v4fCropRatios;
}

std::vector<enPanoType> GRenderParameterManager::GetSupportedPanoTypeList() const
{
	std::vector<enPanoType> vRet;
	for (int i = 0; i < m_vPanoTypeInfo.size(); ++i)
	{
		vRet.push_back(m_vPanoTypeInfo[i].ePanoType);
	}
	return vRet;
}

GPanoTypeParas GRenderParameterManager::GetPanoTypeInfo(enPanoType ePanoType) const
{
	for (int i = 0; i < m_vPanoTypeInfo.size(); ++i)
	{
		if (ePanoType == m_vPanoTypeInfo[i].ePanoType)
		{
			return m_vPanoTypeInfo[i];
		}
	}
	throw GPanoRenderException();
}

GViewCamera4RenderIF* GRenderParameterManager::GetViewCamera()
{
	float fWHRatio = ((0 == m_currentRenderParas.m_iCurrentDrawRegionWidth) || (0 == m_currentRenderParas.m_iCurrentDrawRegionHeight))
		? 0.0 :
		static_cast<float>(m_currentRenderParas.m_iCurrentDrawRegionWidth) / static_cast<float>(m_currentRenderParas.m_iCurrentDrawRegionHeight);

	if (eViewCameraTypePerspectiveOutside == GetViewCameraType())
	{
		const float cfEyeLocationPerspective = 1.5f;
		const float cfDefaultFOVPerspective = 60.0;
		m_viewCameraPerspectiveOutside.SetEyeLocation(cfEyeLocationPerspective);
		m_viewCameraPerspectiveOutside.SetPan(m_currentRenderParas.m_fInteractionControllerDOF1 * 360.0);
		m_viewCameraPerspectiveOutside.SetTilt(m_currentRenderParas.m_fInteractionControllerDOF2 * 180.0);
		m_viewCameraPerspectiveOutside.SetFOV_Degree(atan(tan(cfDefaultFOVPerspective * M_PI / 360.0)
			/ m_currentRenderParas.m_fInteractionControllerDOFFOV) * 360.0 / M_PI);
		m_viewCameraPerspectiveOutside.SetWHRatio(fWHRatio);

		return &m_viewCameraPerspectiveOutside;
	}
	else if (eViewCameraTypePerspectiveInside == GetViewCameraType())
	{
		const float cfDefaultFOVPerspective = 60.0;
		m_viewCameraPerspectiveInside.SetPan(m_currentRenderParas.m_fInteractionControllerDOF1 * 360.0);
		m_viewCameraPerspectiveInside.SetTilt(m_currentRenderParas.m_fInteractionControllerDOF2 * 180.0);
		m_viewCameraPerspectiveInside.SetFOV_Degree(atan(tan(cfDefaultFOVPerspective * M_PI / 360.0)
			/ m_currentRenderParas.m_fInteractionControllerDOFFOV) * 360.0 / M_PI);
		m_viewCameraPerspectiveInside.SetWHRatio(fWHRatio);

		return &m_viewCameraPerspectiveInside;
	}
	else if (eViewCameraTypePerspectiveOutsideSurround == GetViewCameraType())
	{
		const float cfEyeLocationPerspective = 1.5f;
		const float cfDefaultFOVPerspective = 60.0;
		m_viewCameraPerspectiveOutsideSurround.SetEyeLocation(cfEyeLocationPerspective);
		m_viewCameraPerspectiveOutsideSurround.SetPan(m_currentRenderParas.m_fInteractionControllerDOF1 * 360.0);
		m_viewCameraPerspectiveOutsideSurround.SetTilt(m_currentRenderParas.m_fInteractionControllerDOF2 * 180.0);
		m_viewCameraPerspectiveOutsideSurround.SetFOV_Degree(atan(tan(cfDefaultFOVPerspective * M_PI / 360.0)
			/ m_currentRenderParas.m_fInteractionControllerDOFFOV) * 360.0 / M_PI);
		m_viewCameraPerspectiveOutsideSurround.SetWHRatio(fWHRatio);

		return &m_viewCameraPerspectiveOutsideSurround;
	}
	else if (eViewCameraTypePerspectiveInsideSurround == GetViewCameraType())
	{
		const float cfDefaultFOVPerspective = 60.0;
		m_viewCameraPerspectiveInsideSurround.SetPan(m_currentRenderParas.m_fInteractionControllerDOF1 * 360.0);
		m_viewCameraPerspectiveInsideSurround.SetTilt(m_currentRenderParas.m_fInteractionControllerDOF2 * 180.0);
		m_viewCameraPerspectiveInsideSurround.SetFOV_Degree(atan(tan(cfDefaultFOVPerspective * M_PI / 360.0)
			/ m_currentRenderParas.m_fInteractionControllerDOFFOV) * 360.0 / M_PI);
		m_viewCameraPerspectiveInsideSurround.SetWHRatio(fWHRatio);

		return &m_viewCameraPerspectiveInsideSurround;
	}
	else if (eViewCameraTypeOrthogonal == GetViewCameraType())
	{
		cv::Rect_<float> panoRect = GetPanoRectOnCanvas();
		float fDeltaX = (panoRect.tl().x + panoRect.br().x) / 2.0;
		float fDeltaY = (panoRect.tl().y + panoRect.br().y) / 2.0;
		m_viewCameraOrthogonal.SetEyeLocationX((m_currentRenderParas.m_fInteractionControllerDOF1 - 0.5) * panoRect.width + fDeltaX);
		m_viewCameraOrthogonal.SetEyeLocationY((m_currentRenderParas.m_fInteractionControllerDOF2 - 0.5) * panoRect.height + fDeltaY);
		m_viewCameraOrthogonal.SetWidth(panoRect.width / m_currentRenderParas.m_fInteractionControllerDOFFOV);
		m_viewCameraOrthogonal.SetWHRatio(fWHRatio);

		return &m_viewCameraOrthogonal;
	}

	return nullptr;
}
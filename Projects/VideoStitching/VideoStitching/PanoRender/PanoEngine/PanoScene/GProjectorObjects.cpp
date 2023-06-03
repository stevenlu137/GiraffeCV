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
 
 
 
#include "GProjectorObjects.h"
#include "Common/Math/GMathConstants.h"
#include "../Common/GRenderProjectors.h"


void GProjectorObjectIF::SetCameraIdx(int iCameraIdx)
{
	m_iCameraIdx = iCameraIdx;
}

int GProjectorObjectIF::GetCameraIdx() const
{
	return m_iCameraIdx;
}

cv::Point3f GProjectorObjectIF::operator()(const cv::Point3f& ptfSrc) const
{
	if (m_lmPixNeeded(ptfSrc.x, ptfSrc.y, ptfSrc.z))
	{
		return Map(ptfSrc);
	}
	return cv::Point3f(FLT_MAX, FLT_MAX, 0.0f);
}

void GProjectorObjectIF::SetSelectiveFunctor(std::function<bool(float fX, float fY, float fZ)> selectiveFunctor)
{
	m_lmPixNeeded = selectiveFunctor;
}

void GProjectorObjectGlobal3d2NormalizedPix::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectGlobal3d2NormalizedPix::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectGlobal3d2NormalizedPix::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectGlobal3d2NormalizedPix::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectGlobal3d2NormalizedPix::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

cv::Point3f GProjectorObjectGlobal3d2NormalizedPix::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = Global3d2NormalizedPix(tCoordSrc
		, m_eCameratype, m_fDistortionValidRange
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetTheta(float fTheta)
{
	m_fTheta = fTheta;
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

void GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

cv::Point3f GProjectorObjectCylinderExpandedPlane3d2NormalizedPix::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = CylinderExpandedPlane3d2NormalizedPix(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, m_bFlip
		, m_fTheta
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

cv::Point3f GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = CylinderExpandedPlaneSplited3d2NormalizedPix(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

cv::Point3f GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = CylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

cv::Point3f GProjectorObjectSphereExpandedPlane3d2NormalizedPix180::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = SphereExpandedPlane3d2NormalizedPix180(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::SetIsFullSphere(bool bIsFullSphere)
{
	m_bFullSphere = bIsFullSphere;
}

cv::Point3f GProjectorObjectSphereExpandedPlane3d2NormalizedPix360::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = SphereExpandedPlane3d2NormalizedPix360(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, m_bFlip
		, (m_bFullSphere) ? (M_PI / 2.0) : (M_PI / 4.0)
		, (const float*)m_mK1.data, (const float*)m_mDist1.data, (const float*)m_mR10.data);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetR10(const cv::Mat& mR10)
{
	mR10.convertTo(m_mR10, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetDist1(const cv::Mat& mDist1)
{
	mDist1.convertTo(m_mDist1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetK1(const cv::Mat& mK1)
{
	mK1.convertTo(m_mK1, CV_32FC1);
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetCameraType(enCameraType eCameratype)
{
	m_eCameratype = eCameratype;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetIsLeft(bool bIsLeft)
{
	m_bIsLeft = bIsLeft;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetDistortionValidRange(float fDistortionValidRange)
{
	m_fDistortionValidRange = fDistortionValidRange;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetPhi(float fPhiMin, float fPhiMax)
{
	m_fPhiMin = fPhiMin;
	m_fPhiMax = fPhiMax;
}

void GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::SetTheta(float fThetaMin, float fThetaMax)
{
	m_fThetaMin = fThetaMin;
	m_fThetaMax = fThetaMax;
}

cv::Point3f GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = SphereExpandedPlane3d2NormalizedPix360CircularBinocular(tCoordSrc
		, m_eCameratype
		, m_fDistortionValidRange
		, m_bIsLeft
		, m_fPhiMin, m_fPhiMax, m_fThetaMin, m_fThetaMax
		, (const float*)(m_mK1.data), (const float*)(m_mDist1.data), (const float*)(m_mR10.data));

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorObjectOriginal3d2NormalizedPix::SetNumOfCameras(int iNumOfCameras)
{
	m_iNumOfCameras = iNumOfCameras;
}

void GProjectorObjectOriginal3d2NormalizedPix::SetIndexOfCamera(int iIdxOfCamera)
{
	m_iIdxOfCamera = iIdxOfCamera;
}

cv::Point3f GProjectorObjectOriginal3d2NormalizedPix::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = Original3d2NormalizedPix(tCoordSrc
		, m_iNumOfCameras, m_iIdxOfCamera);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

cv::Point3f GProjectorScenePhysicalIF::operator()(const cv::Point3f& ptfSrc) const
{
	return Map(ptfSrc);
}

void GProjectorCylinderExpandedPlane3d2PhysicalDirection::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

void GProjectorCylinderExpandedPlane3d2PhysicalDirection::SetTheta(float fTheta)
{
	m_fTheta = fTheta;
}

cv::Point3f GProjectorCylinderExpandedPlane3d2PhysicalDirection::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = CylinderExpandedPlane3d2PhysicalDirection(tCoordSrc, m_bFlip, m_fTheta);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

cv::Point3f GProjectorCylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = CylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection(tCoordSrc);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

cv::Point3f GProjectorSphereExpandedPlane3d2PhysicalDirection180::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = SphereExpandedPlane3d2PhysicalDirection180(tCoordSrc);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorSphereExpandedPlane3d2PhysicalDirection360::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

void GProjectorSphereExpandedPlane3d2PhysicalDirection360::SetIsFullSphere(bool bIsFullSphere)
{
	m_bFullSphere = bIsFullSphere;
}

cv::Point3f GProjectorSphereExpandedPlane3d2PhysicalDirection360::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = SphereExpandedPlane3d2PhysicalDirection360(tCoordSrc, m_bFlip, (m_bFullSphere) ? (M_PI / 2.0f) : (M_PI / 4.0f));

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorPhysicalDirection2CylinderExpandedPlane3d::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

void GProjectorPhysicalDirection2CylinderExpandedPlane3d::SetTheta(float fTheta)
{
	m_fTheta = fTheta;
}

cv::Point3f GProjectorPhysicalDirection2CylinderExpandedPlane3d::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = PhysicalDirection2CylinderExpandedPlane3d(tCoordSrc, m_bFlip, m_fTheta);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

cv::Point3f GProjectorPhysicalDirection2CylinderExpandedPlaneSplitedFrontBack3d::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = PhysicalDirection2CylinderExpandedPlaneSplitedFrontBack3d(tCoordSrc);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

cv::Point3f GProjectorPhysicalDirection2SphereExpandedPlane3d180::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = PhysicalDirection2SphereExpandedPlane3d180(tCoordSrc);

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}

void GProjectorPhysicalDirection2SphereExpandedPlane3d360::SetFlip(bool bFlip)
{
	m_bFlip = bFlip;
}

void GProjectorPhysicalDirection2SphereExpandedPlane3d360::SetIsFullSphere(bool bIsFullSphere)
{
	m_bFullSphere = bIsFullSphere;
}

cv::Point3f GProjectorPhysicalDirection2SphereExpandedPlane3d360::Map(const cv::Point3f& ptfSrc) const
{
	tCoordinate3D tCoordSrc(0.0f, 0.0f, 0.0f);
	tCoordSrc.fX = ptfSrc.x;
	tCoordSrc.fY = ptfSrc.y;
	tCoordSrc.fZ = ptfSrc.z;

	tCoordinate3D tRet = PhysicalDirection2SphereExpandedPlane3d360(tCoordSrc, m_bFlip, (m_bFullSphere) ? (M_PI / 2.0f) : (M_PI / 4.0f));

	return cv::Point3f(tRet.fX, tRet.fY, tRet.fZ);
}


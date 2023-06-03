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
 
 
 
#ifndef G_PROJECTOR_OBJECTS_H_
#define G_PROJECTOR_OBJECTS_H_

#include <functional>

#include "opencv2/core/core.hpp"
#include "Common/Camera/GCameraType.h"
#include "Common/Math/GMathConstants.h"


class GProjectorObjectIF
{
public:
	GProjectorObjectIF() :
		m_iCameraIdx(0)
		, m_lmPixNeeded([](float fX, float fY, float fZ){return true; })
	{}

	void SetCameraIdx(int iCameraIdx);
	int GetCameraIdx() const;

	cv::Point3f operator() (const cv::Point3f& ptfSrc) const;
	void SetSelectiveFunctor(std::function<bool(float fX, float fY, float fZ)> selectiveFunctor);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptSrc) const = 0;

private:
	int m_iCameraIdx;
	std::function<bool(float fX, float fY, float fZ)> m_lmPixNeeded;
};

class GProjectorScenePhysicalIF
{
public:
	cv::Point3f operator() (const cv::Point3f& ptfSrc) const;

private:
	virtual cv::Point3f Map(const cv::Point3f& ptSrc) const = 0;
};

class GProjectorObjectGlobal3d2NormalizedPix : public GProjectorObjectIF
{
public:
	GProjectorObjectGlobal3d2NormalizedPix() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
};

class GProjectorObjectCylinderExpandedPlane3d2NormalizedPix : public GProjectorObjectIF
{
public:
	GProjectorObjectCylinderExpandedPlane3d2NormalizedPix() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
		, m_fTheta(0.0)
		, m_bFlip(false)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);
	void SetTheta(float fTheta);
	void SetFlip(bool bFlip);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
	float m_fTheta;
	bool m_bFlip;
};

class GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix : public GProjectorObjectIF
{
public:
	GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
};

class GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix : public GProjectorObjectIF
{
public:
	GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
};

class GProjectorObjectSphereExpandedPlane3d2NormalizedPix180 : public GProjectorObjectIF
{
public:
	GProjectorObjectSphereExpandedPlane3d2NormalizedPix180() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
	std::function<bool(float fX, float fY)> m_lmPixNeeded;
};

class GProjectorObjectSphereExpandedPlane3d2NormalizedPix360 : public GProjectorObjectIF
{
public:
	GProjectorObjectSphereExpandedPlane3d2NormalizedPix360() :
		m_eCameratype(eCameraTypeNormal)
		, m_fDistortionValidRange(0.0)
		, m_bFlip(false)
		, m_bFullSphere(false)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetDistortionValidRange(float fDistortionValidRange);
	void SetFlip(bool bFlip);
	void SetIsFullSphere(bool bIsFullSphere);
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	float m_fDistortionValidRange;
	bool m_bFlip;
	bool m_bFullSphere;
};

class GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular : public GProjectorObjectIF
{
public:
	GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular() :
		m_eCameratype(eCameraTypeNormal)
		, m_bIsLeft(true)
		, m_fDistortionValidRange(0.0)
		, m_fPhiMin(-M_PI)
		, m_fPhiMax(M_PI)
		, m_fThetaMin(-M_PI / 2.0)
		, m_fThetaMax(M_PI / 2.0)
	{}

public:
	void SetR10(const cv::Mat& mR10);
	void SetDist1(const cv::Mat& mDist1);
	void SetK1(const cv::Mat& mK1);
	void SetCameraType(enCameraType eCameratype);
	void SetIsLeft(bool bIsLeft);
	void SetDistortionValidRange(float fDistortionValidRange);
	void SetPhi(float fPhiMin, float fPhiMax);
	void SetTheta(float fThetaMin, float fThetaMax);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	cv::Mat m_mR10;
	cv::Mat m_mDist1;
	cv::Mat m_mK1;
	enCameraType m_eCameratype;
	bool m_bIsLeft;
	float m_fDistortionValidRange;
	float m_fPhiMin;
	float m_fPhiMax;
	float m_fThetaMin;
	float m_fThetaMax;
};

class GProjectorObjectOriginal3d2NormalizedPix : public GProjectorObjectIF
{
public:
	GProjectorObjectOriginal3d2NormalizedPix()
	{}

public:
	void SetNumOfCameras(int iNumOfCameras);
	void SetIndexOfCamera(int iIdxOfCamera);
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
private:
	int m_iNumOfCameras;
	int m_iIdxOfCamera;
};

class GProjectorCylinderExpandedPlane3d2PhysicalDirection : public GProjectorScenePhysicalIF
{
public:
	GProjectorCylinderExpandedPlane3d2PhysicalDirection():
		m_fTheta(0.0)
		, m_bFlip(false)
	{}

	void SetTheta(float fTheta);
	void SetFlip(bool bFlip);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;

private:
	float m_fTheta;
	bool m_bFlip;

};

class GProjectorCylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection : public GProjectorScenePhysicalIF
{
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
};

class GProjectorSphereExpandedPlane3d2PhysicalDirection180 : public GProjectorScenePhysicalIF
{
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
};

class GProjectorSphereExpandedPlane3d2PhysicalDirection360 : public GProjectorScenePhysicalIF
{
public:
	GProjectorSphereExpandedPlane3d2PhysicalDirection360() :
		m_bFlip(false)
		, m_bFullSphere(false)
	{}
	void SetFlip(bool bFlip);
	void SetIsFullSphere(bool bIsFullSphere);
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;

private:
	bool m_bFlip;
	bool m_bFullSphere;
};

class GProjectorPhysicalDirection2CylinderExpandedPlane3d : public GProjectorScenePhysicalIF
{
public:
	GProjectorPhysicalDirection2CylinderExpandedPlane3d():
		m_fTheta(0.0)
		, m_bFlip(false)
	{}
	void SetTheta(float fTheta);
	void SetFlip(bool bFlip);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;

private:
	float m_fTheta;
	bool m_bFlip;
};

class GProjectorPhysicalDirection2CylinderExpandedPlaneSplitedFrontBack3d : public GProjectorScenePhysicalIF
{
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
};

class GProjectorPhysicalDirection2SphereExpandedPlane3d180 : public GProjectorScenePhysicalIF
{
private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;
};

class GProjectorPhysicalDirection2SphereExpandedPlane3d360 : public GProjectorScenePhysicalIF
{
public:
	GProjectorPhysicalDirection2SphereExpandedPlane3d360():
		m_bFlip(false)
		, m_bFullSphere(false)
	{}
	
	void SetFlip(bool bFlip);
	void SetIsFullSphere(bool bIsFullSphere);

private:
	virtual cv::Point3f Map(const cv::Point3f& ptfSrc) const;

private:
	bool m_bFlip;
	bool m_bFullSphere;
};

#endif//!G_PROJECTOR_OBJECTS_H_
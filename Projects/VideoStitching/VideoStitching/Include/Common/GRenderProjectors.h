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
 
 
 
#ifndef G_RENDER_PROJECTORS_H_
#define G_RENDER_PROJECTORS_H_

#include "Common/LanguageSwitch/GCPPCUDASwitch.h"
#include "Geometry/GeometryBase/GHomogeneousCoordinate.h"
#include "Geometry/GeometryBase/GBasicProjectors.h"
#include "Geometry/GeometryBase/GCameraProjectors.h"
#include "Common/Math/GMathConstants.h"


G_CUDA_HOST_DEVICE_FORCEINLINE float CalcWeight(float fX, float fY, const float* pK, float fDistortionValidRange)
{
	float fRet = 0.0f;
	float fFocalX = static_cast<float>(pK[0]);
	float fFocalY = static_cast<float>(pK[4]);

	float fTempX = 0.0f;
	float fTempY = 0.0f;

	fTempX = (fX >= 0.5f) ?
		((1.0f - fX) / fFocalX) : (fX / fFocalX);
	fTempY = (fY >= 0.5f) ?
		((1.0f - fY) / fFocalY) : (fY / fFocalY);

	fRet = (fTempX < fTempY) ? fTempX : fTempY;

	float fXCenter = (fX - static_cast<float>(pK[2])) / fFocalX;
	float fYCenter = (fY - static_cast<float>(pK[5])) / fFocalY;
	float fTempR = fDistortionValidRange - sqrtf(fXCenter * fXCenter + fYCenter * fYCenter);

	fRet = (fTempR < fRet) ? fTempR : fRet;
	return (fRet <= 0.0f) ? 0.0f : fRet;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D CylinderExpandedPlane3d2PhysicalDirection(const tCoordinate3D& ptSrc, bool bFlip, float fTheta)
{
	tCoordinate2D pSrcCylindrical(bFlip ? -ptSrc.fY : ptSrc.fY, bFlip ? -ptSrc.fX : ptSrc.fX);
	tCoordinate3D pTempSrc(sinf(pSrcCylindrical.fY), pSrcCylindrical.fX, cosf(pSrcCylindrical.fY));
	tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);

	pSrc.fX = pTempSrc.fX;
	pSrc.fY = pTempSrc.fY*cosf(fTheta) + pTempSrc.fZ*sinf(fTheta);
	pSrc.fZ = -pTempSrc.fY*sinf(fTheta) + pTempSrc.fZ*cosf(fTheta);
	return pSrc;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D CylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection(const tCoordinate3D& ptSrc)
{
	tCoordinate2D pSrcCylindrical(0.0f, 0.0f);
	if (-1.0f <= ptSrc.fY && ptSrc.fY <= 0.0f)
	{
		pSrcCylindrical.fX = ptSrc.fY + 2.0f / 4.0f;
		pSrcCylindrical.fY = ptSrc.fX;
	}
	else
	{
		pSrcCylindrical.fX = ptSrc.fY - 2.0f / 4.0f;
		pSrcCylindrical.fY = -(ptSrc.fX + M_PI);
	}

	tCoordinate3D pSrc(sinf(pSrcCylindrical.fY), pSrcCylindrical.fX, cosf(pSrcCylindrical.fY));
	return pSrc;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D SphereExpandedPlane3d2PhysicalDirection180(const tCoordinate3D& ptSrc)
{
	//Sphere expand plane 180
	tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);
	pSrc.fX = cosf(ptSrc.fY)*sinf(ptSrc.fX);
	pSrc.fY = sinf(ptSrc.fY);
	pSrc.fZ = cosf(ptSrc.fY)*cosf(ptSrc.fX);
	return pSrc;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D SphereExpandedPlane3d2PhysicalDirection360(const tCoordinate3D& ptSrc, bool bFlip, float fYOffset)
{
	tCoordinate2D ptSrcFliped(bFlip ? ptSrc.fX : -ptSrc.fX, bFlip ? ptSrc.fY : -ptSrc.fY);

	tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);
	pSrc.fX = sinf(ptSrcFliped.fY + fYOffset)*cosf(ptSrcFliped.fX);
	pSrc.fY = -sinf(ptSrcFliped.fY + fYOffset)*sinf(ptSrcFliped.fX);
	pSrc.fZ = cosf(ptSrcFliped.fY + fYOffset);
	return pSrc;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D PhysicalDirection2CylinderExpandedPlane3d(const tCoordinate3D& ptSrc, bool bFlip, float fTheta)
{
	tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);
	pSrc.fX = ptSrc.fX;
	pSrc.fY = ptSrc.fY*cosf(fTheta) - ptSrc.fZ*sinf(fTheta);
	pSrc.fZ = ptSrc.fY*sinf(fTheta) + ptSrc.fZ*cosf(fTheta);

	return tCoordinate3D(bFlip ? -atan2f(pSrc.fX, pSrc.fZ) : atan2f(pSrc.fX, pSrc.fZ), bFlip ? -pSrc.fY : pSrc.fY, 0.0f);//to check.
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D PhysicalDirection2CylinderExpandedPlaneSplitedFrontBack3d(const tCoordinate3D& ptSrc)
{
	tCoordinate3D pSrc(atan2f(ptSrc.fX, ptSrc.fZ), ptSrc.fY, 0.0f);
	if (-(M_PI / 2.0f) <= pSrc.fX && pSrc.fX <= (M_PI / 2.0f))
	{
		pSrc.fY = pSrc.fY - 2.0f / 4.0f;
	}
	else
	{
		pSrc.fX = M_PI - pSrc.fX + (((M_PI / 2.0f) <= pSrc.fX) ? 0.0f : (-2.0f * M_PI));
		pSrc.fY = pSrc.fY + 2.0f / 4.0f;
	}
	return pSrc;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D PhysicalDirection2SphereExpandedPlane3d180(const tCoordinate3D& ptSrc)
{
	return tCoordinate3D(atan2f(ptSrc.fX, ptSrc.fZ), asinf(ptSrc.fY), 0.0f);//to check.
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D PhysicalDirection2SphereExpandedPlane3d360(const tCoordinate3D& ptSrc, bool bFlip, float fYOffset)
{
	float fTheta = acosf(ptSrc.fZ) - fYOffset;
	float fPhi = atan2f(-ptSrc.fY, ptSrc.fX);
	return tCoordinate3D(bFlip ? fPhi : -fPhi, bFlip ? fTheta : -fTheta, 0.0f);//to check.
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D Global3d2NormalizedPix(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange
	, const float* pK1, const float* pDist1, const float* pR10)
{
	tCoordinate3D pTemp = MatrixProjector(ptSrc, pR10);

	if (pTemp.fZ <= 0.0f)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	tCoordinate2D pRet(0.0f, 0.0f);
	if (eCameraTypeNormal == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeNormal>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheye == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheye>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheyeAnglePoly5 == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(pTemp, pK1, pDist1);
	}

	return tCoordinate3D(pRet.fX, pRet.fY, CalcWeight(pRet.fX, pRet.fY, pK1, fDistortionValidRange));
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D CylinderExpandedPlane3d2NormalizedPix(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange, bool bFlip, float fTheta
	, const float* pK1, const float* pDist1, const float* pR10)
{
	return Global3d2NormalizedPix(
		CylinderExpandedPlane3d2PhysicalDirection(ptSrc, bFlip, fTheta)
		, eCameratype, fDistortionValidRange, pK1, pDist1, pR10);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D SphereExpandedPlane3d2NormalizedPix180(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange
	, const float* pK1, const float* pDist1, const float* pR10)
{
	return Global3d2NormalizedPix(
		SphereExpandedPlane3d2PhysicalDirection180(ptSrc)
		, eCameratype, fDistortionValidRange, pK1, pDist1, pR10);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D SphereExpandedPlane3d2NormalizedPix360(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange, bool bFlip, float fYOffset
	, const float* pK1, const float* pDist1, const float* pR10)
{
	return Global3d2NormalizedPix(
		SphereExpandedPlane3d2PhysicalDirection360(ptSrc, bFlip, fYOffset)
		, eCameratype, fDistortionValidRange, pK1, pDist1, pR10);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D CylinderExpandedPlaneSplited3d2NormalizedPix(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange
	, const float* pK1, const float* pDist1, const float* pR10)
{
	if (-1.0f <= ptSrc.fY && ptSrc.fY <= 0.0f)
	{
		tCoordinate2D pSrcCylindrical(ptSrc.fY + 2.0f / 4.0f, ptSrc.fX);
		tCoordinate3D pTempSrc(sinf(pSrcCylindrical.fY), pSrcCylindrical.fX, cosf(pSrcCylindrical.fY));
		tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);

		float fTheta = -M_PI / 4.0f;//to do
		pSrc.fX = pTempSrc.fX;
		pSrc.fY = pTempSrc.fY*cosf(fTheta) + pTempSrc.fZ*sinf(fTheta);
		pSrc.fZ = -pTempSrc.fY*sinf(fTheta) + pTempSrc.fZ*cosf(fTheta);

		tCoordinate3D pTemp = MatrixProjector(pSrc, pR10);

		if (pTemp.fZ <= 0.0f)
		{
			return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
		}

		tCoordinate2D pRet(0.0f, 0.0f);
		if (eCameraTypeNormal == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeNormal>(pTemp, pK1, pDist1);
		}
		else if (eCameraTypeFisheye == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeFisheye>(pTemp, pK1, pDist1);
		}
		else if (eCameraTypeFisheyeAnglePoly5 == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(pTemp, pK1, pDist1);
		}

		if (pRet.fY <= 0.5f)
		{
			return tCoordinate3D(pRet.fX, pRet.fY, CalcWeight(pRet.fX, pRet.fY, pK1, fDistortionValidRange));
		}
		else
		{
			return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
		}

	}
	else
	{
		tCoordinate2D pSrcCylindrical(ptSrc.fY - 2.0f / 4.0f, (ptSrc.fX));
		tCoordinate3D pTempSrc(sinf(pSrcCylindrical.fY), pSrcCylindrical.fX, cosf(pSrcCylindrical.fY));
		tCoordinate3D pSrc(0.0f, 0.0f, 0.0f);

		float fTheta = M_PI / 4.0f;
		pSrc.fX = pTempSrc.fX;
		pSrc.fY = pTempSrc.fY*cosf(fTheta) + pTempSrc.fZ*sinf(fTheta);
		pSrc.fZ = -pTempSrc.fY*sinf(fTheta) + pTempSrc.fZ*cosf(fTheta);

		tCoordinate3D pTemp = MatrixProjector(pSrc, pR10);

		if (pTemp.fZ < 0.0f)
		{
			return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
		}

		tCoordinate2D pRet(0.0f, 0.0f);
		if (eCameraTypeNormal == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeNormal>(pTemp, pK1, pDist1);
		}
		else if (eCameraTypeFisheye == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeFisheye>(pTemp, pK1, pDist1);
		}
		else if (eCameraTypeFisheyeAnglePoly5 == eCameratype)
		{
			pRet = Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(pTemp, pK1, pDist1);
		}

		if (pRet.fY > 0.5f)
		{
			return tCoordinate3D(pRet.fX, pRet.fY, CalcWeight(pRet.fX, pRet.fY, pK1, fDistortionValidRange));
		}
		else
		{
			return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
		}
	}
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D CylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange
	, const float* pK1, const float* pDist1, const float* pR10)
{
	tCoordinate3D pPhysical = CylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection(ptSrc);

	tCoordinate3D pTemp = MatrixProjector(pPhysical, pR10);

	if (pTemp.fZ < 0.0f)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	tCoordinate2D pRet(0.0f, 0.0f);
	if (eCameraTypeNormal == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeNormal>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheye == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheye>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheyeAnglePoly5 == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(pTemp, pK1, pDist1);
	}

	return tCoordinate3D(pRet.fX, pRet.fY, CalcWeight(pRet.fX, pRet.fY, pK1, fDistortionValidRange));
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D SphereExpandedPlane3d2NormalizedPix360CircularBinocular(const tCoordinate3D& ptSrc, enCameraType eCameratype, float fDistortionValidRange, bool bIsLeft
	, float fPhiMin, float fPhiMax, float fThetaMin, float fThetaMax
	, const float* pK1, const float* pDist1, const float* pR10)
{
	tCoordinate2D ptSrcFliped(0.0f, 0.0f);
	if (!bIsLeft && (ptSrc.fY >= -M_PI && ptSrc.fY <= 0.0f))
	{
		//upside.
		ptSrcFliped = tCoordinate2D(ptSrc.fX + M_PI, ptSrc.fY + M_PI);
	}
	else if (bIsLeft && (ptSrc.fY > 0.0f && ptSrc.fY <= M_PI))
	{
		//downside.
		ptSrcFliped = tCoordinate2D(ptSrc.fX + M_PI, ptSrc.fY);
	}
	else
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	if (ptSrcFliped.fY < 0.2*M_PI || ptSrcFliped.fY > 0.8*M_PI)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0);
	}

	tCoordinate3D pSrc(0.0, 0.0, 0.0);
	pSrc.fX = sinf(ptSrcFliped.fY)*cosf(ptSrcFliped.fX);
	pSrc.fY = sinf(ptSrcFliped.fY)*sinf(ptSrcFliped.fX);
	pSrc.fZ = -cosf(ptSrcFliped.fY);


	tCoordinate3D pTemp = MatrixProjector(pSrc, pR10);

	if (pTemp.fZ <= 0.0f)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	//float fPhi = atan2f(pTemp.fX, pTemp.fZ);
	//float fTheta = atan2f(-pTemp.fY, pTemp.fZ);

	//float fDistPhi = (fPhi > (fPhiMin + fPhiMax) / 2.0) ? (fPhiMax - fPhi) : (fPhi - fPhiMin);
	//float fDistTheta = (fTheta > (fThetaMin + fThetaMax) / 2.0) ? (fThetaMax - fTheta) : (fTheta - fThetaMin);
	//float fRet = fminf(fDistPhi, fDistTheta);

	tCoordinate2D pRet(0.0f, 0.0f);
	if (eCameraTypeNormal == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeNormal>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheye == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheye>(pTemp, pK1, pDist1);
	}
	else if (eCameraTypeFisheyeAnglePoly5 == eCameratype)
	{
		pRet = Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(pTemp, pK1, pDist1);
	}

	//return (fRet < 0.0) ? cv::Point3f(FLT_MAX, FLT_MAX, 0.0) : cv::Point3f(pRet.fX, pRet.fY, fRet);


	float fXMin = tanf(fPhiMin);
	float fXMax = tanf(fPhiMax);
	float fYMin = tanf(fThetaMin);
	float fYMax = tanf(fThetaMax);
	float fXCenter = (fXMin + fXMax) / 2.0f;
	float fYCenter = (fYMin + fYMax) / 2.0f;

	pTemp.fX /= pTemp.fZ;
	pTemp.fY /= pTemp.fZ;

	float fDistX = (pTemp.fX > fXCenter) ? (fXMax - pTemp.fX) : (pTemp.fX - fXMin);
	float fDistY = (pTemp.fY > fYCenter) ? (fYMax - pTemp.fY) : (pTemp.fY - fYMin);

	float fRet = fminf(fDistX, fDistY);
	if (fRet < 0.0f)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	fRet = fminf(CalcWeight(pRet.fX, pRet.fY, pK1, fDistortionValidRange), powf(fRet, 5.0f));
	return tCoordinate3D(pRet.fX, pRet.fY, fRet);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D Original3d2NormalizedPix(const tCoordinate3D& ptSrc, int iNumOfCameras, int iIdxOfCamera)
{
	int iGridWidth = int(ceil(sqrt(iNumOfCameras)));

	float fStep = 1.0f / static_cast<float>(iGridWidth);

	//calculate camera index & calculate relative coordinate;
	float fXGrid = 0.0f;
	float fYGrid = 0.0f;
	float fXRel = std::modf((ptSrc.fX + 0.5f) / fStep, &fXGrid);
	float fYRel = std::modf((ptSrc.fY + 0.5f) / fStep, &fYGrid);

	int iCameraIdx = static_cast<int>(fYGrid)* iGridWidth + static_cast<int>(fXGrid);

	//check iIdxOfCamera ?= camera index.
	if (iIdxOfCamera != iCameraIdx)
	{
		return tCoordinate3D(FLT_MAX, FLT_MAX, 0.0f);
	}

	//to normalized pixel coordinate system.
	return tCoordinate3D(fXRel, fYRel, 1.0f);
}


G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D PanoPix2PanoGeometry(const tCoordinate2D& ptSrcPix, float fWidthPix2Geometry, float fHeightPix2Geometry
	, float fGeometricWidth, float fGeometricHeight)
{
	tCoordinate2D tRet(ptSrcPix.fX * fWidthPix2Geometry, ptSrcPix.fY * fHeightPix2Geometry);
	tRet.fX -= (fGeometricWidth * 0.5f);//The origin of the panoramic canvas is at the center of the panoramic canvas.
	tRet.fY -= (fGeometricHeight * 0.5f);
	return tRet;
}


#endif//G_RENDER_PROJECTORS_H_
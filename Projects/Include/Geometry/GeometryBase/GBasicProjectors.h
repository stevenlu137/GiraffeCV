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
 
 
 
#ifndef G_BASIC_PROJECTORS_H_
#define G_BASIC_PROJECTORS_H_

#include "Common/LanguageSwitch/GCPPCUDASwitch.h"
#include "Geometry/GeometryBase/GHomogeneousCoordinate.h"


G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D MatrixProjector(const tCoordinate3D& x, const float* pMatrix3D)
{
	return tCoordinate3D(
		x.fX*pMatrix3D[0] + x.fY*pMatrix3D[1] + x.fZ*pMatrix3D[2]
		, x.fX*pMatrix3D[3] + x.fY*pMatrix3D[4] + x.fZ*pMatrix3D[5]
		, x.fX*pMatrix3D[6] + x.fY*pMatrix3D[7] + x.fZ*pMatrix3D[8]);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D MatrixProjector4To3D(const tCoordinate4D& x, const float* pMatrix34D)
{
	return tCoordinate3D(
		x.fX*pMatrix34D[0] + x.fY*pMatrix34D[1] + x.fZ*pMatrix34D[2] + x.fW*pMatrix34D[3]
		, x.fX*pMatrix34D[4] + x.fY*pMatrix34D[5] + x.fZ*pMatrix34D[6] + x.fW*pMatrix34D[7]
		, x.fX*pMatrix34D[8] + x.fY*pMatrix34D[9] + x.fZ*pMatrix34D[10]+x.fW*pMatrix34D[11]);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D DistortionProjector(const tCoordinate2D& x, const float* pDistorsion)
{
	float r = sqrtf(x.fX*x.fX + x.fY*x.fY);
	float r2 = r*r;
	float r4 = r2*r2;
	float dilation = 1.0f + pDistorsion[0] * r2 + pDistorsion[1] * r4
		+ pDistorsion[4] * r2*r4;

	if (dilation > 1.5f || dilation < 0.6f)
	{
		return tCoordinate2D(FLT_MAX, FLT_MAX);
	}

	float offsetX = 2.0f * pDistorsion[2] * x.fX * x.fY + pDistorsion[3] * (r2 + 2.0f * x.fX * x.fX);
	float offsetY = pDistorsion[2] * (r2 + 2.0f * x.fY * x.fY) + 2.0f * pDistorsion[3] * x.fX * x.fY;

	return tCoordinate2D(
		x.fX * dilation + offsetX,
		x.fY * dilation + offsetY);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D UnDistortionProjector(const tCoordinate2D& x, const float* pDistorsion)
{
	tCoordinate2D pTrial(x.fX, x.fY);
	tCoordinate2D pN(0.0f, 0.0f);

	for (int i = 0; i < 10; ++i)
	{
		pN = DistortionProjector(pTrial, pDistorsion);
		pTrial.fX = pTrial.fX + (x.fX - pN.fX);
		pTrial.fY = pTrial.fY + (x.fY - pN.fY);
	}

	return pTrial;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D FisheyeDistortionProjector(const tCoordinate3D& x, const float* pDistorsion)
{
	float fR = sqrtf(x.fX*x.fX + x.fY*x.fY + x.fZ*x.fZ);
	float fTheta = acosf(x.fZ / fR);
	float fTheta2 = fTheta*fTheta;

	float fr = sqrtf(x.fX*x.fX + x.fY*x.fY);

	float fThetad = (0.0f == fr) ?
		1.0f
		: ((fTheta*(1 + fTheta2*(pDistorsion[0] + fTheta2*(pDistorsion[1] + fTheta2*(pDistorsion[2] + fTheta2*pDistorsion[3]))))) / fr);

	return tCoordinate2D(fThetad * x.fX, fThetad * x.fY);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D FisheyeUnDistortionProjector(const tCoordinate2D& x, const float* pDistorsion)
{
	auto CalcLambda = [pDistorsion](float fTheta)
	{
		float fTheta2 = fTheta*fTheta;
		return fTheta*(1 + fTheta2*(pDistorsion[0] + fTheta2*(pDistorsion[1] + fTheta2*(pDistorsion[2] + fTheta2*pDistorsion[3]))));
	};

	auto CalcDLambda = [pDistorsion](float fTheta)
	{
		float fTheta2 = fTheta*fTheta;
		return 1 + fTheta2*(3.0*pDistorsion[0] + fTheta2*(5.0*pDistorsion[1] + fTheta2*(7.0*pDistorsion[2] + fTheta2*9.0*pDistorsion[3])));
	};

	float fLambda = sqrtf(x.fX * x.fX + x.fY * x.fY);
	float fThetaN = (abs(pDistorsion[0]) < 1e-7) ? fLambda : (fLambda / pDistorsion[0]);
	float fLambdaN = 0.0f;

	for (int i = 0; i < 20; ++i)
	{
		fLambdaN = CalcLambda(fThetaN);
		fThetaN += (fLambda - fLambdaN) / CalcDLambda(fThetaN);
	}

	float fScale = sqrtf(x.fX*x.fX + x.fY*x.fY);
	fScale = (abs(fScale) < 1e-7) ? 0.0f : sinf(fThetaN) / fScale;
	float fX = x.fX * fScale;
	float fY = x.fY * fScale;
	float fZ = cosf(fThetaN);

	return tCoordinate3D(fX, fY, fZ);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D FisheyeAnglePoly5DistortionProjector(const tCoordinate3D& x, const float* pDistorsion)
{
	float fR = sqrtf(x.fX*x.fX + x.fY*x.fY + x.fZ*x.fZ);
	float fTheta = acosf(x.fZ / fR);
	float fTheta2 = fTheta*fTheta;

	float fr = sqrtf(x.fX*x.fX + x.fY*x.fY);

	float fThetad = (0.0f == fr) ?
		1.0f
		: ((fTheta*(pDistorsion[0] + fTheta2*(pDistorsion[1] + fTheta2*(pDistorsion[2] + fTheta2*(pDistorsion[3] + fTheta2*pDistorsion[4]))))) / fr);

	return tCoordinate2D(fThetad * x.fX, fThetad * x.fY);
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D FisheyeAnglePoly5UnDistortionProjector(const tCoordinate2D& x, const float* pDistorsion)
{
	auto CalcLambda = [pDistorsion](float fTheta)
	{
		float fTheta2 = fTheta*fTheta;
		return fTheta*(pDistorsion[0] + fTheta2*(pDistorsion[1] + fTheta2*(pDistorsion[2] + fTheta2*(pDistorsion[3] + fTheta2*pDistorsion[4]))));
	};

	auto CalcDLambda = [pDistorsion](float fTheta)
	{
		float fTheta2 = fTheta*fTheta;
		return pDistorsion[0] + fTheta2*(3.0*pDistorsion[1] + fTheta2*(5.0*pDistorsion[2] + fTheta2*(7.0*pDistorsion[3] + fTheta2*9.0*pDistorsion[4])));
	};

	float fLambda = sqrtf(x.fX * x.fX + x.fY * x.fY);
	float fThetaN = (abs(pDistorsion[0]) < 1e-7) ? fLambda : (fLambda / pDistorsion[0]);
	float fLambdaN = 0.0f;

	for (int i = 0; i < 20; ++i)
	{
		fLambdaN = CalcLambda(fThetaN);
		fThetaN += (fLambda - fLambdaN) / CalcDLambda(fThetaN);
	}

	float fScale = sqrtf(x.fX*x.fX + x.fY*x.fY);
	fScale = (abs(fScale) < 1e-7) ? 0.0f : sinf(fThetaN) / fScale;
	float fX = x.fX * fScale;
	float fY = x.fY * fScale;
	float fZ = cosf(fThetaN);

	//float fPhi = atan2f(x.fY, x.fX);
	//float fX = sinf(fThetaN)*cosf(fPhi);
	//float fY = sinf(fThetaN)*sinf(fPhi);
	//float fZ = cosf(fThetaN);

	return tCoordinate3D(fX, fY, fZ);
}

#endif //G_BASIC_PROJECTORS_H_
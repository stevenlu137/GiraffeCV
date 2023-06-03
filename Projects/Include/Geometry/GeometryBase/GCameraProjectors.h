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
 
 
 
#ifndef G_CAMERA_PROJECTORS_H_
#define G_CAMERA_PROJECTORS_H_

#include "Common/LanguageSwitch/GCPPCUDASwitch.h"
#include "Geometry/GeometryBase/GHomogeneousCoordinate.h"
#include "Geometry/GeometryBase/GBasicProjectors.h"

template<int iCameraType>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D Ray2DistortedPixProjector(const tCoordinate3D& ray
	, const float* pK0, const float* pDistorsion);

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D Ray2DistortedPixProjector<eCameraTypeNormal>(const tCoordinate3D& ray
	, const float* pK0, const float* pDistorsion)
{
	return MatrixProjector(DistortionProjector(ray, pDistorsion), pK0);
}

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D Ray2DistortedPixProjector<eCameraTypeFisheye>(const tCoordinate3D& ray
	, const float* pK0, const float* pDistorsion)
{
	return MatrixProjector(FisheyeDistortionProjector(ray, pDistorsion), pK0);
}

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D Ray2DistortedPixProjector<eCameraTypeFisheyeAnglePoly5>(const tCoordinate3D& ray
	, const float* pK0, const float* pDistorsion)
{
	return MatrixProjector(FisheyeAnglePoly5DistortionProjector(ray, pDistorsion), pK0);
}

template<int iCameraType>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D DistortedPix2RayProjector(const tCoordinate2D& x
	, const float* pDistorsion, const float* pK0Inv);

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D DistortedPix2RayProjector<eCameraTypeNormal>(const tCoordinate2D& x
	, const float* pDistorsion, const float* pK0Inv)
{
	return UnDistortionProjector(MatrixProjector(x, pK0Inv), pDistorsion);
}

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D DistortedPix2RayProjector<eCameraTypeFisheye>(const tCoordinate2D& x
	, const float* pDistorsion, const float* pK0Inv)
{
	return FisheyeUnDistortionProjector(MatrixProjector(x, pK0Inv), pDistorsion);
}

template<>
G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D DistortedPix2RayProjector<eCameraTypeFisheyeAnglePoly5>(const tCoordinate2D& x
	, const float* pDistorsion, const float* pK0Inv)
{
	return FisheyeAnglePoly5UnDistortionProjector(MatrixProjector(x, pK0Inv), pDistorsion);
}

#endif //G_CAMERA_PROJECTORS_H_
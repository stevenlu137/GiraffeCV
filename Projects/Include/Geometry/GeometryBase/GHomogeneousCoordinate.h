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
 
 
 
#ifndef G_HOMOGENEOUS_COORDINATE_H_
#define G_HOMOGENEOUS_COORDINATE_H_

#include "Common/LanguageSwitch/GCPPCUDASwitch.h"

struct tCoordinate3D;
struct tCoordinate4D;

struct tCoordinate2D
{
	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D(float x, float y) :fX(x)
	, fY(y)
	{}

	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D(const tCoordinate3D& x);

	float fX;
	float fY;
};

struct tCoordinate3D
{
	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D(float x, float y, float z) : fX(x)
	, fY(y)
	, fZ(z)
	{}

	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D(tCoordinate2D x);
	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D(tCoordinate4D x);

	float fX;
	float fY;
	float fZ;
};

struct tCoordinate4D
{
	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate4D(float x, float y, float z, float w) : fX(x)
	, fY(y)
	, fZ(z)
	, fW(w)
	{}

	G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate4D(tCoordinate3D x);

	float fX;
	float fY;
	float fZ;
	float fW;
};

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate2D::tCoordinate2D(const tCoordinate3D& x)
{
	fX = x.fX / x.fZ;
	fY = x.fY / x.fZ;//todo.
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D::tCoordinate3D(tCoordinate2D x)
{
	fX = x.fX;
	fY = x.fY;
	fZ = 1.0f;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate3D::tCoordinate3D(tCoordinate4D x)
{
	fX = x.fX / x.fW;
	fY = x.fY / x.fW;
	fZ = x.fZ / x.fW;
}

G_CUDA_HOST_DEVICE_FORCEINLINE tCoordinate4D::tCoordinate4D(tCoordinate3D x)
{
	fX = x.fX;
	fY = x.fY;
	fZ = x.fZ;
	fW = 1.0f;
}
#endif //G_HOMOGENEOUS_COORDINATE_H_
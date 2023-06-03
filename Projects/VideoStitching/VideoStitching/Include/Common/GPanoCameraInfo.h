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
 
 
 
#ifndef G_PANO_CAMERA_INFO_H_
#define G_PANO_CAMERA_INFO_H_

#include <vector>
#include <map>

#include "opencv2/core/core.hpp"

#include "Common/Camera/GCameraType.h"


enum enPanoType
{
	ePanoTypeOriginal = 0,

	ePanoTypeImmersionSemiSphere = 1,
	ePanoTypeImmersionFullSphere = 10,
	ePanoTypeImmersionFullSphereInside = 11,
	ePanoTypeImmersionCylinder360 = 20,
	ePanoTypeImmersionCylinder360Inside = 21,

	ePanoTypeUnwrappedSphere180 = 500,
	ePanoTypeUnwrappedSphere360 = 501,
	ePanoTypeUnwrappedSphere360Flip = 502,
	
	ePanoTypeUnwrappedFullSphere360 = 510,
	ePanoTypeUnwrappedFullSphere360Flip = 512,
	ePanoTypeUnwrappedFullSphere360Binocular = 519,

	ePanoTypeUnwrappedCylinder180 = 520,
	ePanoTypeUnwrappedCylinder360 = 521,
	ePanoTypeUnwrappedCylinderSplited = 522,
	ePanoTypeUnwrappedCylinder180Flip = 523,
	ePanoTypeUnwrappedCylinder360Flip = 524,
	ePanoTypeUnwrappedCylinderSplitedFrontBack = 525,

	ePanoTypeMaxNum = 1024,
};

struct GPanoTypeParas
{
	enPanoType ePanoType = ePanoTypeOriginal;
	float fTopCropRatio = 0.0;
	float fBottomCropRatio = 0.0;
	float fLeftCropRatio = 0.0;
	float fRightCropRatio = 0.0;
};

struct GCameraPanoMapKDM
{
	std::map<std::string, cv::Mat> mpMultiK0Map;
	cv::Mat mDist;
	cv::Mat mM;
	double dDistortionValidRange;
};

struct GCameraModel4Profile
{
	enCameraType eCameraType;
	std::string sCameraName;

	GCameraPanoMapKDM cameraPanoMap;
};

struct GPanoCameraProfile
{
	std::vector<GCameraModel4Profile> vCameraModels;
	std::vector<GPanoTypeParas> vPanoTypeList;
};

#endif //G_PANO_CAMERA_INFO_H_
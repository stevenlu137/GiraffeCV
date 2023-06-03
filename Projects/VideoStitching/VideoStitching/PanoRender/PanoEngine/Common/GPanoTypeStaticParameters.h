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
 
 
 
#ifndef G_PANO_TYPE_STATIC_PARAMETERS_H_
#define G_PANO_TYPE_STATIC_PARAMETERS_H_

#include "opencv2/core/core.hpp"

#include "Common/Exception/GiraffeException.h"
#include "Common/Math/GMathConstants.h"
#include "GPanoCameraInfo.h"

enum enViewCameraType
{
	eViewCameraTypePerspectiveOutside,
	eViewCameraTypePerspectiveInside,
	eViewCameraTypeOrthogonal,
	eViewCameraTypePerspectiveOutsideSurround,
	eViewCameraTypePerspectiveInsideSurround,
};

class GPanoTypeStaticParameters
{
public:
	static enViewCameraType GetDefaultViewCameraType(enPanoType ePanoType)
	{
		switch (ePanoType)
		{
		case ePanoTypeImmersionSemiSphere:
			return eViewCameraTypePerspectiveOutside;
			break;
		case ePanoTypeUnwrappedCylinder180:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedCylinder360:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeImmersionCylinder360:
			return eViewCameraTypePerspectiveOutsideSurround;
			break;
		case ePanoTypeImmersionCylinder360Inside:
			return eViewCameraTypePerspectiveInsideSurround;
			break;
		case ePanoTypeUnwrappedCylinderSplited:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedCylinderSplitedFrontBack:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedSphere180:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedSphere360:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedSphere360Flip:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedCylinder180Flip:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedCylinder360Flip:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeImmersionFullSphere:
			return eViewCameraTypePerspectiveOutsideSurround;
			break;
		case ePanoTypeUnwrappedFullSphere360:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedFullSphere360Flip:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeUnwrappedFullSphere360Binocular:
			return eViewCameraTypeOrthogonal;
			break;
		case ePanoTypeImmersionFullSphereInside:
			return eViewCameraTypePerspectiveOutsideSurround;
			break;
		case ePanoTypeOriginal:
			return eViewCameraTypeOrthogonal;
			break;
		default:
			throw GPanoRenderException();
			break;
		}
	}

	static float GetDefaultCanvasWidth(enPanoType ePanoType)
	{
		switch (ePanoType)
		{
		case ePanoTypeImmersionSemiSphere:
			break;
		case ePanoTypeUnwrappedCylinder180:
			return 1.0 * M_PI;
			break;
		case ePanoTypeUnwrappedCylinder360:
			return 2.0 * M_PI;
			break;
		case ePanoTypeImmersionCylinder360:
			break;
		case ePanoTypeImmersionCylinder360Inside:
			break;
		case ePanoTypeUnwrappedCylinderSplited:
			return M_PI;
			break;
		case ePanoTypeUnwrappedCylinderSplitedFrontBack:
			return M_PI;
			break;
		case ePanoTypeUnwrappedSphere180:
			return M_PI;
			break;
		case ePanoTypeUnwrappedSphere360:
			return 2.0 * M_PI;
			break;
		case ePanoTypeUnwrappedSphere360Flip:
			return 2.0 * M_PI;
			break;
		case ePanoTypeUnwrappedCylinder180Flip:
			return 1.0 * M_PI;
			break;
		case ePanoTypeUnwrappedCylinder360Flip:
			return 2.0 * M_PI;
			break;
		case ePanoTypeImmersionFullSphere:
			break;
		case ePanoTypeUnwrappedFullSphere360:
			return 2.0 * M_PI;
			break;
		case ePanoTypeUnwrappedFullSphere360Flip:
			return 2.0 * M_PI;
			break;
		case ePanoTypeUnwrappedFullSphere360Binocular:
			return 2.0 * M_PI;
			break;
		case ePanoTypeImmersionFullSphereInside:
			break;
		case ePanoTypeOriginal:
			return 1.0;
			break;
		default:
			break;
		}
		return 0.0;
	}

	static float GetDefaultCanvasHeight(enPanoType ePanoType)
	{
		switch (ePanoType)
		{
		case ePanoTypeImmersionSemiSphere:
			break;
		case ePanoTypeUnwrappedCylinder180:
			return 1.5;
			break;
		case ePanoTypeUnwrappedCylinder360:
			return 2.0;
			break;
		case ePanoTypeImmersionCylinder360:
			return 2.0;
			break;
		case ePanoTypeImmersionCylinder360Inside:
			return 2.0;
			break;
		case ePanoTypeUnwrappedCylinderSplited:
			return 2.0;
			break;
		case ePanoTypeUnwrappedCylinderSplitedFrontBack:
			return 2.0;
			break;
		case ePanoTypeUnwrappedSphere180:
			return M_PI;
			break;
		case ePanoTypeUnwrappedSphere360:
			return M_PI / 2.0;
			break;
		case ePanoTypeUnwrappedSphere360Flip:
			return M_PI / 2.0;
			break;
		case ePanoTypeUnwrappedCylinder180Flip:
			return 1.5;
			break;
		case ePanoTypeUnwrappedCylinder360Flip:
			return 2.0;
			break;
		case ePanoTypeImmersionFullSphere:
			break;
		case ePanoTypeUnwrappedFullSphere360:
			return M_PI;
			break;
		case ePanoTypeUnwrappedFullSphere360Flip:
			return M_PI;
			break;
		case ePanoTypeUnwrappedFullSphere360Binocular:
			return 2.0 * M_PI;
			break;
		case ePanoTypeImmersionFullSphereInside:
			break;
		case ePanoTypeOriginal:
			return 1.0;
			break;
		default:
			break;
		}
		return 0.0;
	}

	static cv::Rect_<float> GetPanoRectOnCanvas(enPanoType ePanoType, const Eigen::Vector4f& v4fCropRatios)
	{
		//v4fCropRatios = {left,right,top,down}.
		float fTLX = GetDefaultCanvasWidth(ePanoType)*
			(v4fCropRatios[0] - 0.5);
		float fTLY = GetDefaultCanvasHeight(ePanoType)*
			(v4fCropRatios[2] - 0.5);
		float fBRX = GetDefaultCanvasWidth(ePanoType)*
			(0.5 - v4fCropRatios[1]);
		float fBRY = GetDefaultCanvasHeight(ePanoType)*
			(0.5 - v4fCropRatios[3]);
		return cv::Rect_<float>(cv::Point2f(fTLX, fTLY), cv::Point2f(fBRX, fBRY));
	}

	static Eigen::Vector2f GetPanoHeightOnCanvas(enPanoType ePanoType, const Eigen::Vector4f& v4fCropRatios)
	{
		//v4fCropRatios = {left,right,top,down}.
		Eigen::Vector2f v2fHeight;
		v2fHeight[0] = GetDefaultCanvasHeight(ePanoType)*
			(v4fCropRatios[2] - 0.5);
		v2fHeight[1] = GetDefaultCanvasHeight(ePanoType)*
			(0.5 - v4fCropRatios[3]);
		return v2fHeight;
	}
};



#endif //G_PANO_TYPE_STATIC_PARAMETERS_H_

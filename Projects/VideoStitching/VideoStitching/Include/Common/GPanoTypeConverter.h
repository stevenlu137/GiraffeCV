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
 
 
 
#ifndef G_PANO_TYPE_CONVERTER_H_
#define G_PANO_TYPE_CONVERTER_H_

#include <iostream>
#include <string>

#include "PanoRender/GPanoRenderOGLIF.h"
#include "GPanoCameraInfo.h"


namespace GPanoTypeConverter
{
    static enPanoType ConvertString2PanoType(const std::string& sPanoType)
    {
        if ("ImmersionSemiSphere" == sPanoType)
        {
            return ePanoTypeImmersionSemiSphere;
        }
        else if ("UnwrappedCylinder180" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinder180;
        }
        else if ("UnwrappedCylinder360" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinder360;
        }
        else if ("ImmersionCylinder360" == sPanoType)
        {
            return ePanoTypeImmersionCylinder360;
        }
        else if ("ImmersionCylinder360Inside" == sPanoType)
        {
            return ePanoTypeImmersionCylinder360Inside;
        }
        else if ("UnwrappedCylinderSplited" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinderSplited;
        }
        else if ("UnwrappedCylinderSplitedFrontBack" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinderSplitedFrontBack;
        }
        else if ("UnwrappedSphere180" == sPanoType)
        {
            return ePanoTypeUnwrappedSphere180;
        }
        else if ("UnwrappedSphere360" == sPanoType)
        {
            return ePanoTypeUnwrappedSphere360;
        }
        else if ("UnwrappedSphere360Flip" == sPanoType)
        {
            return ePanoTypeUnwrappedSphere360Flip;
        }
        else if ("UnwrappedCylinder180Flip" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinder180Flip;
        }
        else if ("UnwrappedCylinder360Flip" == sPanoType)
        {
            return ePanoTypeUnwrappedCylinder360Flip;
        }
        else if ("ImmersionFullSphere" == sPanoType)
        {
            return ePanoTypeImmersionFullSphere;
        }
        else if ("UnwrappedFullSphere360" == sPanoType)
        {
            return ePanoTypeUnwrappedFullSphere360;
        }
        else if ("UnwrappedFullSphere360Flip" == sPanoType)
        {
            return ePanoTypeUnwrappedFullSphere360Flip;
        }
        else if ("UnwrappedFullSphere360Binocular" == sPanoType)
        {
            return ePanoTypeUnwrappedFullSphere360Binocular;
        }
        else if ("ImmersionFullSphereInside" == sPanoType)
        {
            return ePanoTypeImmersionFullSphereInside;
        }
        else if ("Original" == sPanoType)
        {
            return ePanoTypeOriginal;
        }
        else
        {
            std::cout << "Pano type not supported." << std::endl;
            return ePanoTypeMaxNum;
        }
    }

    static std::string ConvertPanoType2String(enPanoType ePanoType)
    {
        if (ePanoTypeImmersionSemiSphere == ePanoType)
        {
            return "ImmersionSemiSphere";
        }
        else if (ePanoTypeUnwrappedCylinder180 == ePanoType)
        {
            return "UnwrappedCylinder180";
        }
        else if (ePanoTypeUnwrappedCylinder360 == ePanoType)
        {
            return "UnwrappedCylinder360";
        }
        else if (ePanoTypeImmersionCylinder360 == ePanoType)
        {
            return "ImmersionCylinder360";
        }
        else if (ePanoTypeImmersionCylinder360Inside == ePanoType)
        {
            return "ImmersionCylinder360Inside";
        }
        else if (ePanoTypeUnwrappedCylinderSplited == ePanoType)
        {
            return "UnwrappedCylinderSplited";
        }
        else if (ePanoTypeUnwrappedCylinderSplitedFrontBack == ePanoType)
        {
            return "UnwrappedCylinderSplitedFrontBack";
        }
        else if (ePanoTypeUnwrappedSphere180 == ePanoType)
        {
            return "UnwrappedSphere180";
        }
        else if (ePanoTypeUnwrappedSphere360 == ePanoType)
        {
            return "UnwrappedSphere360";
        }
        else if (ePanoTypeUnwrappedSphere360Flip == ePanoType)
        {
            return "UnwrappedSphere360Flip";
        }
        else if (ePanoTypeUnwrappedCylinder180Flip == ePanoType)
        {
            return "UnwrappedCylinder180Flip";
        }
        else if (ePanoTypeUnwrappedCylinder360Flip == ePanoType)
        {
            return "UnwrappedCylinder360Flip";
        }
        else if (ePanoTypeImmersionFullSphere == ePanoType)
        {
            return "ImmersionFullSphere";
        }
        else if (ePanoTypeUnwrappedFullSphere360 == ePanoType)
        {
            return "UnwrappedFullSphere360";
        }
        else if (ePanoTypeUnwrappedFullSphere360Flip == ePanoType)
        {
            return "UnwrappedFullSphere360Flip";
        }
        else if (ePanoTypeUnwrappedFullSphere360Binocular == ePanoType)
        {
            return "UnwrappedFullSphere360Binocular";
        }
        else if (ePanoTypeImmersionFullSphereInside == ePanoType)
        {
            return "ImmersionFullSphereInside";
        }
        else if (ePanoTypeOriginal == ePanoType)
        {
            return "Original";
        }
        else
        {
            std::cout << "Pano type not supported." << std::endl;
            return "";
        }
    }

    static enPanoType ConvertGPanoType2PanoType(G_enPanoType eGPanoType)
    {
		switch (eGPanoType)
		{
		case G_ePanoTypeImmersionSemiSphere:
			return ePanoTypeImmersionSemiSphere;
			break;
		case G_ePanoTypeUnwrappedCylinder180:
			return ePanoTypeUnwrappedCylinder180;
			break;
		case G_ePanoTypeUnwrappedCylinder360:
			return ePanoTypeUnwrappedCylinder360;
			break;
		case G_ePanoTypeImmersionCylinder360:
			return ePanoTypeImmersionCylinder360;
			break;
		case G_ePanoTypeImmersionCylinder360Inside:
			return ePanoTypeImmersionCylinder360Inside;
			break;
		case G_ePanoTypeUnwrappedCylinderSplited:
			return ePanoTypeUnwrappedCylinderSplited;
			break;
		case G_ePanoTypeUnwrappedCylinderSplitedFrontBack:
			return ePanoTypeUnwrappedCylinderSplitedFrontBack;
			break;
		case G_ePanoTypeUnwrappedSphere180:
			return ePanoTypeUnwrappedSphere180;
			break;
		case  G_ePanoTypeUnwrappedSphere360:
			return ePanoTypeUnwrappedSphere360;
			break;
		case  G_ePanoTypeUnwrappedSphere360Flip:
			return ePanoTypeUnwrappedSphere360Flip;
			break;
		case  G_ePanoTypeUnwrappedCylinder180Flip:
			return ePanoTypeUnwrappedCylinder180Flip;
			break;
		case  G_ePanoTypeUnwrappedCylinder360Flip:
			return ePanoTypeUnwrappedCylinder360Flip;
			break;
		case G_ePanoTypeImmersionFullSphere:
			return ePanoTypeImmersionFullSphere;
			break;
		case G_ePanoTypeUnwrappedFullSphere360:
			return ePanoTypeUnwrappedFullSphere360;
			break;
		case G_ePanoTypeUnwrappedFullSphere360Flip:
			return ePanoTypeUnwrappedFullSphere360Flip;
			break;
		case G_ePanoTypeUnwrappedFullSphere360Binocular:
			return ePanoTypeUnwrappedFullSphere360Binocular;
			break;
		case G_ePanoTypeImmersionFullSphereInside:
			return ePanoTypeImmersionFullSphereInside;
			break;
		case G_ePanoTypeOriginal:
			return ePanoTypeOriginal;
			break;
		default:
			return ePanoTypeMaxNum;
			break;
		}
    }

    static G_enPanoType ConvertPanoType2GPanoType(enPanoType ePanoType)
    {
        switch (ePanoType)
        {
        case ePanoTypeImmersionSemiSphere:
            return  G_ePanoTypeImmersionSemiSphere;
            break;
        case ePanoTypeUnwrappedCylinder180:
            return  G_ePanoTypeUnwrappedCylinder180;
            break;
        case ePanoTypeUnwrappedCylinder360:
            return  G_ePanoTypeUnwrappedCylinder360;
            break;
        case ePanoTypeImmersionCylinder360:
            return  G_ePanoTypeImmersionCylinder360;
            break;
        case ePanoTypeImmersionCylinder360Inside:
            return  G_ePanoTypeImmersionCylinder360Inside;
            break;
        case ePanoTypeUnwrappedCylinderSplited:
            return  G_ePanoTypeUnwrappedCylinderSplited;
            break;
        case ePanoTypeUnwrappedCylinderSplitedFrontBack:
            return  G_ePanoTypeUnwrappedCylinderSplitedFrontBack;
            break;
        case ePanoTypeUnwrappedSphere180:
            return  G_ePanoTypeUnwrappedSphere180;
            break;
        case ePanoTypeUnwrappedSphere360:
            return  G_ePanoTypeUnwrappedSphere360;
            break;
        case ePanoTypeUnwrappedSphere360Flip:
            return  G_ePanoTypeUnwrappedSphere360Flip;
            break;
        case ePanoTypeUnwrappedCylinder180Flip:
            return  G_ePanoTypeUnwrappedCylinder180Flip;
            break;
        case ePanoTypeUnwrappedCylinder360Flip:
            return  G_ePanoTypeUnwrappedCylinder360Flip;
            break;
        case ePanoTypeImmersionFullSphere:
            return  G_ePanoTypeImmersionFullSphere;
            break;
        case ePanoTypeUnwrappedFullSphere360:
            return  G_ePanoTypeUnwrappedFullSphere360;
            break;
        case ePanoTypeUnwrappedFullSphere360Flip:
            return  G_ePanoTypeUnwrappedFullSphere360Flip;
            break;
        case ePanoTypeUnwrappedFullSphere360Binocular:
            return  G_ePanoTypeUnwrappedFullSphere360Binocular;
            break;
        case ePanoTypeImmersionFullSphereInside:
            return  G_ePanoTypeImmersionFullSphereInside;
            break;
        case ePanoTypeOriginal:
            return  G_ePanoTypeOriginal;
            break;
        default:
            return G_ePanoTypeMaxNum;
            break;
        }
    }


};

#endif //G_PANO_TYPE_CONVERTER_H_
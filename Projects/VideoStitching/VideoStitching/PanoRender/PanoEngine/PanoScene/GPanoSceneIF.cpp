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
 
 
 
#include "PanoRender/GPanoSceneIF.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "GPanoScene.h"


std::shared_ptr<GPanoSceneIF> GPanoSceneFactory::CreatePanoScene(enPanoType ePanoType)
{
	GLOGGER(info) << "==> GPanoSceneFactory::CreatePanoScene()";

	std::shared_ptr<GPanoSceneIF> ptrRet(nullptr);

	if (ePanoTypeImmersionSemiSphere == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneImmersionSemiSphere());
	}
	else if (ePanoTypeUnwrappedCylinder180 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinder180());
	}
	else if (ePanoTypeUnwrappedCylinder360 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinder360());
	}
	else if (ePanoTypeImmersionCylinder360 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneImmersionCylinder360());
	}
	else if (ePanoTypeImmersionCylinder360Inside == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneImmersionCylinder360());
	}
	else if (ePanoTypeUnwrappedCylinderSplited == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinderSplited());
	}
	else if (ePanoTypeUnwrappedCylinderSplitedFrontBack == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinderSplitedFrontBack());
	}
	else if (ePanoTypeUnwrappedSphere180 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedSphere180());
	}
	else if (ePanoTypeUnwrappedSphere360 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedSphere360());
	}
	else if (ePanoTypeUnwrappedSphere360Flip == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedSphere360Flip());
	}
	else if (ePanoTypeUnwrappedCylinder180Flip == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinder180Flip());
	}
	else if (ePanoTypeUnwrappedCylinder360Flip == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedCylinder360Flip());
	}
	else if (ePanoTypeImmersionFullSphere == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneImmersionFullSphere());
	}
	else if (ePanoTypeUnwrappedFullSphere360 == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedFullSphere360());
	}
	else if (ePanoTypeUnwrappedFullSphere360Flip == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedFullSphere360Flip());
	}
	else if (ePanoTypeUnwrappedFullSphere360Binocular == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneUnwrappedFullSphere360Binocular());
	}
	else if (ePanoTypeImmersionFullSphereInside == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneImmersionFullSphere());
	}
	else if (ePanoTypeOriginal == ePanoType)
	{
		ptrRet = std::shared_ptr<GPanoSceneIF>(new GPanoSceneOriginal());
	}
	else
	{
		GLOGGER(error) << "GPanoSceneFactory::CreatePanoScene(). pano type not implemented now!";
		GLOGGER(info) << "GPanoSceneFactory::CreatePanoScene() ==>";
	}

	GLOGGER(info) << "GPanoSceneFactory::CreatePanoScene() ==>";
	return ptrRet;
}

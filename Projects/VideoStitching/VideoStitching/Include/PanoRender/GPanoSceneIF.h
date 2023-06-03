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
 
 
 
#ifndef G_PANO_SCENE_IF_H_
#define G_PANO_SCENE_IF_H_

#include <memory>
#include <Eigen/Dense>

#include "glm/glm.hpp"

#include "Common/DllExport/GDllExport.h"
#include "Common/GPanoCameraInfo.h"
#include "GPanoRenderCommonIF.h"
#include "GCameraModel.h"


class GPanoSceneIF
{
public:
	virtual ~GPanoSceneIF(){};

	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios) = 0;
	virtual bool Prepare() = 0;
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV) = 0;
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPoint) const = 0;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPoint, Eigen::Vector3f& v3fPhysical3DPoint) const = 0;
	virtual void Release() = 0;
};

class GPanoSceneFactory
{
public:
	static std::shared_ptr<GPanoSceneIF> CreatePanoScene(enPanoType ePanoType);

};

#endif //G_PANO_SCENE_IF_H_
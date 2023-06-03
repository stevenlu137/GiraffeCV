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
 
 
 
#ifndef G_PANO_SCENE_H_
#define G_PANO_SCENE_H_

#include "PanoRender/GPanoSceneIF.h"

class GRenderScene;

class GPanoSceneImmersionSemiSphere : public GPanoSceneIF
{
public:
	GPanoSceneImmersionSemiSphere():
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneImmersionSemiSphere(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinder180 : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinder180() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinder180(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinder360 : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinder360() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinder360(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneImmersionCylinder360 : public GPanoSceneIF
{
public:
	GPanoSceneImmersionCylinder360() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneImmersionCylinder360(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinderSplited : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinderSplited() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinderSplited(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinderSplitedFrontBack : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinderSplitedFrontBack() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinderSplitedFrontBack(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedSphere180 : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedSphere180() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedSphere180(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedSphere360 : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedSphere360() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedSphere360(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedSphere360Flip : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedSphere360Flip() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedSphere360Flip(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinder180Flip : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinder180Flip() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinder180Flip(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedCylinder360Flip : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedCylinder360Flip() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedCylinder360Flip(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneImmersionFullSphere : public GPanoSceneIF
{
public:
	GPanoSceneImmersionFullSphere() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneImmersionFullSphere(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedFullSphere360 : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedFullSphere360() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedFullSphere360(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedFullSphere360Flip : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedFullSphere360Flip() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedFullSphere360Flip(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneUnwrappedFullSphere360Binocular : public GPanoSceneIF
{
public:
	GPanoSceneUnwrappedFullSphere360Binocular() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneUnwrappedFullSphere360Binocular(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

class GPanoSceneOriginal : public GPanoSceneIF
{
public:
	GPanoSceneOriginal() :
		m_ptrRenderScene(std::shared_ptr<GRenderScene>(nullptr))
	{}

	virtual ~GPanoSceneOriginal(){};
	virtual bool Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);
	virtual bool Prepare();
	virtual bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	virtual bool Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const;
	virtual bool UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const;
	virtual void Release();
private:
	GStitcherConfig m_config;
	std::shared_ptr<GRenderScene> m_ptrRenderScene;
};

#endif // !G_PANO_SCENE_H_

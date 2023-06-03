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
 
 
 
#include "GPanoScene.h"
#include "GRenderParaManager.h"
#include "GSurfaceGenerator.h"
#include "GProjectorObjects.h"
#include "GRenderScene.h"
#include "GPanoTypeStaticParameters.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "Common/Math/GMathConstants.h"


bool GetK0(int iCameraIdx, const std::string& sK0Name, const std::vector<CameraModel>& vCameras, cv::Mat& mK0)
{
	if (vCameras[iCameraIdx].m_mpMultiK0Map.end()
		== vCameras[iCameraIdx].m_mpMultiK0Map.find(sK0Name))
	{
		GLOGGER(error) << "GetK0(). input frame size not supported in this panoCamProfile.bin. K0 Name: "
			<< sK0Name;
		return false;
	}

	mK0 = vCameras[iCameraIdx].m_mpMultiK0Map.at(sK0Name);
	return true;
}

bool GPanoSceneImmersionSemiSphere::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorSphere> ptrSphereSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorSphere>(new GSurfaceGeneratorSphere);
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrSphereSurfaceGenerator;

		std::vector<GProjectorObjectGlobal3d2NormalizedPix> vProjectorObjs(vCameras.size());
		std::vector<float> vDistortionValidRange(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);
			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::Create(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionSemiSphere::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionSemiSphere::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::Render(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionSemiSphere::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		v3fScene3DPointf = v3fPhysical3DPoint;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::Project(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionSemiSphere::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		v3fPhysical3DPoint = v3fScene3DPointf;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneImmersionSemiSphere::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionSemiSphere::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinder180::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinder180, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinder180), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinder180));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetFlip(false);
			vProjectorObjs[iProjectorObjIdx].SetTheta(0.0);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2CylinderExpandedPlane3d projector;
		projector.SetFlip(false);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorCylinderExpandedPlane3d2PhysicalDirection projector;
		projector.SetFlip(false);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinder180::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinder360::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinder360, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinder360), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinder360));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetFlip(false);
			vProjectorObjs[iProjectorObjIdx].SetTheta(0.0);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2CylinderExpandedPlane3d projector;
		projector.SetFlip(false);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorCylinderExpandedPlane3d2PhysicalDirection projector;
		projector.SetFlip(false);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinder360::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360::Release(): exception.";
		return;
	}
}

bool GPanoSceneImmersionCylinder360::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		Eigen::Vector2f panoHeight = GPanoTypeStaticParameters::GetPanoHeightOnCanvas(ePanoTypeImmersionCylinder360, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoHeight](float fX, float fY, float fZ)
		{
			return ((fY >= (panoHeight[0] - FLT_EPSILON)) && (fY <= (panoHeight[1] + FLT_EPSILON)));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorCylinder> ptrCylinderSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorCylinder>(new GSurfaceGeneratorCylinder);
		ptrCylinderSurfaceGenerator->SetSurfaceSize(GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeImmersionCylinder360));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrCylinderSurfaceGenerator;

		std::vector<GProjectorObjectGlobal3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);
			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::Create(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionCylinder360::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionCylinder360::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::Render(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionCylinder360::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		v3fScene3DPointf = v3fPhysical3DPoint;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::Project(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionCylinder360::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		v3fPhysical3DPoint = v3fScene3DPointf;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneImmersionCylinder360::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionCylinder360::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinderSplited::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinderSplited, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinderSplited), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinderSplited));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplited::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplited::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplited::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplited::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinderSplited::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplited::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinderSplitedFrontBack::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinderSplitedFrontBack, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinderSplitedFrontBack), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinderSplitedFrontBack));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplitedFrontBack::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplitedFrontBack::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplitedFrontBack::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2CylinderExpandedPlaneSplitedFrontBack3d projector;

		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinderSplitedFrontBack::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorCylinderExpandedPlaneSplitedFrontBack3d2PhysicalDirection projector;

		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinderSplitedFrontBack::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinderSplitedFrontBack::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedSphere180::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedSphere180, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedSphere180), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedSphere180));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix180> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere180::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere180::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere180::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2SphereExpandedPlane3d180 projector;
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere180::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorSphereExpandedPlane3d2PhysicalDirection180 projector;
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedSphere180::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere180::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedSphere360::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedSphere360, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedSphere360), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedSphere360));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);

			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);

			vProjectorObjs[iProjectorObjIdx].SetFlip(false);

			vProjectorObjs[iProjectorObjIdx].SetIsFullSphere(false);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2SphereExpandedPlane3d360 projector;
		projector.SetFlip(false);
		projector.SetIsFullSphere(false);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorSphereExpandedPlane3d2PhysicalDirection360 projector;
		projector.SetFlip(false);
		projector.SetIsFullSphere(false);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedSphere360::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedSphere360Flip::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedSphere360Flip, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedSphere360Flip), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedSphere360Flip));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);

			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);

			vProjectorObjs[iProjectorObjIdx].SetFlip(true);

			vProjectorObjs[iProjectorObjIdx].SetIsFullSphere(false);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360Flip::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360Flip::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360Flip::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2SphereExpandedPlane3d360 projector;
		projector.SetFlip(true);
		projector.SetIsFullSphere(false);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedSphere360Flip::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorSphereExpandedPlane3d2PhysicalDirection360 projector;
		projector.SetFlip(true);
		projector.SetIsFullSphere(false);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedSphere360Flip::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedSphere360Flip::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinder180Flip::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinder180Flip, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinder180Flip), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinder180Flip));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetFlip(true);
			vProjectorObjs[iProjectorObjIdx].SetTheta(0.0);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180Flip::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180Flip::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180Flip::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2CylinderExpandedPlane3d projector;
		projector.SetFlip(true);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder180Flip::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorCylinderExpandedPlane3d2PhysicalDirection projector;
		projector.SetFlip(true);
		projector.SetTheta(0.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinder180Flip::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder180Flip::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedCylinder360Flip::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedCylinder360Flip, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedCylinder360Flip), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedCylinder360Flip));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
			vProjectorObjs[iProjectorObjIdx].SetFlip(true);
			vProjectorObjs[iProjectorObjIdx].SetTheta(0.0);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360Flip::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360Flip::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360Flip::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2CylinderExpandedPlane3d projector;
		projector.SetFlip(true);
		projector.SetTheta(-M_PI / 2.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedCylinder360Flip::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorCylinderExpandedPlane3d2PhysicalDirection projector;
		projector.SetFlip(true);
		projector.SetTheta(-M_PI / 2.0);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedCylinder360Flip::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedCylinder360Flip::Release(): exception.";
		return;
	}
}

bool GPanoSceneImmersionFullSphere::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorSphere> ptrSphereSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorSphere>(new GSurfaceGeneratorSphere);
		ptrSphereSurfaceGenerator->SetIsFullSphere(true);
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrSphereSurfaceGenerator;

		std::vector<GProjectorObjectGlobal3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);
			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);
			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::Create(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionFullSphere::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneImmersionFullSphere::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::Render(): exception.";
		return false;
	}

}

bool GPanoSceneImmersionFullSphere::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		v3fScene3DPointf = v3fPhysical3DPoint;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::Project(): exception.";
		return false;
	}

}

bool GPanoSceneImmersionFullSphere::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		v3fPhysical3DPoint = v3fScene3DPointf;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneImmersionFullSphere::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneImmersionFullSphere::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedFullSphere360::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedFullSphere360, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedFullSphere360), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedFullSphere360));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);

			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);

			vProjectorObjs[iProjectorObjIdx].SetFlip(false);

			vProjectorObjs[iProjectorObjIdx].SetIsFullSphere(true);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2SphereExpandedPlane3d360 projector;
		projector.SetFlip(false);
		projector.SetIsFullSphere(true);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorSphereExpandedPlane3d2PhysicalDirection360 projector;
		projector.SetFlip(false);
		projector.SetIsFullSphere(true);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedFullSphere360::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedFullSphere360Flip::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedFullSphere360Flip, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedFullSphere360Flip), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedFullSphere360Flip));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iProjectorObjIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iProjectorObjIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iProjectorObjIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iProjectorObjIdx].eCameraType);

			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iProjectorObjIdx].dDistortionValidRange);

			vProjectorObjs[iProjectorObjIdx].SetFlip(true);

			vProjectorObjs[iProjectorObjIdx].SetIsFullSphere(true);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Flip::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Flip::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Flip::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		GProjectorPhysicalDirection2SphereExpandedPlane3d360 projector;
		projector.SetFlip(true);
		projector.SetIsFullSphere(true);
		cv::Point3f ptRet = projector(cv::Point3f(v3fPhysical3DPoint[0], v3fPhysical3DPoint[1], v3fPhysical3DPoint[2]));
		v3fScene3DPointf[0] = ptRet.x;
		v3fScene3DPointf[1] = ptRet.y;
		v3fScene3DPointf[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Flip::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		GProjectorSphereExpandedPlane3d2PhysicalDirection360 projector;
		projector.SetFlip(true);
		projector.SetIsFullSphere(true);
		cv::Point3f ptRet = projector(cv::Point3f(v3fScene3DPointf[0], v3fScene3DPointf[1], v3fScene3DPointf[2]));
		v3fPhysical3DPoint[0] = ptRet.x;
		v3fPhysical3DPoint[1] = ptRet.y;
		v3fPhysical3DPoint[2] = ptRet.z;
		return true;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedFullSphere360Flip::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Flip::Release(): exception.";
		return;
	}
}

bool GPanoSceneUnwrappedFullSphere360Binocular::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeUnwrappedFullSphere360Binocular, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeUnwrappedFullSphere360Binocular), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeUnwrappedFullSphere360Binocular));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular> vProjectorObjs(vCameras.size() - 1);
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			int iCameraIdx = iProjectorObjIdx;
			bool bIsLeft = iProjectorObjIdx % 2 == 0;

			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iCameraIdx);
			vProjectorObjs[iProjectorObjIdx].SetR10(vCameras[iCameraIdx].R.inv());
			vProjectorObjs[iProjectorObjIdx].SetDist1(vCameras[iCameraIdx].distorsionCoefficients);

			cv::Mat mK0ForSize;
			if (!GetK0(iCameraIdx, sK0Name, vCameras, mK0ForSize))
			{
				GLOGGER(error) << "GStitcherOGL::CreateScene(). get K0 failed.";
				GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
				return false;
			}
			vProjectorObjs[iProjectorObjIdx].SetK1(mK0ForSize);

			vProjectorObjs[iProjectorObjIdx].SetCameraType(vCameras[iCameraIdx].eCameraType);

			vProjectorObjs[iProjectorObjIdx].SetIsLeft(bIsLeft);

			vProjectorObjs[iProjectorObjIdx].SetDistortionValidRange(vCameras[iCameraIdx].dDistortionValidRange);

			if (iProjectorObjIdx < vCameras.size())
			{
				if (bIsLeft)
				{
					//theoretically: 0.125 - 0.25
					vProjectorObjs[iProjectorObjIdx].SetTheta(M_PI * -0.49, M_PI * 0.49);

					vProjectorObjs[iProjectorObjIdx].SetPhi(M_PI * -0.4, M_PI * 0.4);
				}
				else
				{
					vProjectorObjs[iProjectorObjIdx].SetTheta(M_PI * -0.49, M_PI * 0.49);

					vProjectorObjs[iProjectorObjIdx].SetPhi(M_PI * -0.4, M_PI * 0.4);
				}
			}
			//else
			//{
			//	vProjectorObjs[iProjectorObjIdx].SetTheta(-M_PI / 2.001, M_PI / 2.001);

			//	vProjectorObjs[iProjectorObjIdx].SetPhi(-M_PI / 2.001, M_PI / 2.001);
			//}

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::Create(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Binocular::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Binocular::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::Render(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Binocular::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::Project(): exception.";
		return false;
	}
}

bool GPanoSceneUnwrappedFullSphere360Binocular::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneUnwrappedFullSphere360Binocular::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneUnwrappedFullSphere360Binocular::Release(): exception.";
		return;
	}
}

bool GPanoSceneOriginal::Create(const GStitcherConfig& config, const std::vector<CameraModel>& vCameras, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	try
	{
		m_config = config;

		cv::Rect_<float> panoRect = GPanoTypeStaticParameters::GetPanoRectOnCanvas(ePanoTypeOriginal, v4fCropRatios);
		std::function<bool(float, float, float)> panoPixSelector = [panoRect](float fX, float fY, float fZ)
		{
			return (fX >= (panoRect.tl().x - FLT_EPSILON)) && (fX <= (panoRect.br().x + FLT_EPSILON))
				&& (fY >= (panoRect.tl().y - FLT_EPSILON)) && (fY <= (panoRect.br().y + FLT_EPSILON));
		};

		m_ptrRenderScene = std::shared_ptr<GRenderScene>(new GRenderScene());
		std::shared_ptr<GSurfaceGeneratorPlane> ptrPlaneSurfaceGenerator = std::shared_ptr<GSurfaceGeneratorPlane>(new GSurfaceGeneratorPlane);
		ptrPlaneSurfaceGenerator->SetSurfaceSize(
			GPanoTypeStaticParameters::GetDefaultCanvasWidth(ePanoTypeOriginal), GPanoTypeStaticParameters::GetDefaultCanvasHeight(ePanoTypeOriginal));
		std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator = ptrPlaneSurfaceGenerator;

		std::vector<GProjectorObjectOriginal3d2NormalizedPix> vProjectorObjs(vCameras.size());
		for (int iProjectorObjIdx = 0; iProjectorObjIdx < vProjectorObjs.size(); ++iProjectorObjIdx)
		{
			vProjectorObjs[iProjectorObjIdx].SetCameraIdx(iProjectorObjIdx);

			vProjectorObjs[iProjectorObjIdx].SetNumOfCameras(vCameras.size());

			vProjectorObjs[iProjectorObjIdx].SetIndexOfCamera(iProjectorObjIdx);

			vProjectorObjs[iProjectorObjIdx].SetSelectiveFunctor(panoPixSelector);
		}

		if (!m_ptrRenderScene->Create(ptrSurfaceGenerator, vProjectorObjs))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). create render scene failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneOriginal::Create(): exception.";
		return false;
	}
}

bool GPanoSceneOriginal::Prepare()
{
	try
	{
		if (!m_ptrRenderScene->Prepare(m_config.m_eInputFrameFormat))
		{
			GLOGGER(error) << "GStitcherOGL::CreateScene(). render scene prepare failed.";
			GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
			return false;
		}

		return true;
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneOriginal::Prepare(): exception.";
		return false;
	}
}

bool GPanoSceneOriginal::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	try
	{
		return m_ptrRenderScene->Render(textureGroup, matP, matV);
	}
	catch (...)
	{
		Release();
		GLOGGER(error) << "GPanoSceneOriginal::Render(): exception.";
		return false;
	}
}

bool GPanoSceneOriginal::Project(const Eigen::Vector3f& v3fPhysical3DPoint, Eigen::Vector3f& v3fScene3DPointf) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneOriginal::Project(): exception.";
		return false;
	}
}

bool GPanoSceneOriginal::UnProject(const Eigen::Vector3f& v3fScene3DPointf, Eigen::Vector3f& v3fPhysical3DPoint) const
{
	try
	{
		return false;
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneOriginal::UnProject(): exception.";
		return false;
	}
}

void GPanoSceneOriginal::Release()
{
	try
	{
		if (nullptr != m_ptrRenderScene.get())
		{
			m_ptrRenderScene->Release();
			m_ptrRenderScene.reset();
		}
	}
	catch (...)
	{
		GLOGGER(error) << "GPanoSceneOriginal::Release(): exception.";
		return;
	}
}

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
 
 
 
#ifndef G_STITCHER_OGL_H_
#define G_STITCHER_OGL_H_

#include "Common/DllExport/GCallConvention.h"
#include "GCameraModel.h"

#include <memory>

#include "PanoRender/GPanoRenderCommonIF.h"
#include "GPanoCameraInfo.h"
#include "GViewCamera.h"
#include "PanoRender/GPanoSceneIF.h"
#include "Common/Buffers/GTaskQueue.h"

typedef void(G_CALL_BACK_CALL_CONVENTION *tRenderCallBack)(void* pContext);

class GStitcherOGL
{
public:
	GStitcherOGL()
		:m_config(GStitcherConfig())
		, m_ptrCurrentScene(std::shared_ptr<GPanoSceneIF>(nullptr))
	{}

	bool Init(const GStitcherConfig& stitcherConfig, const std::vector<CameraModel>& vCameras);

	bool IsSceneExist(const std::string& sSceneName);

	bool CreateScene(const std::string& sSceneName, enPanoType ePanoType, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios);

	bool SwitchScene(const std::string& sSceneName);

	bool SetPreRenderCallBack(tRenderCallBack pPreRenderCallBack, void* pContext);

	bool SetRenderCallBack(tRenderCallBack pRenderCallBack, void* pContext);

	bool RenderPano(const GTextureGroup& textGroup, const glm::mat4& matP, const glm::mat4& matV);

	bool UnProjectPix2Physical(float fXPix, float fYPix, float fDepth, float& fX, float& fY, float& fZ, const glm::vec4& mvViewPort);

	void Release();

private:
	GStitcherConfig m_config;

	std::vector<CameraModel> m_vCameras;

	std::map<std::string, std::shared_ptr<GPanoSceneIF>> m_mpRenderScenes;
	std::shared_ptr<GPanoSceneIF> m_ptrCurrentScene;

	glm::mat4 m_glmP;
	glm::mat4 m_glmV;

	struct GRenderCallBack
	{
		GRenderCallBack() : pRenderCallBackFunc(nullptr)
		, pContext(nullptr)
		{}

		tRenderCallBack pRenderCallBackFunc;
		void* pContext;
	};

	GRenderCallBack m_PreRenderCallBack;
	GRenderCallBack m_RenderCallBack;
};

#endif //G_STITCHER_OGL_H_
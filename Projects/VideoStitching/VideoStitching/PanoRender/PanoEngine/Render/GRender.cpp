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
 
 
 
#include "GRender.h"

#include <chrono>
#include <thread>

#include "Common/DllExport/GDllExport.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "GRenderParaManager.h"
#include "GGlobalParametersManager.h"

extern "C"
{
#ifdef WIN32
#include <windows.h>
	G_DLL_EXPORT DWORD NvOptimusEnablement = 0x00000001;
#endif //WIN32
}

#if (defined __APPLE__) || (defined __ANDROID__)
#elif (defined WIN32) || (defined __linux__)
void G_CALL_BACK_CALL_CONVENTION DebugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	std::string sMsg = "OpenGL Debug Output message. ";

	if (source == GL_DEBUG_SOURCE_API_ARB)					sMsg += "Source : API; ";
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	sMsg += "Source : WINDOW_SYSTEM; ";
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	sMsg += "Source : SHADER_COMPILER; ";
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		sMsg += "Source : THIRD_PARTY; ";
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		sMsg += "Source : APPLICATION; ";
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			sMsg += "Source : OTHER; ";

	if (type == GL_DEBUG_TYPE_ERROR_ARB)					sMsg += "Type : ERROR; ";
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	sMsg += "Type : DEPRECATED_BEHAVIOR; ";
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	sMsg += "Type : UNDEFINED_BEHAVIOR; ";
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			sMsg += "Type : PORTABILITY; ";
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			sMsg += "Type : PERFORMANCE; ";
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				sMsg += "Type : OTHER; ";

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				sMsg += "Severity : HIGH; ";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		sMsg += "Severity : MEDIUM; ";
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			sMsg += "Severity : LOW; ";

	// You can set a breakpoint here ! Your debugger will stop the program,
	// and the callstack will immediately show you the offending call.
	GLOGGER(info) << sMsg << " Message : " << std::string(message);
}
#endif //(defined __APPLE__) || (defined __ANDROID__)

bool GStitcherOGL::Init(const GStitcherConfig& stitcherConfig, const std::vector<CameraModel>& vCameras)
{
	GLOGGER(info) << "GStitcherOGL::Init().";

	GGlobalParametersManager::GetInstance()->Init();

	if (!opengl_compatible(2, 1))
	{
		GLOGGER(error) << "GStitcherOGL::Init(): Error. OpenGL version is NOT SUPPORTED. Init failed.";
		return false;
	}

	if ((!opengl_compatible(3, 0)) && (GStitcherConfig::eRenderingModeOnScreen != m_config.m_eRenderingMode))
	{
		GLOGGER(error) << "GStitcherOGL::Init(): Error. OpenGL version is NOT SUPPORTED in offline mode. Init failed.";
		return false;
	}

#if (defined __APPLE__) || (defined __ANDROID__)
#elif (defined WIN32) || (defined __linux__)
	if (GLEW_ARB_debug_output)
	{
		//glEnable(GL_DEBUG_OUTPUT);
		//GLOGGER(info) << "The OpenGL implementation provides debug output. Use it NOW.";
		//glDebugMessageCallbackARB(&DebugOutputCallback, NULL);
		//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}
	else
	{
		GLOGGER(info) << "ARB_debug_output unavailable. You have to use glGetError() and/or gDebugger to catch mistakes.";
	}
#endif //(defined __APPLE__) || (defined __ANDROID__)

	m_config = stitcherConfig;
	m_vCameras = vCameras;

	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLOGGER(info) << "leaving GStitcherOGL::Init().";
	return true;
}

bool GStitcherOGL::IsSceneExist(const std::string& sSceneName)
{
	if ("" == sSceneName)
	{
		return (m_ptrCurrentScene != std::shared_ptr<GPanoSceneIF>(nullptr));
	}
	else
	{
		return (m_mpRenderScenes.end() != m_mpRenderScenes.find(sSceneName));
	}
}

bool GStitcherOGL::CreateScene(const std::string& sSceneName, enPanoType ePanoType, const std::string& sK0Name, const Eigen::Vector4f& v4fCropRatios)
{
	//GLOGGER(info) << "==> GStitcherOGL::CreateScene().";
	std::shared_ptr<GPanoSceneIF> ptrRenderScene = GPanoSceneFactory::CreatePanoScene(ePanoType);
	if (m_mpRenderScenes.end() != m_mpRenderScenes.find(sSceneName))
	{
		m_mpRenderScenes[sSceneName]->Release();
	}

	m_mpRenderScenes[sSceneName] = ptrRenderScene;

	bool bRet = false;
	if (ptrRenderScene->Create(m_config, m_vCameras, sK0Name, v4fCropRatios))
	{
		bRet = ptrRenderScene->Prepare();
	}
	else
	{
		m_mpRenderScenes.erase(sSceneName);
		GLOGGER(error) << "GStitcherOGL::CreateScene() ERROR.";
	}

	//GLOGGER(info) << "GStitcherOGL::CreateScene() ==>";
	return bRet;
}

bool GStitcherOGL::SwitchScene(const std::string& sSceneName)
{
	if (m_mpRenderScenes.end() == m_mpRenderScenes.find(sSceneName))
	{
		return false;
	}
	else
	{
		m_ptrCurrentScene = m_mpRenderScenes[sSceneName];
		return true;
	}
}

bool GStitcherOGL::SetPreRenderCallBack(tRenderCallBack pPreRenderCallBack, void* pContext)
{
	m_PreRenderCallBack.pRenderCallBackFunc = pPreRenderCallBack;
	m_PreRenderCallBack.pContext = pContext;
	return true;
}

bool GStitcherOGL::SetRenderCallBack(tRenderCallBack pRenderCallBack, void* pContext)
{
	m_RenderCallBack.pRenderCallBackFunc = pRenderCallBack;
	m_RenderCallBack.pContext = pContext;
	return true;
}

bool GStitcherOGL::RenderPano(const GTextureGroup& textGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	//Rendering.
	if (nullptr != m_PreRenderCallBack.pRenderCallBackFunc)
	{
		m_PreRenderCallBack.pRenderCallBackFunc(m_PreRenderCallBack.pContext);
	}

	m_ptrCurrentScene->Render(textGroup, matP, matV);

	if (nullptr != m_RenderCallBack.pRenderCallBackFunc)
	{
		m_RenderCallBack.pRenderCallBackFunc(m_RenderCallBack.pContext);
	}

	//Finish.
	m_glmP = matP;
	m_glmV = matV;

	glFlush();

	return true;
}

bool GStitcherOGL::UnProjectPix2Physical(float fXPix, float fYPix, float fDepth, float& fX, float& fY, float& fZ, const glm::vec4& mvViewPort)
{
	if (1.0f == fDepth)
	{
		fX = FLT_MAX;
		fY = FLT_MAX;
		fZ = FLT_MAX;
		return true;
	}

	glm::vec3 glvRet = glm::unProject(glm::vec3(fXPix, fYPix, fDepth), m_glmV, m_glmP, mvViewPort);

	//from scene to physical direction.
	Eigen::Vector3f v3fPhysicalDirection;
	if (!m_ptrCurrentScene->UnProject(Eigen::Vector3f(glvRet[0], glvRet[1], glvRet[2]), v3fPhysicalDirection))
	{
		return false;
	}

	fX = v3fPhysicalDirection[0];
	fY = v3fPhysicalDirection[1];
	fZ = v3fPhysicalDirection[2];

	return true;
}

void GStitcherOGL::Release()
{
	for (auto it = m_mpRenderScenes.begin(); it != m_mpRenderScenes.end(); ++it)
	{
		it->second->Release();
	}
}


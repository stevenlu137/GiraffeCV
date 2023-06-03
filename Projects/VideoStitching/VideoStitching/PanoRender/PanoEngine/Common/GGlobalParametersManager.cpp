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
 
 
 
#include "GGlobalParametersManager.h"

#include <string>
#include <vector>
#include <regex>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/find.hpp>

#include "GGLHeaders.h"

#include "Common/Exception/GiraffeChecker.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"


GGlobalParametersManager* GGlobalParametersManager::GetInstance()
{
	static GGlobalParametersManager Singleton;
	return &Singleton;
}

void GGlobalParametersManager::Init()
{
	std::string sGLVersion((char*)glGetString(GL_VERSION));
	GLOGGER(info) << "OpenGL version: " << sGLVersion;

	if (!boost::ifind_first(sGLVersion, "nvidia").empty())
	{
		m_eGPUVendor = eGGPUVendorNvidia;
		GLOGGER(info) << "GPU Vendor recognized: NVIDIA." << sGLVersion;
	}
	else if ((!boost::ifind_first(sGLVersion, "amd").empty()) ||
		(!boost::ifind_first(sGLVersion, "ati").empty()))
	{
		m_eGPUVendor = eGGPUVendorAMD;
		GLOGGER(info) << "GPU Vendor recognized: AMD." << sGLVersion;
	}
	else if (!boost::ifind_first(sGLVersion, "intel").empty())
	{
		m_eGPUVendor = eGGPUVendorIntel;
		GLOGGER(info) << "GPU Vendor recognized: Intel." << sGLVersion;
	}
	else
	{
		m_eGPUVendor = eGGPUVendorUnknown;
		GLOGGER(info) << "GPU Vendor is NOT recognized." << sGLVersion;
	}

	m_aOpenGLVersion[0] = 0;
	m_aOpenGLVersion[1] = 0;

	std::regex rgExprGLVersion("(\\d+).(\\d+)");
	std::smatch smResultGLVersion;
	if (std::regex_search(sGLVersion, smResultGLVersion, rgExprGLVersion))
	{
		if (smResultGLVersion[0].matched)
		{
			m_aOpenGLVersion[0] = atoi(std::string(smResultGLVersion[1].first, smResultGLVersion[1].second).c_str());
			m_aOpenGLVersion[1] = atoi(std::string(smResultGLVersion[2].first, smResultGLVersion[2].second).c_str());
		}
		else
		{
			GLOGGER(error) << "Error: Failed to get OpenGL version. " << sGLVersion;
			throw GPanoRenderException();
		}
	}

	GLOGGER(info) << "OpenGL version: " << m_aOpenGLVersion[0] << "." << m_aOpenGLVersion[1];
	//m_aOpenGLVersion[0] = 2;
	//m_aOpenGLVersion[1] = 1;

	std::string sGLSLVersion((char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	GLOGGER(info) << "GLSL version: " << sGLSLVersion;

	m_aGLSLVersion[0] = 0;
	m_aGLSLVersion[1] = 0;

	std::regex rgExprGLSLVersion("(\\d+).(\\d\\d)");
	std::smatch smResultGLSLVersion;
	if (std::regex_search(sGLSLVersion, smResultGLSLVersion, rgExprGLSLVersion))
	{
		if (smResultGLSLVersion[0].matched)
		{
			m_aGLSLVersion[0] = atoi(std::string(smResultGLSLVersion[1].first, smResultGLSLVersion[1].second).c_str());
			m_aGLSLVersion[1] = atoi(std::string(smResultGLSLVersion[2].first, smResultGLSLVersion[2].second).c_str());
		}
		else
		{
			GLOGGER(error) << "Error: Failed to get GLSL version. " << sGLSLVersion;
			throw GPanoRenderException();
		}
	}

	//m_aGLSLVersion[0] = 1;
	//m_aGLSLVersion[1] = 20;
}

int GGlobalParametersManager::GetParamInt(enGGlobalParam eParamName)
{
	if (eGGlobalParamIntGLMajorVersion == eParamName)
	{
		return m_aOpenGLVersion[0];
	}
	else if (eGGlobalParamIntGLMinorVersion == eParamName)
	{
		return m_aOpenGLVersion[1];
	}
	else if (eGGlobalParamIntGLSLMajorVersion == eParamName)
	{
		return m_aGLSLVersion[0];
	}
	else if (eGGlobalParamIntGLSLMinorVersion == eParamName)
	{
		return m_aGLSLVersion[1];
	}
	else if (eGGlobalParamIntGPUVendor == eParamName)
	{
		return m_eGPUVendor;
	}
	else
	{
		throw GPanoRenderException();
	}
}

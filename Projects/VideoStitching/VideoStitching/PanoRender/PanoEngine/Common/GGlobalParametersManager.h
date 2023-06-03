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
 
 
 
#ifndef G_GLOBAL_PARAMETERS_MANAGER_H_
#define G_GLOBAL_PARAMETERS_MANAGER_H_


enum enGGlobalParam
{
	eGGlobalParamInt = 0,
	eGGlobalParamIntGLMajorVersion,
	eGGlobalParamIntGLMinorVersion,
	eGGlobalParamIntGLSLMajorVersion,
	eGGlobalParamIntGLSLMinorVersion,
	eGGlobalParamIntGPUVendor,

	eGGlobalParamFloat = 100,

	eGGlobalParamString=200,


};

enum enGGPUVendor
{
	eGGPUVendorNvidia = 0,
	eGGPUVendorAMD,
	eGGPUVendorIntel,
	eGGPUVendorUnknown,
};

class GGlobalParametersManager
{
public:
	static GGlobalParametersManager* GetInstance();

	void Init();

	int GetParamInt(enGGlobalParam eParamName);
	double GetParamFloat(enGGlobalParam eParamName);

private:
	GGlobalParametersManager(){};
	GGlobalParametersManager(const GGlobalParametersManager&){};

	int m_aOpenGLVersion[2];
	int m_aGLSLVersion[2];

	enGGPUVendor m_eGPUVendor;
};


#define opengl_compatible(major,minor) (GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGLMajorVersion) > (major) || \
(GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGLMajorVersion) == (major) && GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGLMinorVersion) >= (minor)))


#endif //G_GLOBAL_PARAMETERS_MANAGER_H_
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
 
 
 
#ifndef G_GLSL_PROGRAM_GENERATOR_H_
#define G_GLSL_PROGRAM_GENERATOR_H_

#include <string>

#include "GGLHeaders.h"

class GGLSLBlenderGenerator
{
public:
	enum  enGVertexShaderType
	{
		eGVertexShaderTypeNormal = 0,
	};

	enum enGFragShaderType
	{
		eGFragShaderTypeNormal = 0,
		eGFragShaderTypeYUV,
		eGFragShaderTypeNV12,
	};

	static bool GetProgram(enGVertexShaderType eVertexShaderType, enGFragShaderType eFragShaderType, int iGLSLVersion, int iNumOfTextureUnits, GLuint& uiProgram);

private:
	static std::string GenerateVertexShaderCode120(int iNumOfTextureUnits);
	static std::string GenerateFragShaderCode120(int iNumOfTextureUnits);
	static std::string GenerateFragShaderYUVCode120(int iNumOfTextureUnits);
	static std::string GenerateFragShaderNV12Code120(int iNumOfTextureUnits);

	static std::string GenerateVertexShaderCode330(int iNumOfTextureUnits);
	static std::string GenerateFragShaderCode330(int iNumOfTextureUnits);
	static std::string GenerateFragShaderYUVCode330(int iNumOfTextureUnits);
	static std::string GenerateFragShaderNV12Code330(int iNumOfTextureUnits);

};


#endif //G_GLSL_PROGRAM_GENERATOR_H_
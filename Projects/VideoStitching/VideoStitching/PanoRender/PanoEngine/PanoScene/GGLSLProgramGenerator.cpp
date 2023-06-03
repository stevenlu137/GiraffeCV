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
 
 
 
#include "GGLSLProgramGenerator.h"

#include <vector>
#include <iostream>
#include <sstream>

#include "GGlobalParametersManager.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"


std::string GGLSLBlenderGenerator::GenerateVertexShaderCode330(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 330 core\n"
		<< "layout(location = 0) in vec3 vertexPosition;\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "layout(location = " << 1 + i << ") in vec3 vertexUV" << i << ";\n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "out vec3 UV" << i << ";\n";
	}

	sStream << "uniform mat4 MVP;\n";

	sStream << "void main(){\n"
		<< "gl_Position = MVP * vec4(vertexPosition, 1);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "UV" << i << "= vertexUV" << i << ";\n";
	}

	sStream << "}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;

	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderCode330(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 330 core\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "in vec3 UV" << i << ";\n";
	}

	sStream << "out vec4 color;\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << ";\n";
	}

	sStream << "void main(){\n"
		<< "color.rgb = ";

	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * " << "texture(textureSampler" << i << ", UV" << i << ".st).rgb +";
	}

	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * " << "texture(textureSampler" << iNumOfTextureUnits - 1 << ", UV" << iNumOfTextureUnits - 1 << ".st).rgb;\n";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   color.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   color.a = 1.0;\n"
		<< "}\n";

	//sRet += "color=color/";
	//sRet += std::to_string(iNumOfTextureUnits);
	//sRet += ";\n";

	//sStream << "color=vec3(1.0,1.0,1.0);\n";


	sStream << "}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderYUVCode330(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 330 core\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "in vec3 UV" << i << ";\n";
	}

	sStream << "out vec4 color;\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << 0 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 1 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 2 << ";\n";
	}

	sStream << "void main(){\n"
		<< "mat3 conv = mat3(1.0, 1.0, 1.0,"
		<< "-0.00093, -0.3437, 1.77216,"
		<< "1.401687, -0.71417, 0.00099);\n"
		<< "vec3 offset = vec3(0.0, -0.5, -0.5);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 yuv" << i
			<< " = vec3(texture(textureSampler" << i << "0, UV" << i << ".st).r, texture(textureSampler" << i << "1, UV" << i << ".st).r, texture(textureSampler" << i << "2, UV" << i << ".st).r); \n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 tex" << i << " = conv * (yuv" << i << " + offset);\n";
	}
	sStream << "color.rgb = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * tex" << i << ".rgb +";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * tex" << iNumOfTextureUnits - 1 << ".rgb;";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   color.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   color.a = 1.0;\n"
		<< "}\n";

	//sStream << "color = vec4(1.0,1.0,1.0,1.0);";

	sStream << "\n}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderNV12Code330(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 330 core\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "in vec3 UV" << i << ";\n";
	}

	sStream << "out vec4 color;\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << 0 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 1 << ";\n";
	}

	sStream << "void main(){\n"
		<< "mat3 conv = mat3(1.0, 1.0, 1.0,"
		<< "-0.00093, -0.3437, 1.77216,"
		<< "1.401687, -0.71417, 0.00099);\n"
		<< "vec3 offset = vec3(0.0, -0.5, -0.5);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 yuv" << i
			<< " = vec3(texture(textureSampler" << i << "0, UV" << i << ".st).r, texture(textureSampler" << i << "1, UV" << i << ".st).rg); \n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 tex" << i << " = conv * (yuv" << i << " + offset);\n";
	}
	sStream << "color.rgb = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * tex" << i << ".rgb +";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * tex" << iNumOfTextureUnits - 1 << ".rgb;";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   color.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   color.a = 1.0;\n"
		<< "}\n";

	//sStream << "color = vec4(1.0,1.0,1.0,1.0);";

	sStream << "\n}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateVertexShaderCode120(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 120\n"
		<< "attribute vec3 vertexPosition;\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "attribute vec3 vertexUV" << i << ";\n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "varying vec3 UV" << i << ";\n";
	}

	sStream << "uniform mat4 MVP;\n";

	sStream << "void main(){\n"
		<< "gl_Position = MVP * vec4(vertexPosition, 1);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "UV" << i << "= vertexUV" << i << ";\n";
	}

	sStream << "}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;

	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderCode120(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 120\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "varying vec3 UV" << i << ";\n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << ";\n";
	}

	sStream << "void main(){\n"
		<< "gl_FragColor.rgb = ";

	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * " << "texture2D(textureSampler" << i << ", UV" << i << ".st).rgb +";
	}

	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * " << "texture2D(textureSampler" << iNumOfTextureUnits - 1 << ", UV" << iNumOfTextureUnits - 1 << ".st).rgb;\n";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   gl_FragColor.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   gl_FragColor.a = 1.0;\n"
		<< "}\n";

	sStream << "}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderYUVCode120(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 120\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "varying vec3 UV" << i << ";\n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << 0 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 1 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 2 << ";\n";
	}

	sStream << "void main(){\n"
		<< "mat3 conv = mat3(1.0, 1.0, 1.0,"
		<< "-0.00093, -0.3437, 1.77216,"
		<< "1.401687, -0.71417, 0.00099);\n"
		<< "vec3 offset = vec3(0.0, -0.5, -0.5);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 yuv" << i
			<< " = vec3(texture2D(textureSampler" << i << "0, UV" << i << ".st).r, texture2D(textureSampler" << i << "1, UV" << i << ".st).r, texture2D(textureSampler" << i << "2, UV" << i << ".st).r); \n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 tex" << i << " = conv * (yuv" << i << " + offset);\n";
	}
	sStream << "gl_FragColor.rgb = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * tex" << i << ".rgb +";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * tex" << iNumOfTextureUnits - 1 << ".rgb;";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   gl_FragColor.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   gl_FragColor.a = 1.0;\n"
		<< "}\n";

	sStream << "\n}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

std::string GGLSLBlenderGenerator::GenerateFragShaderNV12Code120(int iNumOfTextureUnits)
{
	std::stringstream sStream;

	sStream << "#version 120\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "varying vec3 UV" << i << ";\n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream << "uniform sampler2D textureSampler" << i << 0 << ";\n";
		sStream << "uniform sampler2D textureSampler" << i << 1 << ";\n";
	}

	sStream << "void main(){\n"
		<< "mat3 conv = mat3(1.0, 1.0, 1.0,"
		<< "-0.00093, -0.3437, 1.77216,"
		<< "1.401687, -0.71417, 0.00099);\n"
		<< "vec3 offset = vec3(0.0, -0.5, -0.5);\n";

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 yuv" << i
			<< " = vec3(texture2D(textureSampler" << i << "0, UV" << i << ".st).r, texture2D(textureSampler" << i << "1, UV" << i << ".st).rg); \n";
	}

	for (int i = 0; i < iNumOfTextureUnits; ++i)
	{
		sStream <<
			"vec3 tex" << i << " = conv * (yuv" << i << " + offset);\n";
	}
	sStream << "gl_FragColor.rgb = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p * tex" << i << ".rgb +";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p * tex" << iNumOfTextureUnits - 1 << ".rgb;";


	//cut dark edge.
	sStream << "float fTotalWeight = ";
	for (int i = 0; i < iNumOfTextureUnits - 1; ++i)
	{
		sStream << "UV" << i << ".p" << "+";
	}
	sStream << "UV" << iNumOfTextureUnits - 1 << ".p;\n";

	sStream << "if (fTotalWeight < 0.95)\n"
		<< "{\n"
		<< "   gl_FragColor.a = 0.0;\n"
		<< "}\n"
		<< "else\n"
		<< "{\n"
		<< "   gl_FragColor.a = 1.0;\n"
		<< "}\n";

	sStream << "\n}";

	std::string sRet = "";
	sRet = sStream.str();
	//std::cout << sRet << std::endl;
	return sRet;
}

bool GGLSLBlenderGenerator::GetProgram(enGVertexShaderType eVertexShaderType, enGFragShaderType eFragShaderType, int iGLSLVersion, int iNumOfTextureUnits
	, GLuint& uiProgramID)
{
	// Create the shaders
	GLuint uiVertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint uiFragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint iResult = GL_FALSE;
	int iInfoLogLength;

	// Get Vertex Shader
	std::string sVertexShaderCode = "";
	if (GGLSLBlenderGenerator::eGVertexShaderTypeNormal == eVertexShaderType)
	{
		if (iGLSLVersion >= 330)
		{
			sVertexShaderCode = GenerateVertexShaderCode330(iNumOfTextureUnits);
		}
		else if (iGLSLVersion >= 120)
		{
			sVertexShaderCode = GenerateVertexShaderCode120(iNumOfTextureUnits);
		}
		else
		{
			GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). GLSL version is too low: " << iGLSLVersion;
			return false;
		}
	}
	else
	{
		GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). Invalid vertex shader type.";
		return false;
	}

	// Compile Vertex Shader
	char const * pVertexSourcePointer = sVertexShaderCode.c_str();
	glShaderSource(uiVertexShaderID, 1, &pVertexSourcePointer, NULL);
	glCompileShader(uiVertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(uiVertexShaderID, GL_COMPILE_STATUS, &iResult);
	if (GL_FALSE == iResult)
	{
		glGetShaderiv(uiVertexShaderID, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		iInfoLogLength = (iInfoLogLength < 0) ? 0 : iInfoLogLength;

		std::vector<char> vVertexShaderErrorMessage(iInfoLogLength + 1);
		glGetShaderInfoLog(uiVertexShaderID, iInfoLogLength, NULL, &vVertexShaderErrorMessage[0]);
		GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). " << std::string(vVertexShaderErrorMessage.data());
		return false;
	}

	// Get Fragment Shader
	std::string sFragShaderCode = "";
	if (GGLSLBlenderGenerator::eGFragShaderTypeNormal == eFragShaderType)
	{
		if (iGLSLVersion >= 330)
		{
			sFragShaderCode = GenerateFragShaderCode330(iNumOfTextureUnits);
		}
		else if (iGLSLVersion >= 120)
		{
			sFragShaderCode = GenerateFragShaderCode120(iNumOfTextureUnits);
		}
		else
		{
			GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). GLSL version is too low: " << iGLSLVersion;
			return false;
		}
	}
	else if (GGLSLBlenderGenerator::eGFragShaderTypeYUV == eFragShaderType)
	{
		if (iGLSLVersion >= 330)
		{
			sFragShaderCode = GenerateFragShaderYUVCode330(iNumOfTextureUnits);
		}
		else if (iGLSLVersion >= 120)
		{
			sFragShaderCode = GenerateFragShaderYUVCode120(iNumOfTextureUnits);
		}
		else
		{
			GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). GLSL version is too low: " << iGLSLVersion;
			return false;
		}
	}
	else if (GGLSLBlenderGenerator::eGFragShaderTypeNV12 == eFragShaderType)
	{
		if (iGLSLVersion >= 330)
		{
			sFragShaderCode = GenerateFragShaderNV12Code330(iNumOfTextureUnits);
		}
		else if (iGLSLVersion >= 120)
		{
			sFragShaderCode = GenerateFragShaderNV12Code120(iNumOfTextureUnits);
		}
		else
		{
			GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). GLSL version is too low: " << iGLSLVersion;
			return false;
		}
	}
	else
	{
		GLOGGER(error) << "GGLSLBlenderGenerator::GetProgram(). Invalid fragment shader type.";
		return false;
	}

	// Compile Fragment Shader
	char const * pFragmentSourcePointer = sFragShaderCode.c_str();
	glShaderSource(uiFragmentShaderID, 1, &pFragmentSourcePointer, NULL);
	glCompileShader(uiFragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(uiFragmentShaderID, GL_COMPILE_STATUS, &iResult);
	if (GL_FALSE == iResult)
	{
		glGetShaderiv(uiFragmentShaderID, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		iInfoLogLength = (iInfoLogLength < 0) ? 0 : iInfoLogLength;

		std::vector<char> vFragmentShaderErrorMessage(iInfoLogLength + 1);
		glGetShaderInfoLog(uiFragmentShaderID, iInfoLogLength, NULL, &vFragmentShaderErrorMessage[0]);
		GLOGGER(error) << "Fragment Shader: " << std::string(vFragmentShaderErrorMessage.data());
		return false;
	}


	// Link the program
	uiProgramID = glCreateProgram();
	glAttachShader(uiProgramID, uiVertexShaderID);
	glAttachShader(uiProgramID, uiFragmentShaderID);

	//for OpenGL < 3.3, the corresponding attribute name in shader should be explicitly specified.
	if (opengl_compatible(3, 3))
	{
		glBindAttribLocation(uiProgramID, 0, "vertexPosition");
		std::string sUVStr = "vertexUV";
		for (int iTextureIdx = 0; iTextureIdx < iNumOfTextureUnits; ++iTextureIdx)
		{
			glBindAttribLocation(uiProgramID, 1 + iTextureIdx, (sUVStr + std::to_string(iTextureIdx)).c_str());
		}
	}

	glLinkProgram(uiProgramID);

	// Check the program
	glGetProgramiv(uiProgramID, GL_LINK_STATUS, &iResult);
	if (GL_FALSE == iResult)
	{
		glGetProgramiv(uiProgramID, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		iInfoLogLength = (iInfoLogLength < 0) ? 0 : iInfoLogLength;

		std::vector<char> vProgramErrorMessage(iInfoLogLength + 1);
		glGetProgramInfoLog(uiProgramID, iInfoLogLength, NULL, &vProgramErrorMessage[0]);
		GLOGGER(error) << "Program: " << std::string(vProgramErrorMessage.data());
		return false;
	}

	glDeleteShader(uiVertexShaderID);
	glDeleteShader(uiFragmentShaderID);
	return true;
}


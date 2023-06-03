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
 
 
 
#include "GRenderScene.h"

#include "GProjectorObjects.h"
#include "GGLSLProgramGenerator.h"
#include "GGlobalParametersManager.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"


template<class tProjectorObject>
void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<tProjectorObject>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches)
{
	assert(0 == vVerticeIndices.size() % 3);

	mpPatches.clear();

	for (int i = 0; i < vVerticeIndices.size() / 3; ++i)
	{
		std::bitset<MAX_PATCHES_NUM> bsTexture;
		bsTexture.reset();

		tVertex u1 = vVertices[vVerticeIndices[3 * i]];
		tVertex u2 = vVertices[vVerticeIndices[3 * i + 1]];
		tVertex u3 = vVertices[vVerticeIndices[3 * i + 2]];

		float fTotalWeight1 = 0.0f;
		float fTotalWeight2 = 0.0f;
		float fTotalWeight3 = 0.0f;

		std::vector<tTextureCoordinate> vTextureSrcPoints(vProjectorObjs.size() * 3);

		for (int j = 0; j < vProjectorObjs.size(); ++j)
		{
			cv::Point3f ptRet1;
			cv::Point3f ptRet2;
			cv::Point3f ptRet3;

			ptRet1 = vProjectorObjs[j](cv::Point3f(u1.x, u1.y, u1.z));
			ptRet2 = vProjectorObjs[j](cv::Point3f(u2.x, u2.y, u2.z));
			ptRet3 = vProjectorObjs[j](cv::Point3f(u3.x, u3.y, u3.z));

			int vFlags[3] = { 0.0f < ptRet1.x && 0.0f < ptRet1.y && ptRet1.x < 1.0f && ptRet1.y < 1.0f
				, 0.0f < ptRet2.x && 0.0f < ptRet2.y && ptRet2.x < 1.0f && ptRet2.y < 1.0f
				, 0.0f < ptRet3.x && 0.0f < ptRet3.y && ptRet3.x < 1.0f && ptRet3.y < 1.0f };
			if (vFlags[0] || vFlags[1] || vFlags[2])
			{
				assert(j < MAX_PATCHES_NUM);//todo.
				bsTexture.set(j);

				ptRet1.x = (ptRet1.x != ptRet1.x) ? FLT_MAX : ptRet1.x;
				ptRet1.y = (ptRet1.y != ptRet1.y) ? FLT_MAX : ptRet1.y;
				ptRet2.x = (ptRet2.x != ptRet2.x) ? FLT_MAX : ptRet2.x;
				ptRet2.y = (ptRet2.y != ptRet2.y) ? FLT_MAX : ptRet2.y;
				ptRet3.x = (ptRet3.x != ptRet3.x) ? FLT_MAX : ptRet3.x;
				ptRet3.y = (ptRet3.y != ptRet3.y) ? FLT_MAX : ptRet3.y;

				float fBlenderStrength = 20.0f;
				float fWeight1 = (vFlags[0]) ? powf(ptRet1.z, fBlenderStrength) : (0.0f);
				float fWeight2 = (vFlags[1]) ? powf(ptRet2.z, fBlenderStrength) : (0.0f);
				float fWeight3 = (vFlags[2]) ? powf(ptRet3.z, fBlenderStrength) : (0.0f);

				vTextureSrcPoints[3 * j] = tTextureCoordinate(ptRet1.x, ptRet1.y, fWeight1);
				vTextureSrcPoints[3 * j + 1] = tTextureCoordinate(ptRet2.x, ptRet2.y, fWeight2);
				vTextureSrcPoints[3 * j + 2] = tTextureCoordinate(ptRet3.x, ptRet3.y, fWeight3);

				fTotalWeight1 += fWeight1;
				fTotalWeight2 += fWeight2;
				fTotalWeight3 += fWeight3;

				//check max texture units.
				if (bsTexture.count() >= MAX_PATCHES_NUM)
				{
					break;
				}
			}
		}
		GScenePatch& patch = mpPatches[bsTexture];
		patch.vVertices.push_back(u1);
		patch.vVertices.push_back(u2);
		patch.vVertices.push_back(u3);
		patch.vVerticeIndices.push_back(patch.vVertices.size() - 3);
		patch.vVerticeIndices.push_back(patch.vVertices.size() - 2);
		patch.vVerticeIndices.push_back(patch.vVertices.size() - 1);

		if (patch.vMultiTextureCoordinates.empty())
		{
			patch.vMultiTextureCoordinates.resize(bsTexture.count());
		}

		int iTextureIdx = 0;
		for (int k = 0; k < bsTexture.size(); ++k)
		{
			if (bsTexture[k])
			{
				tTextureCoordinate textureCoordinate1 = vTextureSrcPoints[k * 3];
				tTextureCoordinate textureCoordinate2 = vTextureSrcPoints[k * 3 + 1];
				tTextureCoordinate textureCoordinate3 = vTextureSrcPoints[k * 3 + 2];

				textureCoordinate1.weight = (fTotalWeight1 == 0) ? 0.0f : textureCoordinate1.weight / fTotalWeight1;
				textureCoordinate2.weight = (fTotalWeight2 == 0) ? 0.0f : textureCoordinate2.weight / fTotalWeight2;
				textureCoordinate3.weight = (fTotalWeight3 == 0) ? 0.0f : textureCoordinate3.weight / fTotalWeight3;

				patch.vMultiTextureCoordinates[iTextureIdx].first = vProjectorObjs[k].GetCameraIdx();
				patch.vMultiTextureCoordinates[iTextureIdx].second.push_back(textureCoordinate1);
				patch.vMultiTextureCoordinates[iTextureIdx].second.push_back(textureCoordinate2);
				patch.vMultiTextureCoordinates[iTextureIdx].second.push_back(textureCoordinate3);

				++iTextureIdx;
			}
		}
	}
}

template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectGlobal3d2NormalizedPix>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix180>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);
template void GPatchesGenerator::GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
	, const std::vector<GProjectorObjectOriginal3d2NormalizedPix>& vProjectorObjs
	, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);

float GPatchesGenerator::CalcWeight(const cv::Point2f& ptPosition, const cv::Mat& mK, float fDistortionValidRange)
{
	float fRet = 0.0;
	float fFocalX = static_cast<float>(mK.at<double>(0, 0));
	float fFocalY = static_cast<float>(mK.at<double>(1, 1));

	float fTempX = 0.0;
	float fTempY = 0.0;

	fTempX = (ptPosition.x >= 0.5) ?
		((1.0 - ptPosition.x) / fFocalX) : (ptPosition.x / fFocalX);
	fTempY = (ptPosition.y >= 0.5) ?
		((1.0 - ptPosition.y) / fFocalY) : (ptPosition.y / fFocalY);

	fRet = (fTempX < fTempY) ? fTempX : fTempY;

	float fXCenter = (ptPosition.x - static_cast<float>(mK.at<double>(0, 2))) / fFocalX;
	float fYCenter = (ptPosition.y - static_cast<float>(mK.at<double>(1, 2))) / fFocalY;
	float fTempR = fDistortionValidRange - sqrtf(fXCenter * fXCenter + fYCenter * fYCenter);

	fRet = (fTempR < fRet) ? fTempR : fRet;
	return (fRet <= 0.0) ? 0.0 : pow(fRet, 5.0);
}

void GScenePatch::SetUpVertexAttributes()
{
	m_vVBOs.resize(2 + vMultiTextureCoordinates.size());
	glGenBuffers(m_vVBOs.size(), &m_vVBOs[0]);

	glBindBuffer(GL_ARRAY_BUFFER, m_vVBOs[eVBOTypeVertices]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tVertex)*vVertices.size(), &(vVertices[0]), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nullptr));

	for (int i = 0; i < vMultiTextureCoordinates.size(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vVBOs[eVBOTypeTexture + i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tTextureCoordinate)*vMultiTextureCoordinates[i].second.size(), &(vMultiTextureCoordinates[i].second[0]), GL_STATIC_DRAW);
		glVertexAttribPointer(1 + i, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nullptr));
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vVBOs[eVBOTypeIndices]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*vVerticeIndices.size(), &(vVerticeIndices[0]), GL_STATIC_DRAW);
}

bool GScenePatch::Prepare(enFrameFormat eFrameFormat)
{
	if (vMultiTextureCoordinates.empty())
	{
		return true;
	}

	GGLSLBlenderGenerator::enGFragShaderType eFragShaderType = GGLSLBlenderGenerator::eGFragShaderTypeNormal;
	if (eFrameFormatRGB == eFrameFormat)
	{
		eFragShaderType = GGLSLBlenderGenerator::eGFragShaderTypeNormal;
	}
	else if (eFrameFormatYUV420P == eFrameFormat)
	{
		eFragShaderType = GGLSLBlenderGenerator::eGFragShaderTypeYUV;
	}
	else if (eFrameFormatNV12 == eFrameFormat)
	{
		eFragShaderType = GGLSLBlenderGenerator::eGFragShaderTypeNV12;
	}
	else
	{
		GLOGGER(error) << "GScenePatch::Prepare(). frame format not supported: " << eFrameFormat;
		return false;
	}

	if (!GGLSLBlenderGenerator::GetProgram(GGLSLBlenderGenerator::eGVertexShaderTypeNormal, eFragShaderType
		, GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGLSLMajorVersion) * 100 + GGlobalParametersManager::GetInstance()->GetParamInt(eGGlobalParamIntGLSLMinorVersion)
		, vMultiTextureCoordinates.size(), m_uiBlender))
	{
		GLOGGER(error) << "GScenePatch::Prepare(). prepare failed.";
		return false;
	}

	if (opengl_compatible(3, 0))
	{
		glGenVertexArrays(1, &m_uiVAO);
		glBindVertexArray(m_uiVAO);
	}

	glEnableVertexAttribArray(0);
	for (int i = 0; i < vMultiTextureCoordinates.size(); ++i)
	{
		glEnableVertexAttribArray(1 + i);
	}

	SetUpVertexAttributes();

	if (opengl_compatible(3, 0))
	{
		glBindVertexArray(0);
	}

	return true;
}

bool GScenePatch::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	if (vMultiTextureCoordinates.empty())
	{
		return true;
	}

	glUseProgram(m_uiBlender);

	glm::mat4 glmMVP = matP * matV * glm::mat4();
	GLuint uiMatrixID = glGetUniformLocation(m_uiBlender, "MVP");
	glUniformMatrix4fv(uiMatrixID, 1, GL_FALSE, &glmMVP[0][0]);

	if (eFrameFormatRGB == textureGroup.eTextureFormat)
	{
		for (int iOverlapLayerIdx = 0; iOverlapLayerIdx < vMultiTextureCoordinates.size(); ++iOverlapLayerIdx)
		{
			std::string sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx);
			GLuint TextureID = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());

			int iCameraIndex = vMultiTextureCoordinates[iOverlapLayerIdx].first;
			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIndex]);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);

			glUniform1i(TextureID, iOverlapLayerIdx);
		}
	}
	else if (eFrameFormatYUV420P == textureGroup.eTextureFormat)
	{
		for (int iOverlapLayerIdx = 0; iOverlapLayerIdx < vMultiTextureCoordinates.size(); ++iOverlapLayerIdx)
		{
			int iCameraIdx = vMultiTextureCoordinates[iOverlapLayerIdx].first;

			std::string sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx) + std::to_string(0);
			GLuint uiSamplerIdx = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());

			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx * 3);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIdx * 3]);
			glUniform1i(uiSamplerIdx, iOverlapLayerIdx * 3);

			sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx) + std::to_string(1);
			uiSamplerIdx = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());
			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx * 3 + 1);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIdx * 3 + 1]);
			glUniform1i(uiSamplerIdx, iOverlapLayerIdx * 3 + 1);

			sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx) + std::to_string(2);
			uiSamplerIdx = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());
			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx * 3 + 2);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIdx * 3 + 2]);
			glUniform1i(uiSamplerIdx, iOverlapLayerIdx * 3 + 2);
		}
	}
	else if (eFrameFormatNV12 == textureGroup.eTextureFormat)
	{
		for (int iOverlapLayerIdx = 0; iOverlapLayerIdx < vMultiTextureCoordinates.size(); ++iOverlapLayerIdx)
		{
			int iCameraIdx = vMultiTextureCoordinates[iOverlapLayerIdx].first;

			std::string sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx) + std::to_string(0);
			GLuint uiSamplerIdx = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());

			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx * 2);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIdx * 2]);
			glUniform1i(uiSamplerIdx, iOverlapLayerIdx * 2);

			sTextureSampler = "textureSampler" + std::to_string(iOverlapLayerIdx) + std::to_string(1);
			uiSamplerIdx = glGetUniformLocation(m_uiBlender, sTextureSampler.c_str());
			glActiveTexture(GL_TEXTURE0 + iOverlapLayerIdx * 2 + 1);
			glBindTexture(GL_TEXTURE_2D, textureGroup.vTextureGroup[iCameraIdx * 2 + 1]);
			glUniform1i(uiSamplerIdx, iOverlapLayerIdx * 2 + 1);
		}
	}

	//Draw.

	if (opengl_compatible(3, 0))
	{
		glBindVertexArray(m_uiVAO);
	}
	else
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vVBOs[eVBOTypeVertices]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nullptr));

		for (int i = 0; i < vMultiTextureCoordinates.size(); ++i)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_vVBOs[eVBOTypeTexture + i]);
			glVertexAttribPointer(1 + i, 3, GL_FLOAT, GL_FALSE, 0, (void*)(nullptr));
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vVBOs[eVBOTypeIndices]);
	}

	glDrawElements(GL_TRIANGLES, vVerticeIndices.size(), GL_UNSIGNED_INT, (void*)(nullptr));

	if (opengl_compatible(3, 0))
	{
		glBindVertexArray(0);
	}

	glUseProgram(0);

	return true;
}

void GScenePatch::Release()
{
	if (vMultiTextureCoordinates.empty())
	{
		return;
	}

	if (opengl_compatible(3, 0))
	{
		glDeleteVertexArrays(1, &m_uiVAO);
	}

	if (!m_vVBOs.empty())
	{
		glDeleteBuffers(m_vVBOs.size(), &(m_vVBOs[0]));
	}

	GLint iNumOfShaders = 0;
	if (glIsProgram(m_uiBlender))
	{
		glGetProgramiv(m_uiBlender, GL_ATTACHED_SHADERS, &iNumOfShaders);
		if (iNumOfShaders > 0)
		{
			std::vector<GLuint> vShaders(iNumOfShaders);
			glGetAttachedShaders(m_uiBlender, vShaders.size(), nullptr, &(vShaders[0]));
			for (int iShaderIdx = 0; iShaderIdx < vShaders.size(); ++iShaderIdx)
			{
				glDeleteShader(vShaders[iShaderIdx]);
			}
		}

		glDeleteProgram(m_uiBlender);
	}
}

template<class tProjectorObject>
bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator, const std::vector<tProjectorObject>& vProjectorObjs)
{
	//GLOGGER(info) << "==> GRenderScene::Create().";

	int iMaxTextureUnits = 0;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &iMaxTextureUnits);
	GLOGGER(info) << "Max Texture Units: " << iMaxTextureUnits;//todo.

	ptrSurfaceGenerator->GenerateSurface(m_vVertices, m_vVerticeIndices);
	GPatchesGenerator::GeneratePatches(m_vVertices, m_vVerticeIndices, vProjectorObjs, m_mpScenePatches);

	std::vector<tVertex> vTempVertices;
	std::vector<GLuint> vTempVerticeIndices;
	m_vVertices.swap(vTempVertices);
	m_vVerticeIndices.swap(vTempVerticeIndices);

	//GLOGGER(info) << "GRenderScene::Create() ==>";
	return true;
}

template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectGlobal3d2NormalizedPix>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectCylinderExpandedPlane3d2NormalizedPix>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectCylinderExpandedPlaneSplited3d2NormalizedPix>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectCylinderExpandedPlaneSplitedFrontBack3d2NormalizedPix>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix180>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectSphereExpandedPlane3d2NormalizedPix360CircularBinocular>& vProjectorObjs);
template bool GRenderScene::Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator
	, const std::vector<GProjectorObjectOriginal3d2NormalizedPix>& vProjectorObjs);


bool GRenderScene::Prepare(enFrameFormat eFrameFormat)
{
	//GLOGGER(info) << "==> GRenderScene::Prepare().";
	for (auto it = m_mpScenePatches.begin(); it != m_mpScenePatches.end(); ++it)
	{
		if (!it->second.Prepare(eFrameFormat))
		{
			GLOGGER(error) << "GRenderScene::Prepare(). patch prepare failed.";
			GLOGGER(info) << "GRenderScene::Prepare()==> ";
			return false;
		}
	}
	//GLOGGER(info) << "GRenderScene::Prepare() ==> ";
	return true;
}

bool GRenderScene::Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV)
{
	for (auto it = m_mpScenePatches.begin(); it != m_mpScenePatches.end(); ++it)
	{
		it->second.Render(textureGroup, matP, matV);
	}
	return true;
}

void GRenderScene::Release()
{
	for (auto it = m_mpScenePatches.begin(); it != m_mpScenePatches.end(); ++it)
	{
		it->second.Release();
	}
}
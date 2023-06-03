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
 
 
 
#ifndef G_RENDER_SCENE_H_
#define G_RENDER_SCENE_H_

#include <vector>
#include <map>
#include <bitset>
#include <memory>

#include "GCameraModel.h"
#include "PanoRender/GPanoRenderCommonIF.h"
#include "PanoRender/GSurfaceGeneratorIF.h"

#define MAX_PATCHES_NUM 64

class CameraModel;
struct tVertex;

struct tTextureCoordinate
{
	tTextureCoordinate()
	{}
	tTextureCoordinate(float fu, float fv, float fWeight) :
		u(fu)
		, v(fv)
		, weight(fWeight)
	{}

	float u;
	float v;
	float weight;
};

class BitsetLessThan
{
public:
	bool operator()(const std::bitset<MAX_PATCHES_NUM>& x, const std::bitset<MAX_PATCHES_NUM>& y) const
	{
		return x.to_ullong() < y.to_ullong();
	}
};

class GScenePatch
{
public:
	GScenePatch():
		m_uiVAO(0)
		, m_uiBlender(0)
	{}

	enum enVBOType
	{
		eVBOTypeVertices = 0,
		eVBOTypeIndices,
		eVBOTypeTexture,
	};

	std::vector<tVertex> vVertices;
	std::vector<GLuint> vVerticeIndices;
	std::vector<std::pair<int, std::vector<tTextureCoordinate>>> vMultiTextureCoordinates;

public:
	bool Prepare(enFrameFormat eFrameFormat);
	bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);
	void Release();
private:
	void SetUpVertexAttributes();
private:
	std::vector<GLuint> m_vVBOs;
	GLuint m_uiVAO;
	GLuint m_uiBlender;
};

class GPatchesGenerator
{
public:
	template<class tProjectorObject>
	static void GeneratePatches(const std::vector<tVertex>& vVertices, const std::vector<GLuint>& vVerticeIndices
		, const std::vector<tProjectorObject>& vProjectorObjs
		, std::map < std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan>& mpPatches);

private:
	static float CalcWeight(const cv::Point2f& ptPosition, const cv::Mat& mK, float fDistortionValidRange);
};

class GRenderScene
{
public:
	template<class tProjectorObject>
	bool Create(std::shared_ptr<GSurfaceGenerator4RenderIF> ptrSurfaceGenerator, const std::vector<tProjectorObject>& vProjectorObjs);

	bool Prepare(enFrameFormat eFrameFormat);

	bool Render(const GTextureGroup& textureGroup, const glm::mat4& matP, const glm::mat4& matV);

	void Release();
private:
	std::vector<tVertex> m_vVertices;
	std::vector<GLuint> m_vVerticeIndices;

	std::map< std::bitset<MAX_PATCHES_NUM>, GScenePatch, BitsetLessThan> m_mpScenePatches;
};




#endif//!G_RENDER_SCENE_H_
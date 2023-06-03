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
 
 
 
#include "GSurfaceGenerator.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"


void GSurfaceGeneratorSphere::GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	vVertices.clear();
	vVerticeIndices.clear();

	float vertices0[6][3] = { { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
	, { -1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } };

	int iDep = 7;
	//clock wise watch from outside.
	SubDivide(Eigen::Vector3f(vertices0[0]), Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[2]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[3]), Eigen::Vector3f(vertices0[2]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[3]), Eigen::Vector3f(vertices0[4]), Eigen::Vector3f(vertices0[2]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[4]), Eigen::Vector3f(vertices0[0]), Eigen::Vector3f(vertices0[2]), iDep, vVertices, vVerticeIndices);
	if (m_bIsFullSphere)
	{
		SubDivide(Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[0]), iDep, vVertices, vVerticeIndices);
		SubDivide(Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[3]), Eigen::Vector3f(vertices0[1]), iDep, vVertices, vVerticeIndices);
		SubDivide(Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[4]), Eigen::Vector3f(vertices0[3]), iDep, vVertices, vVerticeIndices);
		SubDivide(Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[0]), Eigen::Vector3f(vertices0[4]), iDep, vVertices, vVerticeIndices);
	}
}

void GSurfaceGeneratorSphere::SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, int iDepth
	, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	Eigen::Vector3f u1 = vu1;
	u1 /= u1.norm();
	Eigen::Vector3f u2 = vu2;
	u2 /= u2.norm();
	Eigen::Vector3f u3 = vu3;
	u3 /= u3.norm();

	if (iDepth == 0)
	{
		vVertices.push_back(tVertex(u1[0], u1[1], u1[2]));
		vVertices.push_back(tVertex(u2[0], u2[1], u2[2]));
		vVertices.push_back(tVertex(u3[0], u3[1], u3[2]));

		vVerticeIndices.push_back(vVertices.size() - 3);
		vVerticeIndices.push_back(vVertices.size() - 2);
		vVerticeIndices.push_back(vVertices.size() - 1);

		return;
	}

	Eigen::Vector3f u12, u23, u31;
	u12 = (u1 + u2) / 2.0f;
	u23 = (u2 + u3) / 2.0f;
	u31 = (u3 + u1) / 2.0f;

	u12 /= u12.norm();
	u23 /= u23.norm();
	u31 /= u31.norm();

	//clock wise subdivide.
	SubDivide(u1, u12, u31, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u2, u23, u12, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u3, u31, u23, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u12, u23, u31, iDepth - 1, vVertices, vVerticeIndices);
}

void GSurfaceGeneratorSphere::SetIsFullSphere(bool bIsFullSphere)
{
	m_bIsFullSphere = bIsFullSphere;
}

void GSurfaceGeneratorPlane::GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	vVertices.clear();
	vVerticeIndices.clear();

	float vertices0[4][3] = { { -m_fWidth / 2.0f, -m_fHeight / 2.0f, 0.0f }, { m_fWidth / 2.0f, -m_fHeight / 2.0f, 0.0f }, { m_fWidth / 2.0f, m_fHeight / 2.0f, 0.0f }, { -m_fWidth / 2.0f, m_fHeight / 2.0f, 0.0f } };

	int iDep = 7;
	SubDivide(Eigen::Vector3f(vertices0[0]), Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[2]), Eigen::Vector3f(vertices0[3]), iDep, vVertices, vVerticeIndices);
}

void GSurfaceGeneratorPlane::SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, const Eigen::Vector3f& vu4
	, int iDepth, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	if (iDepth == 0)
	{
		TriangleDivide(vu1, vu2, vu3, vVertices, vVerticeIndices);
		TriangleDivide(vu3, vu4, vu1, vVertices, vVerticeIndices);
		return;
	}

	Eigen::Vector3f u12, u23, u34, u41, u31;
	u12 = (vu1 + vu2) / 2.0f;
	u23 = (vu2 + vu3) / 2.0f;
	u34 = (vu3 + vu4) / 2.0f;
	u41 = (vu4 + vu1) / 2.0f;
	u31 = (vu3 + vu1) / 2.0f;

	SubDivide(vu1, u12, u31, u41, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(vu2, u23, u31, u12, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(vu3, u34, u31, u23, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(vu4, u41, u31, u34, iDepth - 1, vVertices, vVerticeIndices);
}

void GSurfaceGeneratorPlane::TriangleDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3
	, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	vVertices.push_back(tVertex(vu1[0], vu1[1], vu1[2]));
	vVertices.push_back(tVertex(vu2[0], vu2[1], vu2[2]));
	vVertices.push_back(tVertex(vu3[0], vu3[1], vu3[2]));

	vVerticeIndices.push_back(vVertices.size() - 3);
	vVerticeIndices.push_back(vVertices.size() - 2);
	vVerticeIndices.push_back(vVertices.size() - 1);
}

void GSurfaceGeneratorPlane::SetSurfaceSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

void GSurfaceGeneratorCylinder::GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	vVertices.clear();
	vVerticeIndices.clear();

	float vertices0[8][3] = { { 1.0f, -m_fHeight / 2.0f, 0.0f }, { 1.0f, m_fHeight / 2.0f, 0.0f }, { 0.0f, m_fHeight / 2.0f, 1.0f }, { 0.0f, -m_fHeight / 2.0f, 1.0f }
	, { -1.0f, -m_fHeight / 2.0f, 0.0f }, { -1.0f, m_fHeight / 2.0f, 0.0f }, { 0.0f, m_fHeight / 2.0f, -1.0f }, { 0.0f, -m_fHeight / 2.0f, -1.0f } };

	int iDep = 7;
	SubDivide(Eigen::Vector3f(vertices0[0]), Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[2]), Eigen::Vector3f(vertices0[3]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[3]), Eigen::Vector3f(vertices0[2]), Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[4]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[4]), Eigen::Vector3f(vertices0[5]), Eigen::Vector3f(vertices0[6]), Eigen::Vector3f(vertices0[7]), iDep, vVertices, vVerticeIndices);
	SubDivide(Eigen::Vector3f(vertices0[7]), Eigen::Vector3f(vertices0[6]), Eigen::Vector3f(vertices0[1]), Eigen::Vector3f(vertices0[0]), iDep, vVertices, vVerticeIndices);
}

void GSurfaceGeneratorCylinder::SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, const Eigen::Vector3f& vu4
	, int iDepth, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	Eigen::Vector3f u1 = vu1;
	float fDistance1 = sqrtf(powf(u1[0], 2) + powf(u1[2], 2));
	u1[0] /= fDistance1;
	u1[2] /= fDistance1;
	Eigen::Vector3f u2 = vu2;
	float fDistance2 = sqrtf(powf(u2[0], 2) + powf(u2[2], 2));
	u2[0] /= fDistance2;
	u2[2] /= fDistance2;
	Eigen::Vector3f u3 = vu3;
	float fDistance3 = sqrtf(powf(u3[0], 2) + powf(u3[2], 2));
	u3[0] /= fDistance3;
	u3[2] /= fDistance3;
	Eigen::Vector3f u4 = vu4;
	float fDistance4 = sqrtf(powf(u4[0], 2) + powf(u4[2], 2));
	u4[0] /= fDistance4;
	u4[2] /= fDistance4;

	if (iDepth == 0)
	{
		TriangleDivide(u1, u2, u3, vVertices, vVerticeIndices);
		TriangleDivide(u3, u4, u1, vVertices, vVerticeIndices);
		return;
	}

	Eigen::Vector3f u12, u23, u34, u41, u31;
	u12 = (u1 + u2) / 2.0f;
	float fDistance12 = sqrtf(powf(u12[0], 2) + powf(u12[2], 2));
	u12[0] /= fDistance12;
	u12[2] /= fDistance12;

	u23 = (u2 + u3) / 2.0f;
	float fDistance23 = sqrtf(powf(u23[0], 2) + powf(u23[2], 2));
	u23[0] /= fDistance23;
	u23[2] /= fDistance23;

	u34 = (u3 + u4) / 2.0f;
	float fDistance34 = sqrtf(powf(u34[0], 2) + powf(u34[2], 2));
	u34[0] /= fDistance34;
	u34[2] /= fDistance34;

	u41 = (u4 + u1) / 2.0f;
	float fDistance41 = sqrtf(powf(u41[0], 2) + powf(u41[2], 2));
	u41[0] /= fDistance41;
	u41[2] /= fDistance41;

	u31 = (u3 + u1) / 2.0f;
	float fDistance31 = sqrtf(powf(u31[0], 2) + powf(u31[2], 2));
	u31[0] /= fDistance31;
	u31[2] /= fDistance31;

	SubDivide(u1, u12, u31, u41, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u2, u23, u31, u12, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u3, u34, u31, u23, iDepth - 1, vVertices, vVerticeIndices);
	SubDivide(u4, u41, u31, u34, iDepth - 1, vVertices, vVerticeIndices);
}

void GSurfaceGeneratorCylinder::TriangleDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3
	, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices)
{
	vVertices.push_back(tVertex(vu1[0], vu1[1], vu1[2]));
	vVertices.push_back(tVertex(vu2[0], vu2[1], vu2[2]));
	vVertices.push_back(tVertex(vu3[0], vu3[1], vu3[2]));

	vVerticeIndices.push_back(vVertices.size() - 3);
	vVerticeIndices.push_back(vVertices.size() - 2);
	vVerticeIndices.push_back(vVertices.size() - 1);
}

void GSurfaceGeneratorCylinder::SetSurfaceSize(float fHeight)
{
	m_fHeight = fHeight;
}

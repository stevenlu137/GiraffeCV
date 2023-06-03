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
 
 
 
#ifndef G_SURFACE_GENERATOR_H_
#define G_SURFACE_GENERATOR_H_

#include <vector>

#include <Eigen/Geometry>

#include "PanoRender/GSurfaceGeneratorIF.h"


class GSurfaceGeneratorSphere : public GSurfaceGenerator4RenderIF
{
public:
	GSurfaceGeneratorSphere() :
		m_bIsFullSphere(false)
	{}

	virtual ~GSurfaceGeneratorSphere(){};
	virtual void GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);

public:
	void SetIsFullSphere(bool bIsFullSphere);

private:
	void SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, int iDepth
		, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);

	bool m_bIsFullSphere;
};

class GSurfaceGeneratorPlane : public GSurfaceGenerator4RenderIF
{
public:
	GSurfaceGeneratorPlane() :
		m_fWidth(0.0)
		, m_fHeight(0.0)
	{}

	virtual ~GSurfaceGeneratorPlane(){};
	virtual void GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);
	
public:
	void SetSurfaceSize(float fWidth,float fHeight);

private:
	void SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, const Eigen::Vector3f& vu4
		, int iDepth, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);
	void TriangleDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3
		, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);

	float m_fWidth;
	float m_fHeight;
};

class GSurfaceGeneratorCylinder : public GSurfaceGenerator4RenderIF
{
public:
	GSurfaceGeneratorCylinder() :
		m_fHeight(0.0)
	{}

	virtual ~GSurfaceGeneratorCylinder(){};
	virtual void GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);

public:
	void SetSurfaceSize(float fHeight);

private:
	void SubDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3, const Eigen::Vector3f& vu4
		, int iDepth, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);
	void TriangleDivide(const Eigen::Vector3f& vu1, const Eigen::Vector3f& vu2, const Eigen::Vector3f& vu3
		, std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices);

	float m_fHeight;
};
#endif//!G_SURFACE_GENERATOR_H_
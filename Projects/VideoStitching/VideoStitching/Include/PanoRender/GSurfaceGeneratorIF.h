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
 
 
 
#ifndef G_SURFACE_GENERATOR_IF_H_
#define G_SURFACE_GENERATOR_IF_H_

#include <vector>

#include <Eigen/Dense>

#include "GGLHeaders.h"

struct tVertex
{
	tVertex(float fx, float fy, float fz) :
		x(fx)
		, y(fy)
		, z(fz)
	{}

	Eigen::Vector3d ToEigenVector()
	{
		return Eigen::Vector3d(x, y, z);
	}

	float x;
	float y;
	float z;
};

class GSurfaceGenerator4RenderIF
{
public:
	virtual ~GSurfaceGenerator4RenderIF(){};
	virtual void GenerateSurface(std::vector<tVertex>& vVertices, std::vector<GLuint>& vVerticeIndices) = 0;
};





#endif // !G_SURFACE_GENERATOR_IF_H_

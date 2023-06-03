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
 
 
 
#ifndef G_VIEW_CAMERA_H_
#define G_VIEW_CAMERA_H_

#include <vector>
#include <memory>

#include <Eigen/Dense>

#include <glm/gtc/matrix_transform.hpp>

#include "PanoRender/GPanoRenderCommonIF.h"


class GViewCameraPerspective : public GViewCamera4RenderIF
{
public:
	GLfloat gdEyeX;
	GLfloat gdEyeY;
	GLfloat gdEyeZ;
	GLfloat gdCenterX;
	GLfloat gdCenterY;
	GLfloat gdCenterZ;
	GLfloat gdUpX;
	GLfloat gdUpY;
	GLfloat gdUpZ;

	float fViewFieldH_Degree;
	float fWHRatio;

public:
	virtual ~GViewCameraPerspective(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

public:
	enum enViewCameraAxis
	{
		eViewCameraAxisEx,
		eViewCameraAxisUp,
		eViewCameraAxisView,
	};

	GViewCameraPerspective(){};

	GViewCameraPerspective(GLfloat eyeX, GLfloat eyeY, GLfloat eyeZ,
		GLfloat centerX, GLfloat centerY, GLfloat centerZ,
		GLfloat upX, GLfloat upY, GLfloat upZ,
		float viewFieldH_Degree, int width, int height) :
		gdEyeX(eyeX)
		, gdEyeY(eyeY)
		, gdEyeZ(eyeZ)
		, gdCenterX(centerX)
		, gdCenterY(centerY)
		, gdCenterZ(centerZ)
		, gdUpX(upX)
		, gdUpY(upY)
		, gdUpZ(upZ)
		, fViewFieldH_Degree(viewFieldH_Degree)
		, fWHRatio(0.0)
	{}

	void SetEyeLocation(const Eigen::Vector3f& v3fEyeLocation);
	Eigen::Vector3f GetEyeLocation() const;
	void SetViewPoint(const Eigen::Vector3f& v3fViewPoint);
	Eigen::Vector3f GetViewPoint() const;
	void SetUpVector(const Eigen::Vector3f& v3fUpVector);
	Eigen::Vector3f GetUpVector() const;
	void SetFOV_Degree(float viewFieldH_Degree);
	float GetViewFOV_Degree() const;
	void SetWHRatio(float whRatio){ fWHRatio = whRatio; }
	float GetWHRatio(){ return fWHRatio; }
	float GetPan() const;
	float GetTilt() const;
	void SetPan(float fPan);
	void SetTilt(float fTilt);
	void SetPT(float fPan, float fTilt);

	Eigen::Matrix3f GetFrameMatrix() const;

	void AxisRotateTo(enViewCameraAxis eAxis, const Eigen::Vector3f& v3fNewAxis, bool bFixEyeLocation);

private:
	Eigen::Vector3f GetReferenceEyeLocVector() const;
	Eigen::Vector3f GetReferenceViewPiontVector() const;
	Eigen::Vector3f GetReferenceUpVector() const;
};

class GViewCameraPerspectiveOutside : public GViewCamera4RenderIF
{
public:
	GViewCameraPerspectiveOutside() :m_fEyeDistance(0.0)
		, m_fPan(0.0)
		, m_fTilt(0.0)
		, m_fViewFieldH_Degree(0.0)
		, m_fWHRatio(0.0)
	{}

	virtual ~GViewCameraPerspectiveOutside(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

	void SetEyeLocation(float fEyeDistance) { m_fEyeDistance = fEyeDistance; };
	void SetFOV_Degree(float viewFieldH_Degree){ m_fViewFieldH_Degree = viewFieldH_Degree; };
	float GetViewFOV_Degree() const { return m_fViewFieldH_Degree; };
	void SetWHRatio(float whRatio){ m_fWHRatio = whRatio; }
	float GetWHRatio() const { return m_fWHRatio; }
	float GetPan() const { return m_fPan; };
	float GetTilt() const { return m_fTilt; };

	void SetPan(float fPan) { m_fPan = fPan; };
	void SetTilt(float fTilt){ m_fTilt = fTilt; };
private:
	float m_fEyeDistance;

	float m_fPan;
	float m_fTilt;
	float m_fViewFieldH_Degree;
	float m_fWHRatio;
};

class GViewCameraPerspectiveInside : public GViewCamera4RenderIF
{
public:
	GViewCameraPerspectiveInside() :m_fPan(0.0)
		, m_fTilt(0.0)
		, m_fViewFieldH_Degree(0.0)
		, m_fWHRatio(0.0)
	{}

	virtual ~GViewCameraPerspectiveInside(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

	void SetFOV_Degree(float viewFieldH_Degree){ m_fViewFieldH_Degree = viewFieldH_Degree; };
	float GetViewFOV_Degree() const { return m_fViewFieldH_Degree; };
	void SetWHRatio(float whRatio){ m_fWHRatio = whRatio; }
	float GetWHRatio() const { return m_fWHRatio; }
	float GetPan() const { return m_fPan; };
	float GetTilt() const { return m_fTilt; };

	void SetPan(float fPan) { m_fPan = fPan; };
	void SetTilt(float fTilt){ m_fTilt = fTilt; };
private:
	float m_fPan;
	float m_fTilt;
	float m_fViewFieldH_Degree;
	float m_fWHRatio;
};

class GViewCameraPerspectiveOutsideSurround : public GViewCamera4RenderIF
{
public:
	GViewCameraPerspectiveOutsideSurround() :m_fEyeDistance(0.0)
		, m_fPan(0.0)
		, m_fTilt(0.0)
		, m_fViewFieldH_Degree(0.0)
		, m_fWHRatio(0.0)
	{}

	virtual ~GViewCameraPerspectiveOutsideSurround(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

	void SetEyeLocation(float fEyeDistance) { m_fEyeDistance = fEyeDistance; };
	void SetFOV_Degree(float viewFieldH_Degree){ m_fViewFieldH_Degree = viewFieldH_Degree; };
	float GetViewFOV_Degree() const { return m_fViewFieldH_Degree; };
	void SetWHRatio(float whRatio){ m_fWHRatio = whRatio; }
	float GetWHRatio() const { return m_fWHRatio; }
	float GetPan() const { return m_fPan; };
	float GetTilt() const { return m_fTilt; };

	void SetPan(float fPan) { m_fPan = fPan; };
	void SetTilt(float fTilt){ m_fTilt = fTilt; };
private:
	float m_fEyeDistance;

	float m_fPan;
	float m_fTilt;
	float m_fViewFieldH_Degree;
	float m_fWHRatio;
};

class GViewCameraPerspectiveInsideSurround : public GViewCamera4RenderIF
{
public:
	GViewCameraPerspectiveInsideSurround() :
		m_fPan(0.0)
		, m_fTilt(0.0)
		, m_fViewFieldH_Degree(0.0)
		, m_fWHRatio(0.0)
	{}

	virtual ~GViewCameraPerspectiveInsideSurround(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

	void SetFOV_Degree(float viewFieldH_Degree){ m_fViewFieldH_Degree = viewFieldH_Degree; };
	float GetViewFOV_Degree() const { return m_fViewFieldH_Degree; };
	void SetWHRatio(float whRatio){ m_fWHRatio = whRatio; }
	float GetWHRatio() const { return m_fWHRatio; }
	float GetPan() const { return m_fPan; };
	float GetTilt() const { return m_fTilt; };

	void SetPan(float fPan) { m_fPan = fPan; };
	void SetTilt(float fTilt){ m_fTilt = fTilt; };
private:
	float m_fPan;
	float m_fTilt;
	float m_fViewFieldH_Degree;
	float m_fWHRatio;
};

class GViewCameraOrthogonal : public GViewCamera4RenderIF
{
public:
	GViewCameraOrthogonal():
		m_fX0(0.0)
		, m_fY0(0.0)
		, m_fWidth(0.0)
		, m_fWHRatio(0.0)
	{}

	virtual ~GViewCameraOrthogonal(){};
	virtual glm::mat4 CalcP() const;
	virtual glm::mat4 CalcV() const;

	void SetEyeLocationX(float fX);
	float GetEyeLocationX() const;
	void SetEyeLocationY(float fY);
	float GetEyeLocationY() const;

	void SetWidth(float fWidth);
	float GetWidth() const;

	void SetWHRatio(float fWHRatio);
	float GetWHRatio() const;

private:
	float m_fX0;
	float m_fY0;
	float m_fWidth;
	float m_fWHRatio;
};

#endif // !G_VIEW_CAMERA_H_

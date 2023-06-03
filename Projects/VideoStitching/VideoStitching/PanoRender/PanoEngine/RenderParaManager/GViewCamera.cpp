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
 
 
 
#include "GViewCamera.h"

#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "Common/Math/GMathConstants.h"



void GViewCameraPerspective::SetEyeLocation(const Eigen::Vector3f& v3fEyeLocation)
{
	gdEyeX = v3fEyeLocation[0];
	gdEyeY = v3fEyeLocation[1];
	gdEyeZ = v3fEyeLocation[2];
}

Eigen::Vector3f GViewCameraPerspective::GetEyeLocation() const
{
	return Eigen::Vector3f(gdEyeX, gdEyeY, gdEyeZ);
}

void GViewCameraPerspective::SetViewPoint(const Eigen::Vector3f& v3fViewPoint)
{
	gdCenterX = v3fViewPoint[0];
	gdCenterY = v3fViewPoint[1];
	gdCenterZ = v3fViewPoint[2];
}

Eigen::Vector3f GViewCameraPerspective::GetViewPoint() const
{
	return Eigen::Vector3f(gdCenterX, gdCenterY, gdCenterZ);
}

void GViewCameraPerspective::SetUpVector(const Eigen::Vector3f& v3fUpVector)
{
	gdUpX = v3fUpVector[0];
	gdUpY = v3fUpVector[1];
	gdUpZ = v3fUpVector[2];
}

Eigen::Vector3f GViewCameraPerspective::GetUpVector() const
{
	return Eigen::Vector3f(gdUpX, gdUpY, gdUpZ);
}

void GViewCameraPerspective::SetFOV_Degree(float viewFieldH_Degree)
{
	fViewFieldH_Degree = viewFieldH_Degree;
}

float GViewCameraPerspective::GetViewFOV_Degree() const
{
	return fViewFieldH_Degree;
}

glm::mat4 GViewCameraPerspective::CalcP() const
{
	return glm::perspective(glm::radians(GetViewFOV_Degree()), fWHRatio, 0.5f, 100.0f);
}

glm::mat4 GViewCameraPerspective::CalcV() const
{
	return glm::lookAt(glm::vec3(gdEyeX, gdEyeY, gdEyeZ)
		, glm::vec3(gdCenterX, gdCenterY, gdCenterZ)
		, glm::vec3(gdUpX, gdUpY, gdUpZ));
}

Eigen::Matrix3f GViewCameraPerspective::GetFrameMatrix() const
{
	Eigen::Vector3f v3fEx, v3fEy, v3fEz;
	v3fEy = GetUpVector();
	v3fEz = GetViewPoint() - GetEyeLocation();
	v3fEx = v3fEy.cross(v3fEz);
	v3fEx /= v3fEx.norm();
	v3fEy /= v3fEy.norm();
	v3fEz /= v3fEz.norm();

	Eigen::Matrix3f m3fVecs;
	m3fVecs.block<1, 3>(eViewCameraAxisEx, 0) = v3fEx;
	m3fVecs.block<1, 3>(eViewCameraAxisUp, 0) = v3fEy;
	m3fVecs.block<1, 3>(eViewCameraAxisView, 0) = v3fEz;

	return m3fVecs;
}

void GViewCameraPerspective::AxisRotateTo(GViewCameraPerspective::enViewCameraAxis eAxis, const Eigen::Vector3f& v3fNewAxis, bool bFixEyeLocation)
{
	Eigen::Matrix3f m3fFrame = GetFrameMatrix();

	Eigen::Vector3f v3fRotationAxis = v3fNewAxis.cross(m3fFrame.block<1, 3>(eAxis, 0));
	v3fRotationAxis /= v3fRotationAxis.norm();
	float fTheta = asinf(v3fRotationAxis.norm() / v3fNewAxis.norm());

	Eigen::AngleAxisf Rotate = Eigen::AngleAxisf(fTheta, v3fRotationAxis);

	Eigen::Matrix3f m3fFrameNew = Rotate*m3fFrame;

	SetUpVector(m3fFrameNew.block<1, 3>(eViewCameraAxisUp, 0) * GetUpVector().norm());

	Eigen::Vector3f v3fViewVec = GetViewPoint() - GetEyeLocation();
	Eigen::Vector3f v3fViewVecNew = Rotate * v3fViewVec;

	if (bFixEyeLocation)
	{
		SetViewPoint(GetEyeLocation() + v3fViewVecNew);
	}
	else
	{
		SetEyeLocation(GetViewPoint() - v3fViewVecNew);
	}
}

float GViewCameraPerspective::GetPan() const
{
	//Eigen::Vector3f v3fViewVec = GetViewPoint() - GetEyeLocation();
	//v3fViewVec[1] = 0.0;
	//float fPhi = acosf(v3fViewVec[2] / v3fViewVec.norm());

	//fPhi = (v3fViewVec.x() >= 0.0) ? fPhi : -fPhi;

	Eigen::Vector3f v3fViewVec = GetViewPoint() - GetEyeLocation();
	v3fViewVec[2] = 0.0;
	float fDot = (0.0 == v3fViewVec.norm()) ? 1.0 : v3fViewVec[1] / v3fViewVec.norm();
	fDot = (fDot > 1.0) ? 1.0 : fDot;
	fDot = (fDot < -1.0) ? -1.0 : fDot;
	float fPhi = acosf(fDot);

	fPhi = (v3fViewVec.x() >= 0.0) ? fPhi : -fPhi;

	return (fPhi + M_PI / 2.0) * 360.0 / (2.0 * M_PI);
}

float GViewCameraPerspective::GetTilt() const
{
	Eigen::Vector3f v3dViewVec = GetViewPoint() - GetEyeLocation();
	float fDot = -v3dViewVec[1] / v3dViewVec.norm();
	fDot = (fDot > 1.0) ? 1.0 : fDot;
	fDot = (fDot < -1.0) ? -1.0 : fDot;
	float fTheta = acosf(fDot);
	return fTheta * 360.0 / (2.0 * M_PI);
}

void GViewCameraPerspective::SetPan(float fPan)
{
	SetPT(fPan, GetTilt());
}

void GViewCameraPerspective::SetTilt(float fTilt)
{
	SetPT(GetPan(), fTilt);
}

void GViewCameraPerspective::SetPT(float fPan, float fTilt)
{
	float fPhi = (fPan * 2.0 * M_PI / 360.0) + M_PI / 2.0;
	float fTheta = (180.0 - fTilt) * 2.0 * M_PI / 360.0;

	Eigen::AngleAxisf RotationTheta(fTheta, Eigen::Vector3f(-1.0, 0.0, 0.0));
	Eigen::AngleAxisf RotationPhi(fPhi, Eigen::Vector3f(0.0, 0.0, 1.0));

	Eigen::Vector3f v3fNewEyeLocation = RotationPhi * RotationTheta * GetReferenceEyeLocVector();
	Eigen::Vector3f v3fNewViewPoint = RotationPhi * RotationTheta * GetReferenceViewPiontVector();
	Eigen::Vector3f v3fNewUpVector = RotationPhi * RotationTheta * GetReferenceUpVector();

	if (v3fNewEyeLocation.allFinite() &&
		v3fNewViewPoint.allFinite() &&
		v3fNewUpVector.allFinite())
	{
		SetEyeLocation(v3fNewEyeLocation);
		SetViewPoint(v3fNewViewPoint);
		SetUpVector(v3fNewUpVector);
	}
	else
	{
		GLOGGER(error) << "GViewCameraPerspective::SetPTR(). invalid PTR. P: "
			<< fPan << ",T: " << fTilt;
	}

	//float fPhi = (fPan * 2.0 * M_PI / 360.0) - M_PI / 2.0;
	//float fTheta = fTilt * 2.0 * M_PI / 360.0;

	//Eigen::Vector3f v3fViewVec = GetViewPoint() - GetEyeLocation();
	//float fViewVecNorm = v3fViewVec.norm();

	//Eigen::Vector3f v3fNewViewVec;
	//v3fNewViewVec[0] = fViewVecNorm * sin(fTheta) * sin(fPhi);
	//v3fNewViewVec[1] = -fViewVecNorm * cos(fTheta);
	//v3fNewViewVec[2] = fViewVecNorm * sin(fTheta) * cos(fPhi);

	//SetViewPoint(GetEyeLocation() + v3fNewViewVec);

	//Eigen::Vector3f v3fNewUpVec;
	//v3fNewUpVec[0] = -cos(fTheta) * sin(fPhi);
	//v3fNewUpVec[1] = -sin(fTheta);
	//v3fNewUpVec[2] = -cos(fTheta) * cos(fPhi);

	//SetUpVector(v3fNewUpVec);
}

Eigen::Vector3f GViewCameraPerspective::GetReferenceEyeLocVector() const
{
	float fNorm = GetEyeLocation().norm();
	fNorm = (0.0 == fNorm) ? 1.0 : fNorm;
	return fNorm * Eigen::Vector3f(0.0, -1.0, 0.0);
}

Eigen::Vector3f GViewCameraPerspective::GetReferenceViewPiontVector() const
{
	float fNorm = GetViewPoint().norm();
	fNorm = (0.0 == fNorm) ? 1.0 : fNorm;
	return fNorm * Eigen::Vector3f(0.0, 1.0, 0.0);
}

Eigen::Vector3f GViewCameraPerspective::GetReferenceUpVector() const
{
	float fNorm = GetUpVector().norm();
	fNorm = (0.0 == fNorm) ? 1.0 : fNorm;
	return fNorm * Eigen::Vector3f(0.0, 0.0, -1.0);
}

glm::mat4 GViewCameraPerspectiveOutside::CalcP() const
{
	return glm::perspective(glm::radians(m_fViewFieldH_Degree), m_fWHRatio, 0.5f, 100.0f);
}

glm::mat4 GViewCameraPerspectiveOutside::CalcV() const
{
	float fPhi = m_fPan * 2.0 * M_PI / 360.0;
	float fTheta = m_fTilt * 2.0 * M_PI / 360.0;

	Eigen::Vector3f v3fEyeVecOriginal = Eigen::Vector3f(0.0, 0.0, -m_fEyeDistance);
	Eigen::Vector3f v3fUpVecOriginal = Eigen::Vector3f(0.0, -1.0, 0.0);

	Eigen::Vector3f v3fRotAxis = Eigen::Vector3f(1.0, 0.0, 0.0);
	Eigen::AngleAxisf RotationTheta = Eigen::AngleAxisf(fTheta, v3fRotAxis);

	Eigen::Vector3f v3fEyeVec0 = RotationTheta * v3fEyeVecOriginal;
	Eigen::Vector3f v3fUpVec0 = RotationTheta * v3fUpVecOriginal;

	Eigen::AngleAxisf RotationPhi = Eigen::AngleAxisf(fPhi, Eigen::Vector3f(0.0, 0.0, -1.0));
	Eigen::Vector3f v3fEyeVec1 = RotationPhi * v3fEyeVec0;
	Eigen::Vector3f v3fUpVec1 = RotationPhi * v3fUpVec0;

	return glm::lookAt(glm::vec3(v3fEyeVec1[0], v3fEyeVec1[1], v3fEyeVec1[2])
		, glm::vec3(0.0, 0.0, 0.0)
		, glm::vec3(v3fUpVec1[0], v3fUpVec1[1], v3fUpVec1[2]));
}

glm::mat4 GViewCameraPerspectiveInside::CalcP() const
{
	return glm::perspective(glm::radians(m_fViewFieldH_Degree), m_fWHRatio, 0.5f, 100.0f);
}

glm::mat4 GViewCameraPerspectiveInside::CalcV() const
{
	float fPhi = m_fPan * 2.0 * M_PI / 360.0;
	float fTheta = m_fTilt * 2.0 * M_PI / 360.0;

	Eigen::Vector3f v3fViewPointVecOriginal = Eigen::Vector3f(0.0, 0.0, 1.0);
	Eigen::Vector3f v3fUpVecOriginal = Eigen::Vector3f(0.0, -1.0, 0.0);

	Eigen::Vector3f v3fRotAxis = Eigen::Vector3f(1.0, 0.0, 0.0);
	Eigen::AngleAxisf RotationTheta = Eigen::AngleAxisf(fTheta, v3fRotAxis);

	Eigen::Vector3f v3fEyeVec0 = RotationTheta * v3fViewPointVecOriginal;
	Eigen::Vector3f v3fUpVec0 = RotationTheta * v3fUpVecOriginal;

	Eigen::AngleAxisf RotationPhi = Eigen::AngleAxisf(fPhi, Eigen::Vector3f(0.0, 0.0, -1.0));
	Eigen::Vector3f v3fViewPointVec1 = RotationPhi * v3fEyeVec0;
	Eigen::Vector3f v3fUpVec1 = RotationPhi * v3fUpVec0;

	return glm::lookAt(glm::vec3(0.0, 0.0, 0.0)
		, glm::vec3(v3fViewPointVec1[0], v3fViewPointVec1[1], v3fViewPointVec1[2])
		, glm::vec3(v3fUpVec1[0], v3fUpVec1[1], v3fUpVec1[2]));
}

glm::mat4 GViewCameraPerspectiveOutsideSurround::CalcP() const
{
	return glm::perspective(glm::radians(m_fViewFieldH_Degree), m_fWHRatio, 0.5f, 100.0f);
}

glm::mat4 GViewCameraPerspectiveOutsideSurround::CalcV() const
{
	float fPhi = m_fPan * 2.0 * M_PI / 360.0;
	float fTheta = m_fTilt * 2.0 * M_PI / 360.0;

	Eigen::Vector3f v3fEyeVecOriginal = Eigen::Vector3f(0.0, 0.0, -m_fEyeDistance);
	Eigen::Vector3f v3fUpVecOriginal = Eigen::Vector3f(0.0, -1.0, 0.0);

	Eigen::Vector3f v3fRotAxis = Eigen::Vector3f(1.0, 0.0, 0.0);
	Eigen::AngleAxisf RotationTheta = Eigen::AngleAxisf(fTheta, v3fRotAxis);

	Eigen::Vector3f v3fEyeVec0 = RotationTheta * v3fEyeVecOriginal;
	Eigen::Vector3f v3fUpVec0 = RotationTheta * v3fUpVecOriginal;

	Eigen::AngleAxisf RotationPhi = Eigen::AngleAxisf(fPhi, Eigen::Vector3f(0.0, 1.0, 0.0));
	Eigen::Vector3f v3fEyeVec1 = RotationPhi * v3fEyeVec0;
	Eigen::Vector3f v3fUpVec1 = RotationPhi * v3fUpVec0;

	return glm::lookAt(glm::vec3(v3fEyeVec1[0], v3fEyeVec1[1], v3fEyeVec1[2])
		, glm::vec3(0.0, 0.0, 0.0)
		, glm::vec3(v3fUpVec1[0], v3fUpVec1[1], v3fUpVec1[2]));
}

glm::mat4 GViewCameraPerspectiveInsideSurround::CalcP() const
{
	return glm::perspective(glm::radians(m_fViewFieldH_Degree), m_fWHRatio, 0.5f, 100.0f);
}

glm::mat4 GViewCameraPerspectiveInsideSurround::CalcV() const
{
	float fPhi = m_fPan * 2.0 * M_PI / 360.0;
	float fTheta = m_fTilt * 2.0 * M_PI / 360.0;

	Eigen::Vector3f v3fViewPointVecOriginal = Eigen::Vector3f(0.0, 0.0, 1.0);
	Eigen::Vector3f v3fUpVecOriginal = Eigen::Vector3f(0.0, -1.0, 0.0);

	Eigen::Vector3f v3fRotAxis = Eigen::Vector3f(1.0, 0.0, 0.0);
	Eigen::AngleAxisf RotationTheta = Eigen::AngleAxisf(fTheta, v3fRotAxis);

	Eigen::Vector3f v3fEyeVec0 = RotationTheta * v3fViewPointVecOriginal;
	Eigen::Vector3f v3fUpVec0 = RotationTheta * v3fUpVecOriginal;

	Eigen::AngleAxisf RotationPhi = Eigen::AngleAxisf(fPhi, Eigen::Vector3f(0.0, 1.0, 0.0));
	Eigen::Vector3f v3fViewPointVec1 = RotationPhi * v3fEyeVec0;
	Eigen::Vector3f v3fUpVec1 = RotationPhi * v3fUpVec0;

	return glm::lookAt(glm::vec3(0.0, 0.0, 0.0)
		, glm::vec3(v3fViewPointVec1[0], v3fViewPointVec1[1], v3fViewPointVec1[2])
		, glm::vec3(v3fUpVec1[0], v3fUpVec1[1], v3fUpVec1[2]));
}

glm::mat4 GViewCameraOrthogonal::CalcP() const
{
	return glm::ortho(-m_fWidth / 2.0, m_fWidth / 2.0
		, -m_fWidth / (2.0 * m_fWHRatio), m_fWidth / (2.0 * m_fWHRatio)
		, 0.5, 100.0);
}

glm::mat4 GViewCameraOrthogonal::CalcV() const
{
	return glm::lookAt(glm::vec3(m_fX0, m_fY0, -1.0)
		, glm::vec3(m_fX0, m_fY0, 0.0)
		, glm::vec3(0.0, -1.0, 0.0));
}

void GViewCameraOrthogonal::SetEyeLocationX(float fX)
{
	m_fX0 = fX;
}

float GViewCameraOrthogonal::GetEyeLocationX() const
{
	return m_fX0;
}

void GViewCameraOrthogonal::SetEyeLocationY(float fY)
{
	m_fY0 = fY;
}

float GViewCameraOrthogonal::GetEyeLocationY() const
{
	return m_fY0;
}

void GViewCameraOrthogonal::SetWidth(float fWidth)
{
	m_fWidth = fWidth;
}

float GViewCameraOrthogonal::GetWidth() const
{
	return m_fWidth;
}

void GViewCameraOrthogonal::SetWHRatio(float fWHRatio)
{
	m_fWHRatio = fWHRatio;
}

float GViewCameraOrthogonal::GetWHRatio() const
{
	return m_fWHRatio;
}

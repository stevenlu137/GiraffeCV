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
 
 
 
#ifndef G_CAMERA_MODEL_H_
#define G_CAMERA_MODEL_H_

#include <map>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "Common/Camera/GCameraType.h"


class CameraModel
{
public:
	CameraModel() : eCameraType(eCameraTypeNormal)
		, sCameraName("")
		,K0(cv::Mat(3, 3, CV_64F))
		, distorsionCoefficients(cv::Mat(1, 5, CV_64F))
		, K(cv::Mat(3, 3, CV_64F))
		, R(cv::Mat(3, 3, CV_64F))
		, t(cv::Mat(1, 3, CV_64F))
		, dDistortionValidRange(0.0)
	{
		K0.setTo(0.0);
		distorsionCoefficients.setTo(0.0);
		K.setTo(0.0);
		R.setTo(0.0);
		t.setTo(0.0);
	};
	~CameraModel(){};

	void Create(enCameraType eCamType)
	{
		eCamType = eCameraType;
		if (eCameraTypeNormal == eCameraType)
		{
			distorsionCoefficients.create(1, 5, CV_64F);
		}
		else if (eCameraTypeFisheye == eCameraType)
		{
			distorsionCoefficients.create(1, 4, CV_64F);
		}
		else if (eCameraTypeFisheyeAnglePoly5 == eCameraType)
		{
			distorsionCoefficients.create(1, 5, CV_64F);
		}

		K0.create(3, 3, CV_64F);
		K.create(3, 3, CV_64F);
		R.create(3, 3, CV_64F);
		t.create(1, 3, CV_64F);

		distorsionCoefficients.setTo(0.0);
		K0.setTo(0.0);
		K.setTo(0.0);
		R.setTo(0.0);
		t.setTo(0.0);

		dDistortionValidRange = 0.0;
	}

	inline double focal() const { return K0.at<double>(0, 0); }
	inline double aspect() const { return K0.at<double>(1, 1) / K.at<double>(0, 0); }
	inline double ppx() const { return K0.at<double>(0, 2); }
	inline double ppy() const { return K0.at<double>(1, 2); }

	inline void SetFocals(double f0, double fAspect)
	{
		K.at<double>(0, 0) = f0;
		K.at<double>(1, 1) = f0 * fAspect;
	}
	inline void SetPpx(double val) { K.at<double>(0, 2) = val; }
	inline void SetPpy(double val) { K.at<double>(1, 2) = val; }

	//Mat: double, row vector if the mat is 1-dimensional.
	enCameraType eCameraType;
	std::string sCameraName;
	cv::Mat K0;
	cv::Mat distorsionCoefficients;
	cv::Mat K;
	cv::Mat R;
	cv::Mat t;
	double dDistortionValidRange;

	std::map<std::string, cv::Mat> m_mpMultiK0Map;
};

static std::string Size2K0Name(int iWidth, int iHeight)
{
	return "K0_" + std::to_string(iWidth) + "_" + std::to_string(iHeight);
}

static cv::Size K0Name2Size(const std::string& sK0Name)
{
	return cv::Size(atoi(sK0Name.substr(sK0Name.find_first_of('_') + 1, sK0Name.find_last_of('_')).c_str()),
		atoi(sK0Name.substr(sK0Name.find_last_of('_') + 1, sK0Name.length()).c_str()));
}

#endif //G_CAMERA_MODEL_H_
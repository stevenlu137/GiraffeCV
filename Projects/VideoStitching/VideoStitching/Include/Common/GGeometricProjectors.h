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
 
 
 
#ifndef G_GEOMETRIC_PROJECTORS_H_
#define G_GEOMETRIC_PROJECTORS_H_

#include "opencv2/core/core.hpp"
#include "Common/Camera/GCameraType.h"

class GGeometricProjectors
{
public:
	//these projectors is all defined between coordinate to coordinate.
	//Pix: pixel coordinate of specified camera.
	//CylindricalDH: coordinate on cylindrical surface, d = r*theta, h is height.
	//these functions should be overloaded for different ModelScheme.

	static void Pix2Center(const std::vector<cv::Point2f>& vptSrc, std::vector<cv::Point2f>& vptDst
		, const cv::Mat& mDist, const cv::Mat& mK);

	static void Pix2UndistortedPix(const std::vector<cv::Point2f>& vptSrc, std::vector<cv::Point2f>& vptDst
		, const cv::Mat& mNewK, const cv::Mat& mDist, const cv::Mat& mK0);

	static cv::Point2f UndistortedPix2Pix(const cv::Point2f& ptSrc, enCameraType eCameratype, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mKNew);

	static cv::Point2f World3d2Pix(const cv::Point3f& ptSrc, enCameraType eCameratype, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mP);
	static void World3d2Pix(const std::vector<cv::Point3f>& vptSrc, std::vector<cv::Point2f>& vptDst, enCameraType eCameratype
		, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mP);
};


#endif //G_GEOMETRIC_PROJECTORS_H_
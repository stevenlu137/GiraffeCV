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
 
 
 
#include "GGeometricProjectors.h"

#include "GGeometricProjectors.cu"


void GGeometricProjectors::Pix2Center(const std::vector<cv::Point2f>& vptSrc, std::vector<cv::Point2f>& vptDst, const cv::Mat& mDist, const cv::Mat& mK)
{
	cv::Mat mKinv, mDistNew;
	mKinv = mK.inv();
	mKinv.convertTo(mKinv, CV_32FC1);
	mDist.convertTo(mDistNew, CV_32FC1);

	assert(mKinv.isContinuous());
	assert(mDistNew.isContinuous());

	vptDst.resize(vptSrc.size());
	return GGeometricProjectorsCUDA::Pix2Center(vptSrc, vptDst, (float*)(mDistNew.data), (float*)(mKinv.data));
}

static void Pix2UndistortedPix(const std::vector<cv::Point2f>& vptSrc, std::vector<cv::Point2f>& vptDst
	, const cv::Mat& mNewK, const cv::Mat& mDist, const cv::Mat& mK0)
{
	cv::Mat mKNew, mK0inv, mDistNew;
	mNewK.convertTo(mKNew, CV_32FC1);
	mK0inv = mK0.inv();
	mK0inv.convertTo(mK0inv, CV_32FC1);
	mDist.convertTo(mDistNew, CV_32FC1);

	assert(mKNew.isContinuous());
	assert(mK0inv.isContinuous());
	assert(mDistNew.isContinuous());

	vptDst.resize(vptSrc.size());
	return GGeometricProjectorsCUDA::Pix2UndistortedPix(vptSrc, vptDst, (float*)(mKNew.data), (float*)(mDistNew.data), (float*)(mK0inv.data));
}

cv::Point2f GGeometricProjectors::UndistortedPix2Pix(const cv::Point2f& ptSrc, enCameraType eCameratype
	, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mKNew)
{
	cv::Mat mfK0, mfDist, mfKNewInv;
	mK0.convertTo(mfK0, CV_32FC1);
	mDist.convertTo(mfDist, CV_32FC1);
	mfKNewInv = mKNew.inv();
	mfKNewInv.convertTo(mfKNewInv, CV_32FC1);

	assert(mfK0.isContinuous());
	assert(mfDist.isContinuous());
	assert(mfKNewInv.isContinuous());

	return GGeometricProjectorsCUDA::UndistortedPix2Pix(ptSrc, eCameratype, (float*)mfK0.data, (float*)mfDist.data, (float*)mfKNewInv.data);
}

cv::Point2f GGeometricProjectors::World3d2Pix(const cv::Point3f& ptSrc, enCameraType eCameratype, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mP)
{
	cv::Mat mfK0, mfDist, mfP;
	mK0.convertTo(mfK0, CV_32FC1);
	mDist.convertTo(mfDist, CV_32FC1);
	mP.convertTo(mfP, CV_32FC1);

	assert(mfK0.isContinuous());
	assert(mfDist.isContinuous());
	assert(mfP.isContinuous());

	return GGeometricProjectorsCUDA::World3d2Pix(ptSrc, eCameratype, (const float*)mfK0.data, (const float*)mfDist.data, (const float*)mfP.data);
}

void GGeometricProjectors::World3d2Pix(const std::vector<cv::Point3f>& vptSrc, std::vector<cv::Point2f>& vptDst, enCameraType eCameratype
	, const cv::Mat& mK0, const cv::Mat& mDist, const cv::Mat& mP)
{
	cv::Mat mfK0, mfDist, mfP;
	mK0.convertTo(mfK0, CV_32FC1);
	mDist.convertTo(mfDist, CV_32FC1);
	mP.convertTo(mfP, CV_32FC1);

	assert(mfK0.isContinuous());
	assert(mfDist.isContinuous());
	assert(mfP.isContinuous());

	GGeometricProjectorsCUDA::World3d2Pix(vptSrc, vptDst, eCameratype, (const float*)mfK0.data, (const float*)mfDist.data, (const float*)mfP.data);
}

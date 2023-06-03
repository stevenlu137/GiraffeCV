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
 
 
 
#ifndef G_SERIALIZATION_H_
#define G_SERIALIZATION_H_

#include <boost/serialization/version.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/map.hpp>

#include <Eigen/Core>
#include <opencv2/opencv.hpp>

#include "Common/GPanoCameraInfo.h"


BOOST_SERIALIZATION_SPLIT_FREE(::cv::Mat)

namespace boost
{
	namespace serialization
	{
		//cv::Mat
		//todo: add support for Mat with non-zero steps.
		template <class Archive>
		void save(Archive & ar, const ::cv::Mat& m, const unsigned int version)
		{
			size_t elem_size = m.elemSize();
			size_t elem_type = m.type();

			ar & m.cols;
			ar & m.rows;
			ar & elem_size;
			ar & elem_type;

			const size_t data_size = m.cols * m.rows * elem_size;
			ar & boost::serialization::make_array(m.ptr(), data_size);
		}

		template <class Archive>
		void load(Archive & ar, ::cv::Mat& m, const unsigned int version)
		{
			int cols, rows;
			size_t elem_size, elem_type;

			ar & cols;
			ar & rows;
			ar & elem_size;
			ar & elem_type;

			m.create(rows, cols, elem_type);

			size_t data_size = m.cols * m.rows * elem_size;
			ar & boost::serialization::make_array(m.ptr(), data_size);
		}

		//Eigen::Matrix
		template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
		void save(Archive& ar
			, const ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& m
			, const unsigned int version)
		{
			int iRows = m.rows();
			int iCols = m.cols();

			ar & iRows;
			ar & iCols;
			ar & boost::serialization::make_array(m.data(), iRows * iCols);
		}

		template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
		void load(Archive& ar
			, ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& m
			, const unsigned int version)
		{
			int iCols, iRows;

			ar & iRows;
			ar & iCols;
			m.resize(iRows, iCols);
			ar & boost::serialization::make_array(m.data(), iRows * iCols);
		}

		template <class Archive, class _Scalar, int _Rows, int _Cols, int _Options, int _MaxRows, int _MaxCols>
		void serialize(Archive& ar
			, ::Eigen::Matrix<_Scalar, _Rows, _Cols, _Options, _MaxRows, _MaxCols>& m
			, const unsigned int version)
		{
			split_free(ar, m, version);
		}

		//cv::Size
		template<class Archive>
		void serialize(Archive & ar, cv::Size & m, const unsigned int version)
		{
			ar & m.width;
			ar & m.height;
		}

		//GCameraPanoMapKDM
		template<class Archive>
		void serialize(Archive & ar, ::GCameraPanoMapKDM & m, const unsigned int version)
		{
			ar & m.mpMultiK0Map;
			ar & m.mDist;
			ar & m.mM;
			ar & m.dDistortionValidRange;
		}

		//GCameraModel4Profile
		template<class Archive>
		void serialize(Archive & ar, ::GCameraModel4Profile & m, const unsigned int version)
		{
			ar & m.eCameraType;
			ar & m.sCameraName;
			ar & m.cameraPanoMap;
		}

		//GPanoTypeParas
		template<class Archive>
		void serialize(Archive & ar, ::GPanoTypeParas & e, const unsigned int version)
		{
			ar & e.ePanoType;
			ar & e.fBottomCropRatio;
			ar & e.fLeftCropRatio;
			ar & e.fRightCropRatio;
			ar & e.fTopCropRatio;
		}

		//GPanoCameraProfile
		template<class Archive>
		void serialize(Archive & ar, ::GPanoCameraProfile & e, const unsigned int version)
		{
			ar & e.vCameraModels;
			ar & e.vPanoTypeList;
		}
	}
}

#endif //G_SERIALIZATION_H_
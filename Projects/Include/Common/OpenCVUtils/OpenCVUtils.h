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
 
 
 
#ifndef OPENCV_UTILS_H_
#define OPENCV_UTILS_H_

#include "limits"

#include "opencv2/core/types_c.h"

template<unsigned int ValueTypeIndex>
struct ValueTypeTraits
{
	typedef void ValueType;
};

template<>
struct ValueTypeTraits<0>
{
	typedef uchar ValueType;
	const ValueType Max = UCHAR_MAX;
	const ValueType Min = 0;
};

template<>
struct ValueTypeTraits<1>
{
	typedef char ValueType;
	const ValueType Max = CHAR_MAX;
	const ValueType Min = CHAR_MIN;
};

template<>
struct ValueTypeTraits<2>
{
	typedef unsigned short ValueType;
	const ValueType Max = USHRT_MAX;
	const ValueType Min = 0;
};

template<>
struct ValueTypeTraits<3>
{
	typedef short ValueType;
	const ValueType Max = SHRT_MAX;
	const ValueType Min = SHRT_MIN;
};

template<>
struct ValueTypeTraits<4>
{
	typedef int ValueType;
	const ValueType Max = INT_MAX;
	const ValueType Min = INT_MIN;
};

template<>
struct ValueTypeTraits<5>
{
	typedef float ValueType;
	const ValueType Max = FLT_MAX;
	const ValueType Min = FLT_MIN;
};

template<>
struct ValueTypeTraits<6>
{
	typedef double ValueType;
	const ValueType Max = DBL_MAX;
	const ValueType Min = DBL_MIN;
};


template<typename TValueType, unsigned int uiChannelsNum>
class ToOpenCVType
{
public:
	static unsigned int Get()
	{
		return 0;
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<uchar, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_8U, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<char, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_8S, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<unsigned short, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_16U, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<short, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_16S, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<int, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_32S, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<float, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_32F, (uiChannelsNum));
	}
};

template<unsigned int uiChannelsNum>
class ToOpenCVType<double, uiChannelsNum>
{
public:
	static unsigned int Get()
	{
		return CV_MAKETYPE(CV_64F, (uiChannelsNum));
	}
};

class CVRunTimeTypeInfo
{
public:
	static unsigned int GetValueType(unsigned int uiType)
	{
		return CV_MAT_DEPTH(uiType);
	};

	static unsigned int GetChannels(unsigned int uiType)
	{
		return CV_MAT_CN(uiType);
	};

	static double GetConvMultifier(unsigned int uiSrcType, unsigned int uiDstType)
	{
		unsigned int uiSrcValueType = (uiSrcType & ((1 << CV_CN_SHIFT) - 1));
		unsigned int uiDstValueType = (uiDstType & ((1 << CV_CN_SHIFT) - 1));
		return (
			GetTypeMax(uiDstValueType) / (GetTypeMax(uiSrcValueType) - GetTypeMin(uiSrcValueType)) +
			GetTypeMin(uiDstValueType) / (GetTypeMin(uiSrcValueType) - GetTypeMax(uiSrcValueType)));
	};

	static double GetConvPlus(unsigned int uiSrcType, unsigned int uiDstType)
	{
		unsigned int uiSrcValueType = (uiSrcType & 7);
		unsigned int uiDstValueType = (uiDstType & 7);
		return (GetTypeMax(uiDstValueType)*GetTypeMin(uiSrcValueType)) / (GetTypeMin(uiSrcValueType) - GetTypeMax(uiSrcValueType)) +
			(GetTypeMin(uiDstValueType)*GetTypeMax(uiSrcValueType)) / (GetTypeMax(uiSrcValueType) - GetTypeMin(uiSrcValueType));
	};

	static int GetTypeSize_Byte(unsigned int uiType)
	{
		switch (CV_MAT_DEPTH(uiType))
		{
		case 0://8U
			return 1;
			break;
		case 1://8S
			return 1;
			break;
		case 2://16U
			return 2;
			break;
		case 3://16S
			return 2;
			break;
		case 4://32S
			return 4;
			break;
		case 5://32F
			return 4;
			break;
		case 6://64F
			return 8;
			break;
		default:
			break;
		}
	};
	
	static double GetTypeMax(unsigned int uiType)
	{
		switch (CV_MAT_DEPTH(uiType))
		{
		case 0://8U
			return static_cast<double>(std::numeric_limits<uchar>::max());
			break;
		case 1://8S
			return static_cast<double>(std::numeric_limits<char>::max());
			break;
		case 2://16U
			return static_cast<double>(std::numeric_limits<unsigned short>::max());
			break;
		case 3://16S
			return static_cast<double>(std::numeric_limits<short>::max());
			break;
		case 4://32S
			return static_cast<double>(std::numeric_limits<int>::max());
			break;
		case 5://32F
			return static_cast<double>(std::numeric_limits<float>::max());
			break;
		case 6://64F
			return static_cast<double>(std::numeric_limits<double>::max());
			break;
		default:
			break;
		}
	};

	static double GetTypeMin(unsigned int uiType)
	{
		switch (CV_MAT_DEPTH(uiType))
		{
		case 0://8U
			return static_cast<double>(std::numeric_limits<uchar>::min());
			break;
		case 1://8S
			return static_cast<double>(std::numeric_limits<char>::min());
			break;
		case 2://16U
			return static_cast<double>(std::numeric_limits<unsigned short>::min());
			break;
		case 3://16S
			return static_cast<double>(std::numeric_limits<short>::min());
			break;
		case 4://32S
			return static_cast<double>(std::numeric_limits<int>::min());
			break;
		case 5://32F
			return static_cast<double>(std::numeric_limits<float>::min());
			break;
		case 6://64F
			return static_cast<double>(std::numeric_limits<double>::min());
			break;
		default:
			break;
		}
	};
};


#endif//OPENCV_UTILS_H_
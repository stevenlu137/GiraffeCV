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
 
 
 
#ifndef GIRAFFECV_EXCEPTION_H_
#define GIRAFFECV_EXCEPTION_H_

#include <exception>

#include "Common/GiraffeLogger/GiraffeLogger.h"

class GiraffeException : virtual public std::exception
{
public:

};

class GCommonException : virtual public GiraffeException
{
public:

};

class GGraphException : virtual public GiraffeException
{
public:

};

class GModelEstimatorException : virtual public GiraffeException
{
public:

};

class GCalibrationTaskException : virtual public GiraffeException
{
public:

};

class GCalibrationServiceException : virtual public GiraffeException
{
public:

};

class GServerException : virtual public GiraffeException
{
public:

};

class GAquilaException : virtual public GiraffeException
{
public:

};

class GPanoRenderException : virtual public GiraffeException
{
public:

};

class GInternetErrorException : virtual public GAquilaException
{
public:

};

class GVideoSourceException : virtual public GiraffeException
{
public:

};

class GVideoStitchingQuit : virtual public GiraffeException
{
public:

};

#endif //GIRAFFECV_EXCEPTION_H_
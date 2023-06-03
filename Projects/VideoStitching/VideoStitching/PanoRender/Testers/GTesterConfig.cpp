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
 
 
 
#include "GTesterConfig.h"
#include "opencv2/core/persistence.hpp"


void GTesterConfig::ReadConfigFile(const std::string& sConfigPath)
{
	cv::FileStorage fileStorageRead(sConfigPath, cv::FileStorage::READ);

	if (!fileStorageRead.isOpened())
	{
		fileStorageRead.release();
		return;
	}

	if (fileStorageRead["sWorkMode"].empty()
		|| fileStorageRead["sRenderingMode"].empty()
		|| fileStorageRead["sProfilePath"].empty()
		|| fileStorageRead["vURLs"].empty()
		)
	{
		fileStorageRead.release();
		return;
	}

	fileStorageRead["sWorkMode"] >> sWorkMode;
	fileStorageRead["sRenderingMode"] >> sRenderingMode;
	fileStorageRead["sProfilePath"] >> sProfilePath;
	fileStorageRead["vURLs"] >> vURLs;

	fileStorageRead.release();
}

void GTesterConfig::SaveConfigFile(const std::string& sConfigPath)
{
	cv::FileStorage fileStorageWrite("PanoRenderOGLTesterConfig.yml", cv::FileStorage::WRITE);

	if (!fileStorageWrite.isOpened())
	{
		fileStorageWrite.release();
		return;
	}

	fileStorageWrite << "sWorkMode" << sWorkMode;
	fileStorageWrite << "sRenderingMode" << sRenderingMode;
	fileStorageWrite << "sProfilePath" << sProfilePath;
	fileStorageWrite << "vURLs" << vURLs;
	fileStorageWrite.release();
}

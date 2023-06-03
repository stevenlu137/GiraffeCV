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
 
 
 
#ifndef G_VIDEO_STITCHER_TESTER_UTILS_H_
#define G_VIDEO_STITCHER_TESTER_UTILS_H_

#include <string>
#include <vector>

#include "boost/filesystem.hpp"

#include "GCameraModel.h"
#include "GPanoCameraInfo.h"


void GetFileList(const std::string& sPath, const std::vector<std::string>& vFileTypes, std::vector<boost::filesystem::path>& vFileList);

void ReadFileContentBin(const std::string sFilePath, std::vector<char>& vFileContent);

template <class TGraph>
bool SaveObjectToFileBin(const TGraph& graph, const std::string& sFilePath);

template <class TGraph>
bool LoadObjectFromFileBin(const std::string& sFilePath, TGraph& graph);

template <class TGraph>
bool SaveObjectToFileText(const TGraph& graph, const std::string& sFilePath);

template <class TGraph>
bool LoadObjectFromFileText(const std::string& sFilePath, TGraph& graph);

template <class TGraph>
bool LoadObjectFromFileTextString(const std::string& sModelFileTextContent, TGraph& graph);

template <class TGraph>
bool LoadObjectFromFileBinRawData(const char* pData, int iSize_Byte, TGraph& graph);

bool ReadCalibrationFile(const std::string& sFilePath, enCameraType& eCameraType, cv::Mat& mK0, cv::Mat& mDist);

template <class TCamera>
void SortByCameraName(std::vector<TCamera>& vCameras);

void TransPanoCameraProfile2CameraModel(const GPanoCameraProfile& panoCameraProfile, std::vector<CameraModel>& vCameraModel);

void CamerasFilter(std::vector<boost::filesystem::path>& vVideoFiles, std::vector<CameraModel>& vCameraModel);

bool GetCameraParams(std::vector<CameraModel> &vCameras);

bool GetCameraParamsOld(std::vector<CameraModel> &vCameras);

#endif //G_VIDEO_STITCHER_TESTER_UTILS_H_
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
 
 
 
#include "GFileUtils.h"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <sstream>

#include "boost/range/iterator_range.hpp"

#include "GPortableArchive.h"
#include "Common/GiraffeLogger/GiraffeLogger.h"
#include "GSerialization.h"


void GetFileList(const std::string& sPath, const std::vector<std::string>& vFileTypes, std::vector<boost::filesystem::path>& vFileList)
{
	//find the source files under specified path.
	boost::filesystem::path pWorkPath(sPath);
	if (!is_directory(pWorkPath))
	{
		std::cout << "GetFileList: path is not correctly specified." << std::endl;
		return;
	}

	for (auto& subPath : boost::make_iterator_range(boost::filesystem::directory_iterator(pWorkPath), boost::filesystem::directory_iterator()))
	{
		if (vFileTypes.end() != std::find(vFileTypes.begin(), vFileTypes.end(), subPath.path().extension().string()))
		{
			vFileList.push_back(system_complete(subPath));
		}
	}

	//read images and construct dataGraph.
	std::sort(vFileList.begin(), vFileList.end(), [](const boost::filesystem::path& p1, const boost::filesystem::path& p2)
	{
		return p1.stem().string() < p2.stem().string();
	});
}

void ReadFileContentBin(const std::string sFilePath, std::vector<char>& vFileContent)
{
	std::filebuf* pBuf;
	std::ifstream ifs(sFilePath, std::ios::in | std::ios::binary);
	if (!ifs.good())
	{
		std::cout << sFilePath << " file not exist." << std::endl;
		return;
	}

	pBuf = ifs.rdbuf();
	int iFileSize_Byte = pBuf->pubseekoff(0, std::ios::end, std::ios::in);
	pBuf->pubseekpos(0, std::ios::in);

	vFileContent.resize(iFileSize_Byte);
	pBuf->sgetn(vFileContent.data(), iFileSize_Byte);

	ifs.close();
}

template <class TGraph>
bool SaveObjectToFileBin(const TGraph& graph, const std::string& sFilePath)
{
	std::ofstream ofs(sFilePath, std::ios::binary);
	if (!ofs.good())
	{
		std::cerr << "Can NOT open " << sFilePath << "\n";
		return false;
	}

	portable_oarchive oa(ofs);
	oa << graph;

	return true;
}

template bool SaveObjectToFileBin(const GPanoCameraProfile& obj, const std::string& sFilePath);


template <class TGraph>
bool LoadObjectFromFileBin(const std::string& sFilePath, TGraph& graph)
{
	std::ifstream ifs(sFilePath, std::ios::binary);
	if (!ifs.good())
	{
		std::cerr << "Can NOT open " << sFilePath << "\n";
		return false;
	}

	portable_iarchive ia(ifs);
	ia >> graph;

	return true;
}

template bool LoadObjectFromFileBin(const std::string& sFilePath, GPanoCameraProfile& obj);


template <class TGraph>
bool SaveObjectToFileText(const TGraph& graph, const std::string& sFilePath)
{
	std::ofstream ofs(sFilePath);
	if (!ofs.good())
	{
		std::cerr << "Can NOT open " << sFilePath << "\n";
		return false;
	}

	text_oarchive oa(ofs);
	oa << graph;

	return true;
}

template bool SaveObjectToFileText(const GPanoCameraProfile& obj, const std::string& sFilePath);


template <class TGraph>
bool LoadObjectFromFileText(const std::string& sFilePath, TGraph& graph)
{
	std::ifstream ifs(sFilePath);
	if (!ifs.good())
	{
		GLOGGER(error) << "Can NOT open " << sFilePath;
		return false;
	}

	text_iarchive ia(ifs);
	ia >> graph;

	return true;
}

template bool LoadObjectFromFileText(const std::string& sFilePath, GPanoCameraProfile& obj);


template <class TGraph>
bool LoadObjectFromFileTextString(const std::string& sModelFileTextContent, TGraph& graph)
{
	std::stringstream iss(sModelFileTextContent);
	if (!iss.good())
	{
		GLOGGER(error) << "Invalid model file string: " << sModelFileTextContent;
		return false;
	}

	text_iarchive ia(iss);
	ia >> graph;

	return true;
}

template bool LoadObjectFromFileTextString(const std::string& sModelFileTextContent, GPanoCameraProfile& obj);

template <class TGraph>
bool LoadObjectFromFileBinRawData(const char* pData, int iSize_Byte, TGraph& graph)
{
	std::string sModel(pData, iSize_Byte);
	std::stringstream iss(sModel);

	if (!iss.good())
	{
		GLOGGER(error) << "Invalid model raw data";
		return false;
	}

	portable_iarchive ia(iss);
	ia >> graph;

	return true;
}

template bool LoadObjectFromFileBinRawData(const char* pData, int iSize_Byte, GPanoCameraProfile& obj);

bool ReadCalibrationFile(const std::string& sFilePath, enCameraType& eCameraType, cv::Mat& mK0, cv::Mat& mDist)
{
	cv::FileStorage fileStorageReadCam(sFilePath, cv::FileStorage::READ);
	
	if ((!fileStorageReadCam["Default_K"].empty())
		&& (!fileStorageReadCam["Default_Dist"].empty())
		&& (!fileStorageReadCam["Default_Type"].empty()))
	{
		fileStorageReadCam["Default_K"] >> mK0;
		fileStorageReadCam["Default_Dist"] >> mDist;

		std::string sCameraType = "";
		fileStorageReadCam["Default_Type"] >> sCameraType;
		if ("normal" == sCameraType)
		{
			eCameraType = eCameraTypeNormal;
		}
		else if ("fisheye" == sCameraType)
		{
			eCameraType = eCameraTypeFisheye;
		}
		else if ("fisheyeAnglePoly5" == sCameraType)
		{
			eCameraType = eCameraTypeFisheyeAnglePoly5;
		}
	}
	else
	{
		std::cout << "ReadCalibrationFile(): Can not find default parameter in .yml file." << std::endl;
		return false;
	}

	mK0.convertTo(mK0, CV_64FC1);
	mDist.convertTo(mDist, CV_64FC1);

	return true;
}

template <class TCamera>
void SortByCameraName(std::vector<TCamera>& vCameras)
{
	auto fnLessThan = [](const TCamera& cam0, const TCamera& cam1)
	{
		return atoi(cam0.sCameraName.c_str()) < atoi(cam1.sCameraName.c_str());
	};

	std::sort(vCameras.begin(), vCameras.end(), fnLessThan);
}

template void SortByCameraName(std::vector<GCameraModel4Profile>& vCameras);

void TransPanoCameraProfile2CameraModel(const GPanoCameraProfile& panoCameraInfo, std::vector<CameraModel>& vCameraModel)
{
	vCameraModel.clear();

	for (int iCamIdx = 0; iCamIdx < panoCameraInfo.vCameraModels.size(); ++iCamIdx)
	{
		CameraModel cam;
		cam.eCameraType = panoCameraInfo.vCameraModels[iCamIdx].eCameraType;
		cam.sCameraName = panoCameraInfo.vCameraModels[iCamIdx].sCameraName;
		cam.m_mpMultiK0Map = panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.mpMultiK0Map;
		cam.distorsionCoefficients = panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.mDist;
		cam.R = panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.mM;
		cam.dDistortionValidRange = panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.dDistortionValidRange;

		cv::Size szK0Mode = K0Name2Size(panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.mpMultiK0Map.begin()->first);
		cv::Mat mT(3, 3, CV_64FC1);
		mT.setTo(0.0);
		mT.at<double>(0, 0) = static_cast<double>(szK0Mode.width);
		mT.at<double>(1, 1) = static_cast<double>(szK0Mode.height);
		mT.at<double>(2, 2) = 1.0;

		cam.K0 = mT * panoCameraInfo.vCameraModels[iCamIdx].cameraPanoMap.mpMultiK0Map.begin()->second;
		//ATTENTION. NO m_K0 speicfied here.
		vCameraModel.push_back(cam);
	}
}

void CamerasFilter(std::vector<boost::filesystem::path>& vVideoFiles, std::vector<CameraModel>& vCameraModel)
{
	std::vector<boost::filesystem::path> vNewVideoFiles;

	for (int iCamIdx = vCameraModel.size() - 1; iCamIdx >= 0; --iCamIdx)
	{
		auto itFoundFile = std::find_if(vVideoFiles.begin(), vVideoFiles.end()
			, [&vCameraModel, iCamIdx](const boost::filesystem::path& videoFilePath)
		{
			return (videoFilePath.stem().string() == vCameraModel[iCamIdx].sCameraName);
		});

		if (vVideoFiles.end() == itFoundFile)
		{
			vCameraModel.erase(vCameraModel.begin() + iCamIdx);
		}
		else
		{
			vNewVideoFiles.push_back(*itFoundFile);
		}
	}

	vVideoFiles.clear();
	for (int iFileIdx = 0; iFileIdx < vNewVideoFiles.size(); ++iFileIdx)
	{
		vVideoFiles.push_back(vNewVideoFiles[vNewVideoFiles.size() - iFileIdx - 1]);
	}
}

bool GetCameraParams(std::vector<CameraModel> &vCameras)//todo.
{
	vCameras.clear();

	cv::FileStorage fileStorageReadCam("CalibrationData/Cameras_Calibration.yml", cv::FileStorage::READ);

	if (!fileStorageReadCam.isOpened())
	{
		std::cout << "VideoStitcher::InitModelGraph(): Can not find valid parameter files: Cameras_Calibration.yml." << std::endl;
		fileStorageReadCam.release();
		return false;
	}

	CameraModel cam;

	if ((!fileStorageReadCam["Default_K"].empty()) && (!fileStorageReadCam["Default_Dist"].empty()) && (!fileStorageReadCam["Default_Type"].empty()))
	{
		fileStorageReadCam["Default_K"] >> cam.K0;
		fileStorageReadCam["Default_Dist"] >> cam.distorsionCoefficients;

		std::string sTemp = "";
		fileStorageReadCam["Default_Type"] >> sTemp;
		if ("normal" == sTemp)
		{
			cam.eCameraType = eCameraTypeNormal;
		}
		else if ("fisheye" == sTemp)
		{
			cam.eCameraType = eCameraTypeFisheye;
		}
		else if ("fisheyeAnglePoly5" == sTemp)
		{
			cam.eCameraType = eCameraTypeFisheyeAnglePoly5;
		}
	}
	else
	{
		std::cout << "VideoStitcher::InitModelGraph(): Can not find valid parameter files: Cameras_Calibration.yml." << std::endl;
		fileStorageReadCam.release();
		return false;
	}

	cam.K0.convertTo(cam.K0, CV_64FC1);
	cam.distorsionCoefficients.convertTo(cam.distorsionCoefficients, CV_64FC1);
	cam.R = cv::Mat::eye(3, 3, CV_64FC1);
	cam.K.create(3, 3, CV_64FC1);
	cam.K.setTo(0.0);

	vCameras.push_back(cam);
	return true;
}

bool GetCameraParamsOld(std::vector<CameraModel> &vCameras)
{
	cv::Mat mDistorsionCoefficients;
	cv::Mat mK;

	cv::FileStorage fileStorageReadDist("CalibrationData/DistorsionCoefficients_Calibration.yml", cv::FileStorage::READ);
	cv::FileStorage fileStorageReadCam("CalibrationData/Cameras_Calibration.yml", cv::FileStorage::READ);

	if ((!fileStorageReadDist.isOpened())
		|| (!fileStorageReadCam.isOpened())
		|| fileStorageReadDist["DistorsionCoefficients"].empty()
		|| fileStorageReadCam["Cameras_ALL_K"].empty())
	{
		std::cerr << "VideoStitcher::GetCameras(): Can not find valid parameter files: DistorsionCoefficients_Calibration.yml,Cameras_Calibration.yml." << "\n";
		fileStorageReadCam.release();
		fileStorageReadDist.release();
		return false;
	}

	fileStorageReadCam["Cameras_ALL_K"] >> mK;
	fileStorageReadDist["DistorsionCoefficients"] >> mDistorsionCoefficients;

	fileStorageReadCam.release();
	fileStorageReadDist.release();

	cv::FileStorage fileStorageReadCamera("CalibrationData/CameraMatrixs.yml", cv::FileStorage::READ);
	if (fileStorageReadCamera.isOpened())
	{
		int iCamNum = 0;
		fileStorageReadCamera["CamNum"] >> iCamNum;

		vCameras.resize(iCamNum);

		//ALL the cameras share the same K0 matrix and distortion coefficients.
		for (int i = 0; i < iCamNum; ++i)
		{
			mDistorsionCoefficients.convertTo(vCameras[i].distorsionCoefficients, CV_64F);
			mK.convertTo(vCameras[i].K0, CV_64F);
			mK.convertTo(vCameras[i].K, CV_64F);
		}

		for (int i = 0; i < iCamNum; ++i)
		{
			std::string sIdx = "Cameras_";
			sIdx += std::to_string(i);
			fileStorageReadCamera[sIdx + "_R"] >> vCameras[i].R;
			fileStorageReadCamera[sIdx + "_K"] >> vCameras[i].K;
			fileStorageReadCamera[sIdx + "_t"] >> vCameras[i].t;
		}
		fileStorageReadCamera.release();
	}

	return true;
}

cmake_minimum_required(VERSION 2.8.12)

PROJECT (VideoStitching)

include($ENV{GIRAFFECV_DIR}/cmake/BaseConfig.cmake)


############################  Include Pathes List #################################

set(VIDEO_STITCHER_INCLUDE_PATH_LIST
${CMAKE_SOURCE_DIR}
$ENV{GIRAFFECV_DIR}/build/3rd_party
${GIRAFFECV_Boost_PATH}/Include
${GIRAFFECV_Eigen_PATH}/Include
$ENV{GIRAFFECV_DIR}/Projects/Include
$ENV{GIRAFFECV_DIR}/Projects/Include/Common
$ENV{GIRAFFECV_DIR}/Projects/Include/Common/CUDA
$ENV{GIRAFFECV_DIR}/Projects/Include/VideoStitching
$ENV{GIRAFFECV_DIR}/Projects/VideoStitching/VideoStitching/Include
$ENV{GIRAFFECV_DIR}/Projects/VideoStitching/VideoStitching/Include/Common
$ENV{GIRAFFECV_DIR}/Projects/VideoStitching/VideoStitching
${GIRAFFECV_OpenCV_PATH}/Include
${GIRAFFECV_OpenCV_PATH}/Include/opencv
${GIRAFFECV_OpenCV_PATH}/Include/opencv2
${GIRAFFECV_FreeGlut_PATH}/Include
${GIRAFFECV_GLM_PATH}/Include
${GIRAFFECV_GLFW_PATH}/Include
${GIRAFFECV_GLEW_PATH}/Include
)


############################  Link Pathes List #############################

set(VIDEO_STITCHER_LINK_PATH_LIST
	${GIRAFFECV_Boost_PATH}/${OSFLAG}/${ARCHFLAG}/lib
	${CUDA_SDK_LIB_DIR}
    $ENV{GIRAFFECV_DIR}/build/${OSFLAG}/${ARCHFLAG}/lib/${CMAKE_BUILD_TYPE}
	${GIRAFFECV_LIBS_PATH}/${OSFLAG}/${ARCHFLAG}/lib/${CMAKE_BUILD_TYPE}
)

giraffecv_get_opencv_lib("core" "Release" LibOpenCVCore)
giraffecv_get_opencv_lib("imgproc" "Release" LibOpenCVImgproc)
giraffecv_get_opencv_lib("features2d" "Release" LibOpenCVFeatures2d)
giraffecv_get_opencv_lib("calib3d" "Release" LibOpenCVCalib3d)
giraffecv_get_opencv_lib("video" "Release" LibOpenCVVideo)
giraffecv_get_opencv_lib("highgui" "Release" LibOpenCVHighgui)
giraffecv_get_opencv_lib("stitching" "Release" LibOpenCVStitching)
giraffecv_get_opencv_lib("flann" "Release" LibOpenCVFlann)
giraffecv_get_opencv_lib("imgcodecs" "Release" LibOpenCVImgcodecs)
giraffecv_get_opencv_lib("cudafeatures2d" "Release" LibOpenCVCUDAFeatures2D)
giraffecv_get_opencv_lib("xfeatures2d" "Release" LibOpenCVXFeatures2D)
giraffecv_get_opencv_lib("videoio" "Release" LibOpenCVVideoIO)
giraffecv_get_opencv_lib("cudawarping" "Release" LibOpenCVCUDAWarping)

giraffecv_get_opencv_lib("core" "Debug" LibOpenCVCoreD)
giraffecv_get_opencv_lib("imgproc" "Debug" LibOpenCVImgprocD)
giraffecv_get_opencv_lib("features2d" "Debug" LibOpenCVFeatures2dD)
giraffecv_get_opencv_lib("calib3d" "Debug" LibOpenCVCalib3dD)
giraffecv_get_opencv_lib("video" "Debug" LibOpenCVVideoD)
giraffecv_get_opencv_lib("highgui" "Debug" LibOpenCVHighguiD)
giraffecv_get_opencv_lib("stitching" "Debug" LibOpenCVStitchingD)
giraffecv_get_opencv_lib("flann" "Debug" LibOpenCVFlannD)
giraffecv_get_opencv_lib("imgcodecs" "Debug" LibOpenCVImgcodecsD)
giraffecv_get_opencv_lib("cudafeatures2d" "Debug" LibOpenCVCUDAFeatures2DD)
giraffecv_get_opencv_lib("xfeatures2d" "Debug" LibOpenCVXFeatures2DD)
giraffecv_get_opencv_lib("videoio" "Debug" LibOpenCVVideoIOD)
giraffecv_get_opencv_lib("cudawarping" "Debug" LibOpenCVCUDAWarpingD)

giraffecv_get_opencv_lib_static("core" "Release" LibOpenCVCore_Static)
giraffecv_get_opencv_lib_static("core" "Debug" LibOpenCVCore_StaticD)

INCLUDE_DIRECTORIES(
${VIDEO_STITCHER_INCLUDE_PATH_LIST}
)

LINK_DIRECTORIES(
${VIDEO_STITCHER_LINK_PATH_LIST}
)


add_subdirectory(PanoRender)


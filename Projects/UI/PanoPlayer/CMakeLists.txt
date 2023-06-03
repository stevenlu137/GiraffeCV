cmake_minimum_required(VERSION 3.16)

PROJECT (PanoPlayer)

include($ENV{GIRAFFECV_DIR}/cmake/BaseConfig.cmake)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(QT_COMPONENTS Widgets)
find_package(Qt6 COMPONENTS ${QT_COMPONENTS})
if(Qt6_FOUND)
    set(QT_VERSION_MAJOR 6)
    set(QT_LIBRARIES Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS ${QT_COMPONENTS} REQUIRED)
    set(QT_VERSION_MAJOR 5)
    set(QT_LIBRARIES Qt5::Widgets)
endif()

message(STATUS "QT Version found: " ${QT_VERSION_MAJOR})


############################  Include Pathes List #################################

set(PANO_PLAYER_INCLUDE_PATH_LIST
${CMAKE_SOURCE_DIR}
${GIRAFFECV_OpenCV_PATH}/Include
${GIRAFFECV_OpenCV_PATH}/Include/opencv
${GIRAFFECV_OpenCV_PATH}/Include/opencv2
$ENV{GIRAFFECV_DIR}/Projects/Include
$ENV{GIRAFFECV_DIR}/Projects/Include/VideoSource
$ENV{GIRAFFECV_DIR}/Projects/Include/VideoStitching
)


############################  Link Pathes List #############################

set(PANO_PLAYER_LINK_PATH_LIST
    $ENV{GIRAFFECV_DIR}/build/${OSFLAG}/${ARCHFLAG}/lib/${CMAKE_BUILD_TYPE}
    ${GIRAFFECV_LIBS_PATH}/${OSFLAG}/${ARCHFLAG}/lib/${CMAKE_BUILD_TYPE}
)

giraffecv_get_opencv_lib("core" "Release" LibOpenCVCore)
giraffecv_get_opencv_lib("highgui" "Release" LibOpenCVHighgui)
giraffecv_get_opencv_lib("imgcodecs" "Release" LibOpenCVImgcodecs)
giraffecv_get_opencv_lib("videoio" "Release" LibOpenCVVideoIO)
giraffecv_get_opencv_lib("imgproc" "Release" LibOpenCVImageProc)

giraffecv_get_opencv_lib("core" "Debug" LibOpenCVCoreD)
giraffecv_get_opencv_lib("highgui" "Debug" LibOpenCVHighguiD)
giraffecv_get_opencv_lib("imgcodecs" "Debug" LibOpenCVImgcodecsD)
giraffecv_get_opencv_lib("videoio" "Debug" LibOpenCVVideoIOD)
giraffecv_get_opencv_lib("imgproc" "Debug" LibOpenCVImageProcD)

include_directories(
${PANO_PLAYER_INCLUDE_PATH_LIST}
)

link_directories(
${PANO_PLAYER_LINK_PATH_LIST}
)

add_executable(PanoPlayer
main.cpp
mainwindow.cpp
GPanoRenderQWidget.cpp
mainwindow.ui
)

target_link_libraries(PanoPlayer
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
optimized ${LibOpenCVHighgui} debug ${LibOpenCVHighguiD}
optimized ${LibOpenCVImgcodecs} debug ${LibOpenCVImgcodecsD}
optimized ${LibOpenCVVideoIO} debug ${LibOpenCVVideoIOD}
optimized ${LibOpenCVImageProc} debug ${LibOpenCVImageProcD}
${QT_LIBRARIES}
PanoRenderOGL
VideoSource
)




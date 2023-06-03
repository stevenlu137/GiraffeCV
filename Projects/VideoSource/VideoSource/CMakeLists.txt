cmake_minimum_required(VERSION 2.8.12)

PROJECT (VideoSource)

include($ENV{GIRAFFECV_DIR}/cmake/BaseConfig.cmake)

if (UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility=hidden -DBOOST_LOG_DYN_LINK")
elseif(WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
endif(UNIX)


############################  Libs List ####################################

set(VIDEO_SOURCE_LIBS_LIST)
if (WIN32)
set(VIDEO_SOURCE_LIBS_LIST
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
optimized ${LibOpenCVImgproc} debug ${LibOpenCVImgprocD}
optimized ${LibOpenCVVideoIO} debug ${LibOpenCVVideoIOD}
avcodec.lib
avformat.lib
avutil.lib
swscale.lib
)

elseif(UNIX)
set(VIDEO_SOURCE_LIBS_LIST
pthread
boost_filesystem
boost_system
boost_thread
avcodec
avformat
avutil
swscale
)

endif(WIN32)

message(STATUS "VIDEO_SOURCE_LINK_PATH_LIST" ${VIDEO_SOURCE_LINK_PATH_LIST})
message(STATUS "VIDEO_SOURCE_LIBS_LIST" ${VIDEO_SOURCE_LIBS_LIST})

############################  Projects ####################################

######################  VideoSource ########################

if (OSFLAG STREQUAL "Windows")
ADD_LIBRARY(VideoSource SHARED
../../Common/Utils/GiraffeLogger/GiraffeLogger.cpp
Common/GCVideoSourceIF.cpp
Common/GVideoSource.cpp
)

SET_TARGET_PROPERTIES(VideoSource PROPERTIES COMPILE_FLAGS "-DG_VIDEO_SOURCE_BUILD -D__WIN_X64__")

TARGET_LINK_LIBRARIES(VideoSource
${VIDEO_SOURCE_LIBS_LIST}
)
elseif (OSFLAG STREQUAL "Linux")
ADD_LIBRARY(VideoSource SHARED
../../Common/Utils/GiraffeLogger/GiraffeLogger.cpp
Common/GCVideoSourceIF.cpp
Common/GVideoSource.cpp
)

SET_TARGET_PROPERTIES(VideoSource PROPERTIES COMPILE_FLAGS "-DG_VIDEO_SOURCE_BUILD")

TARGET_LINK_LIBRARIES(VideoSource
${VIDEO_SOURCE_LIBS_LIST}
)
endif(OSFLAG STREQUAL "Windows")
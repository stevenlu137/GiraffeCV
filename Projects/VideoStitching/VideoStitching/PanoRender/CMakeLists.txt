cmake_minimum_required(VERSION 2.8.12)

PROJECT (PanoRender)

include($ENV{GIRAFFECV_DIR}/cmake/BaseConfig.cmake)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility=hidden -fexceptions -frtti -DBOOST_LOG_DYN_LINK")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fvisibility=hidden -DBOOST_LOG_DYN_LINK")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
endif()

INCLUDE_DIRECTORIES(
${VIDEO_STITCHER_INCLUDE_PATH_LIST}
PanoEngine/Common
PanoEngine/FrameQueue
PanoEngine/PanoScene
PanoEngine/Render
PanoEngine/RenderParaManager
PanoRender
Testers
${GIRAFFECV_Oculus_PATH}/Include/Logging/include
)

############################  Libs List ####################################

set(PANO_RENDER_OGL_LIBS_LIST)
set(PANO_RENDER_OGL_TESTER_LIBS_LIST)
set(PANO_RENDER_OGL_TESTER_WIN32_LIBS_LIST)

if (OSFLAG STREQUAL "Windows")
set(PANO_RENDER_OGL_LIBS_LIST
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
glew32.lib
)

set(PANO_RENDER_OGL_TESTER_LIBS_LIST
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
optimized ${LibOpenCVHighgui} debug ${LibOpenCVHighguiD}
optimized ${LibOpenCVImgproc} debug ${LibOpenCVImgprocD}
optimized ${LibOpenCVVideoIO} debug ${LibOpenCVVideoIOD}
optimized ${LibOpenCVImgcodecs} debug ${LibOpenCVImgcodecsD}
glew32.lib
avcodec.lib
avformat.lib
avutil.lib
VideoSource.lib
PanoRenderOGL.lib
)

set(PANO_RENDER_OGL_TESTER_WIN32_LIBS_LIST
Comctl32.lib
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
optimized ${LibOpenCVHighgui} debug ${LibOpenCVHighguiD}
optimized ${LibOpenCVImgcodecs} debug ${LibOpenCVImgcodecsD}
optimized ${LibOpenCVImgproc} debug ${LibOpenCVImgprocD}
optimized ${LibOpenCVVideoIO} debug ${LibOpenCVVideoIOD}
glew32.lib
PanoRenderOGL.lib
VideoSource.lib
)

elseif(OSFLAG STREQUAL "Linux")
set(PANO_RENDER_OGL_LIBS_LIST
PRIVATE "-Wl,--no-undefined"
pthread
boost_serialization
boost_graph
boost_filesystem
boost_system
boost_log
boost_log_setup
GLEW.a
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
m
EGL
GL
z
-static-libgcc -static-libstdc++
)

set(PANO_RENDER_OGL_TESTER_LIBS_LIST
avcodec
avformat
avutil
VideoSource
PanoRenderOGL
pthread
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
optimized ${LibOpenCVHighgui} debug ${LibOpenCVHighguiD}
optimized ${LibOpenCVImgproc} debug ${LibOpenCVImgprocD}
optimized ${LibOpenCVVideoIO} debug ${LibOpenCVVideoIOD}
optimized ${LibOpenCVImgcodecs} debug ${LibOpenCVImgcodecsD}
m
GL
glut
GLEW
)

elseif(OSFLAG STREQUAL "Android")
set(PANO_RENDER_OGL_LIBS_LIST
pthread
boost_serialization
boost_graph
boost_filesystem
boost_system
boost_log
boost_log_setup
boost_thread
optimized ${LibOpenCVCore} debug ${LibOpenCVCoreD}
m
GL
)
endif()

message(STATUS "VIDEO_STITCHER_LINK_PATH_LIST" ${VIDEO_STITCHER_LINK_PATH_LIST})
message(STATUS "PANO_RENDER_OGL_LIBS_LIST" ${PANO_RENDER_OGL_LIBS_LIST})
message(STATUS "PANO_RENDER_OGL_TESTER_LIBS_LIST" ${PANO_RENDER_OGL_TESTER_LIBS_LIST})

############################  Projects ####################################


################################### PanoRenderOGL #####################################

ADD_LIBRARY(PanoRenderOGL SHARED
../../../Common/Utils/GiraffeLogger/GiraffeLogger.cpp
../Common/GFileUtils.cpp
PanoRender/GPanoRenderOGLIF.cpp
PanoRender/GPanoRenderOGL.cpp
PanoEngine/OpenGL/GOpenGLFrameBuffer.cpp
PanoEngine/OpenGL/GOpenGLPixelBuffer.cpp
PanoEngine/Render/GRender.cpp
PanoEngine/PanoScene/GGLSLProgramGenerator.cpp
PanoEngine/PanoScene/GRenderScene.cpp
PanoEngine/PanoScene/GSurfaceGenerator.cpp
PanoEngine/RenderParaManager/GViewCamera.cpp
PanoEngine/PanoScene/GProjectorObjects.cpp
PanoEngine/FrameQueue/GFrameQueueIF.cpp
PanoEngine/Render/GDataTransfer.cpp
PanoEngine/Render/GDownChannels.cpp
PanoEngine/RenderParaManager/GRenderParaManager.cpp
PanoEngine/Common/GGlobalParametersManager.cpp
PanoEngine/PanoScene/GPanoSceneIF.cpp
PanoEngine/PanoScene/GPanoScene.cpp
)

SET_TARGET_PROPERTIES(PanoRenderOGL PROPERTIES COMPILE_FLAGS "-DPANO_RENDER_OGL_BUILD -DG_PANO_RENDER_OGL_BUILD")

TARGET_LINK_LIBRARIES(PanoRenderOGL
${PANO_RENDER_OGL_LIBS_LIST}
)

################################### PanoRenderOGL_s #####################################

ADD_LIBRARY(PanoRenderOGL_s STATIC
../../../Common/Utils/GiraffeLogger/GiraffeLogger.cpp
../Common/GFileUtils.cpp
PanoRender/GPanoRenderOGLIF.cpp
PanoRender/GPanoRenderOGL.cpp
PanoEngine/OpenGL/GOpenGLFrameBuffer.cpp
PanoEngine/OpenGL/GOpenGLPixelBuffer.cpp
PanoEngine/Render/GRender.cpp
PanoEngine/PanoScene/GGLSLProgramGenerator.cpp
PanoEngine/PanoScene/GRenderScene.cpp
PanoEngine/PanoScene/GSurfaceGenerator.cpp
PanoEngine/RenderParaManager/GViewCamera.cpp
PanoEngine/PanoScene/GProjectorObjects.cpp
PanoEngine/FrameQueue/GFrameQueueIF.cpp
PanoEngine/Render/GDataTransfer.cpp
PanoEngine/Render/GDownChannels.cpp
PanoEngine/RenderParaManager/GRenderParaManager.cpp
PanoEngine/Common/GGlobalParametersManager.cpp
PanoEngine/PanoScene/GPanoSceneIF.cpp
PanoEngine/PanoScene/GPanoScene.cpp
)

if(ARCHFLAG STREQUAL "X64")
SET_TARGET_PROPERTIES(PanoRenderOGL_s PROPERTIES COMPILE_FLAGS "-DPANO_RENDER_OGL_BUILD -DG_PANO_RENDER_OGL_BUILD -DG_PANO_RENDER_OGL_STATIC -fPIC")
else()
SET_TARGET_PROPERTIES(PanoRenderOGL_s PROPERTIES COMPILE_FLAGS "-DPANO_RENDER_OGL_BUILD -DG_PANO_RENDER_OGL_BUILD -DG_PANO_RENDER_OGL_STATIC")
endif()

TARGET_LINK_LIBRARIES(PanoRenderOGL_s
${PANO_RENDER_OGL_LIBS_LIST}
)

if(OSFLAG STREQUAL "Windows")
################################### PanoRenderOGLTester #####################################

ADD_EXECUTABLE(PanoRenderOGLTester
Testers/GPanoRenderOGLTester.cpp
)

SET_TARGET_PROPERTIES(PanoRenderOGLTester PROPERTIES COMPILE_FLAGS "-DPANO_RENDER_OGL_TESTER_BUILD")

TARGET_LINK_LIBRARIES(PanoRenderOGLTester
${PANO_RENDER_OGL_TESTER_LIBS_LIST}
)

################################### PanoRenderOGLTester_Win32 #####################################

ADD_EXECUTABLE(PanoRenderOGLTester_Win32
Testers/GPanoRenderOGLTester_Win32.cpp
Testers/GTesterConfig.cpp
)

SET_TARGET_PROPERTIES(PanoRenderOGLTester_Win32 PROPERTIES COMPILE_FLAGS "-DPANO_RENDER_OGL_TESTER_WIN32_BUILD -D_WINDOWS" LINK_FLAGS "/SUBSYSTEM:WINDOWS /SAFESEH:NO")

TARGET_LINK_LIBRARIES(PanoRenderOGLTester_Win32
${PANO_RENDER_OGL_TESTER_WIN32_LIBS_LIST}
)

endif(OSFLAG STREQUAL "Windows")




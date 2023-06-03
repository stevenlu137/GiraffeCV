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
 
 
 
#ifndef G_PANO_RENDER_OGL_IF_H_
#define G_PANO_RENDER_OGL_IF_H_

#ifdef WIN32
#define G_CALL_CONVENTION __stdcall
#elif __APPLE__
#define G_CALL_CONVENTION
#elif __ANDROID__
#define G_CALL_CONVENTION
#elif __linux__
#define G_CALL_CONVENTION
#endif

#if defined _WIN32 || defined __CYGWIN__
#ifdef __GNUC__
#define G_DLL_IMPORT __attribute__ ((dllimport))
#define G_DLL_EXPORT __attribute__ ((dllexport))
#else
#define G_DLL_IMPORT __declspec(dllimport)
#define G_DLL_EXPORT __declspec(dllexport)
#endif
#else
#if __GNUC__ >= 4
#define G_DLL_IMPORT __attribute__ ((visibility ("default")))
#define G_DLL_EXPORT __attribute__ ((visibility ("default")))
#else
#define G_DLL_IMPORT
#define G_DLL_EXPORT
#endif
#endif

#ifndef G_PANO_RENDER_OGL_STATIC
#ifdef G_PANO_RENDER_OGL_BUILD
#define G_PANO_RENDER_OGL_API G_DLL_EXPORT
#else
#define G_PANO_RENDER_OGL_API G_DLL_IMPORT
#endif// G_PANO_RENDER_OGL_BUILD
#else
#define G_PANO_RENDER_OGL_API
#endif// G_PANO_RENDER_OGL_STATIC


typedef void(G_CALL_CONVENTION *G_tRenderCallBack)(void* pContext);
typedef void(G_CALL_CONVENTION *G_tOutputCallBack)(unsigned int uiWidth, unsigned int uiHeight, const unsigned char* pRGBData, void* pContext);

enum G_enPanoType
{
	G_ePanoTypeOriginal = 0,

	G_ePanoTypeImmersionSemiSphere = 1,
	G_ePanoTypeImmersionFullSphere = 10,
	G_ePanoTypeImmersionFullSphereInside = 11,
	G_ePanoTypeImmersionCylinder360 = 20,
	G_ePanoTypeImmersionCylinder360Inside = 21,

	G_ePanoTypeUnwrappedSphere180 = 500,
	G_ePanoTypeUnwrappedSphere360 = 501,
	G_ePanoTypeUnwrappedSphere360Flip = 502,
	
	G_ePanoTypeUnwrappedFullSphere360 = 510,
	G_ePanoTypeUnwrappedFullSphere360Flip = 512,
	G_ePanoTypeUnwrappedFullSphere360Binocular = 519,

	G_ePanoTypeUnwrappedCylinder180 = 520,
	G_ePanoTypeUnwrappedCylinder360 = 521,
	G_ePanoTypeUnwrappedCylinderSplited = 522,
	G_ePanoTypeUnwrappedCylinder180Flip = 523,
	G_ePanoTypeUnwrappedCylinder360Flip = 524,
	G_ePanoTypeUnwrappedCylinderSplitedFrontBack = 525,

	G_ePanoTypeMaxNum = 1024,
};

enum G_enInputFormat
{
	G_eInputFormatRGB = 0,
	G_eInputFormatYUV420P = 1,
	G_eInputFormatYV12 = 2,
	G_eInputFormatNV12 = 3,

	G_eInputFormatMaxNum = 512,
};

enum G_enPanoRenderErrorCode
{
	G_ePanoRenderErrorCodeNone = 0,
	G_ePanoRenderErrorCodeUnknown = -1,
	G_ePanoRenderErrorCodeInvalidPanoRenderIdx = -2,
	G_ePanoRenderErrorCodeUnsupportedOpenGLVersion = -3,

	G_ePanoRenderErrorCodeMaxNum = 4096,
};

enum G_enCameraModelType
{
	G_eCameraModelTypeNormal2r2t1r = 0,

	G_eCameraModelTypeFisheye4theta = 100,
	G_eCameraModelTypeFisheyeAnglePoly5 = 101,
};

enum G_enRenderingMode
{
	//With display output, real time.
	G_eRenderingModeOnScreen = 0,
	//Without display output, not real time. NOT IMPLEMENTED YET, use G_eRenderingModeOffScreenRealTime instead.
	G_eRenderingModeOffScreen = 1,
	//Without display output, real time.
	G_eRenderingModeOffScreenRealTime = 2,
};

struct G_tPanoRenderConfig
{
	G_enRenderingMode eRenderingMode;

	G_enInputFormat eInputFormat;
};

struct G_tViewCamera
{
// For immersive panorama types, fHorizontal/fVertical represent normalized phi/theta in the spherical coordinate system.
// fHorizontal: [0.0, 1.0] corresponds to phi: [0, 2*pi].
// fVertical: [0.0, 1.0] corresponds to theta: [0, pi].
// For unwrapped panorama types, fHorizontal/fVertical represent normalized pixel coordinates on the panorama canvas.
// fHorizontal: [0.0, 1.0] corresponds to [left border, right border].
// fVertical: [0.0, 1.0] corresponds to [top border, bottom border].
	float fHorizontal;
	float fVertical;

// Zoom scale factor.
	float fZoom;
};

struct G_tNormalizedPixelCoordinate
{
	float fX;
	float fY;
};

struct G_tPhysicalDirection
{
	float fTheta;
	float fPhi;
};

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

//*********************** Can ONLY be called once. *************************//

// pModel: pointer to the buffer containing the calibration file.
// iPanoRenderIdx: handle of the PanoRender object.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_CreatePanoRender(G_tPanoRenderConfig panoRenderConfig, const char* pModel, unsigned int uiModelSize_Byte, int* piPanoRenderIdx);

// Initialize the OpenGL rendering environment.
// iPanoRenderIdx: handle of the PanoRender object.
// hWnd: handle to the window that the OpenGL rendering environment will be initialized in.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_InitPanoRender(int iPanoRenderIdx, void* hWnd);

//*********************** Can be called dynamically. ************************//

// Get the supported panorama type list.
// iPanoRenderIdx: handle of the PanoRender object.
// pSupportedPanoTypeList: pointer to an array that will store the supported panorama types. The array should have a size greater or equal to G_ePanoTypeMaxNum.
// pSize: pointer to store the number of supported panorama types in the list.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetSupportedPanoTypeList(int iPanoRenderIdx, int* pSupportedPanoTypeList, int* pSize);

// Retrieve the current view camera parameters.
// iPanoRenderIdx: handle of the PanoRender object.
// pViewCamera: pointer to a G_tViewCamera structure that will store the view camera parameters.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetViewCamera(int iPanoRenderIdx, G_tViewCamera* pViewCamera);

// Obtain the panorama resolution that can be acquired by G_GrabPano().
// iPanoRenderIdx: handle of the PanoRender object.
// puiWidth: pointer to store the width of the original panorama.
// puiHeight: pointer to store the height of the original panorama.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetOriginalPanoSize(int iPanoRenderIdx, unsigned int* puiWidth, unsigned int* puiHeight);

// Set a new viewCamera to change the viewing angle, viewpoint, or zoom factor.
// iPanoRenderIdx: handle of the PanoRender object.
// viewCamera: a G_tViewCamera structure containing the new view camera parameters.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetViewCamera(int iPanoRenderIdx, G_tViewCamera viewCamera);

// Set the OpenGL drawing area.
// iPanoRenderIdx: handle of the PanoRender object.
// uiX0 / uiY0: coordinates of the upper left corner of the drawing area, relative to the current window.
// uiWidth / uiHeight: width and height of the drawing area.
// This function does not work in offline rendering mode, where the resolution is determined by the effective input resolution.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetDrawingRegion(int iPanoRenderIdx, unsigned int uiX0, unsigned int uiY0, unsigned int uiWidth, unsigned int uiHeight);

// Input frame data.
// This function is thread-safe and can be called in any thread at any time.
// iPanoRenderIdx: handle of the PanoRender object.
// iCameraIdx: index of the camera whose frame data is being input.
// uiWidth / uiHeight: width and height of the input frame.
// pData: array of pointers to the input frame data.
// uiStep_Byte: array of byte steps for each input frame data pointer.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_InputFrame(int iPanoRenderIdx, int iCameraIdx
		, unsigned int uiWidth, unsigned int uiHeight, void* pData[3], unsigned int uiStep_Byte[3]);

// Get the current panorama type.
// iPanoRenderIdx: handle of the PanoRender object.
// pCurrentPanoType: pointer to store the current panorama type.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetCurrentPanoType(int iPanoRenderIdx, int* pCurrentPanoType);

// Switch the current panorama type to a new one.
// iPanoRenderIdx: handle of the PanoRender object.
// eNewPanoType: the new panorama type to switch to. If it is not supported, the function simply returns.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SwitchPanoType(int iPanoRenderIdx, int eNewPanoType);

// Capture the currently displayed panorama.
// iPanoRenderIdx: handle of the PanoRender object.
// Output format: RGB image with original resolution (determined by effective input resolution).
// puiWidth/puiHeight: input & output argument. Input the resolution of the preallocated buffer, output the actual resolution.
// pRGBData: pointer to the preallocated buffer for the captured panorama.
// G_GrabPano will return a failure if the preallocated buffer size is not sufficient.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GrabPano(int iPanoRenderIdx, unsigned int* puiWidth, unsigned int* puiHeight, unsigned char* pRGBData);

// Set output callback.
// iPanoRenderIdx: handle of the PanoRender object.
// outputCallBackFunc: the output callback function to be set. Set to nullptr to cancel the callback.
// The output frame format is RGB.
// fScale: the ratio between the resolution of the output frame and the original resolution. 1.0 means the original resolution; negative values are not allowed.
// pContext: pointer to user-defined data to be passed to the callback function.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetOutputCallBack(int iPanoRenderIdx, G_tOutputCallBack outputCallBackFunc, float fScale, void* pContext);

// Set the pre-render callback.
// iPanoRenderIdx: handle of the PanoRender object.
// renderCallBackFunc: the pre-render callback function to be set. This function will be called before rendering.
// pContext: pointer to user-defined data to be passed to the callback function.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetPreRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext);

// Set the post-render callback.
// iPanoRenderIdx: handle of the PanoRender object.
// renderCallBackFunc: the post-render callback function to be set. This function will be called after rendering and is commonly used for post-processing.
// pContext: pointer to user-defined data to be passed to the callback function.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext);

// Calculate the corresponding physical directions from normalized pixel coordinates in the panorama. The computation is done in batches.
// iPanoRenderIdx: handle of the PanoRender object.
// pPoints/uiPointsSize: the normalized pixel coordinates.
// pPhysicalDirection: the output list of physical directions. An array of uiPointsSize should be pre-allocated.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_UnProject(int iPanoRenderIdx, G_tNormalizedPixelCoordinate* pPoints, unsigned int uiPointsSize, G_tPhysicalDirection* pPhysicalDirection);

// Calculate the G_tViewCamera needed to turn the view to the specified physical direction.
// iPanoRenderIdx: handle of the PanoRender object.
// pPhysicalDirection: the desired physical direction.
// pViewCamera: pointer to a G_tViewCamera structure that will store the calculated view camera parameters.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_CalcViewCamera(int iPanoRenderIdx, G_tPhysicalDirection pPhysicalDirection, G_tViewCamera* pViewCamera);

// Get camera intrinsic and extrinsic parameters.
// iPanoRenderIdx: handle of the PanoRender object.
// iCameraIdx: the index of the desired camera.
// sCameraMode: the camera has different parameters for different input resolutions. This argument is used to distinguish between different working modes
// of various resolutions. For example, for 1280x960, the string is "K0_1280_960". The prefix "K0_" is generic.
// fK, fDistortion, fR: the parameters of the camera. They will be interpreted differently depending on the camera model. Please refer to the instructions below.
// The caller should pre-allocate memory for fK, fDistortion, and fR.
// fK: float[9]
// fR: float[9]
// fDistortion: float[8]
// eCameraModelType: The camera model used by the specified camera.
// G_eCameraModelTypeNormal2r2t1r: fK is the 3x3 intrinsic matrix. fDistortion is the vector {r0, r1, t0, t1, r2}, where rk represents the radial distortion parameters,
// and tk represents the tangential distortion parameters. fR is the 3x3 rotation matrix.
// G_eCameraModelTypeFisheye4theta: fK and fR are the same as above. fDistortion is the distortion vector for fisheye cameras: {theta0, theta1, theta2, theta3}.
// The above two intrinsic matrices are both defined on the normalized pixel coordinate system, which has a value range of [0.0, 1.0].
// The camera models used above are consistent with those in OpenCV.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetCameraParameters(int iPanoRenderIdx, int iCameraIdx, char* sCameraMode, float* fK, float* fDistortion, float* fR, G_enCameraModelType* peCameraModelType);

//************************************* Destroy. ************************************************//

// Release the PanoRender object.
// iPanoRenderIdx: handle of the PanoRender object.
	G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_DestroyPanoRender(int iPanoRenderIdx);


#ifdef __cplusplus
}
#endif



#endif //!G_PANO_RENDER_OGL_IF_H_
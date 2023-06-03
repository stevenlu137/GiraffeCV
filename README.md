# Quick navigation
If you want to:

### Build a Panoramic Camera:
Please read the "Aquila User Manual" section below to calibrate your camera, and use the publicly released PanoPlayer to browse panoramic videos.

### Build a panoramic video application:
For those who don't want to read the documentation, there is nothing better than reading a simple sample program code directly. Please refer to the three sample programs under

* Projects/UI/PanoPlayer

* Projects/VideoStitching/VideoStitching/PanoRender/Testers

For more details, please refer to the PanoRender SDK Documentation bellow.

# VideoStitching
VideoStitching is a set of software technologies used to build multi-view panoramic cameras. The technology includes the mass production calibration tool **Aquila**, which is not open source and only releases a limited version of the binary program currently, and the open source high-performance real-time stitching module **PanoRenderOGL**.

Based on OpenGL technology, it implements an efficient stitching fusion algorithm that can be widely applied to various PCs and embedded environments. The algorithm has two output methods, either directly using OpenGL to draw in the window, or as an image processing module outputting panoramic frame data. This algorithm is one of the most efficient panoramic stitching algorithms in the world to date, and it can realize super high resolution real-time panoramic stitching fusion on ordinary mid-end PCs.

It supports the following features:

* Any number of cameras (Tested up to 24 cameras);
* Any type of camera (ordinary/fisheye) mixing;
* Any camera placement combination (dual fisheye/multi-eye 180 degrees/multi-eye 360 degrees/panoramic PTZ linkage, etc.);
* Instant initialization;
* Main and sub-stream real-time switching;
* Panoramic projection type real-time switching;
* PTZ linkage and partially zooming;
* Panoramic frame multi-resolution real-time output;
* Physical direction <-> panorama pixel coordinate forward and backward projection mapping;
* In VR applications, the algorithm also supports stereoscopic 3D panoramic stitching.


## Applicable scenarios and hardware requirements

VideoStitching is mainly designed for the **mass production of high-efficiency, high-resolution multi-camera panoramic video cameras**, so it is not suitable for some application scenarios such as VR movie production.

It only supports panoramic stitching in the situation where **multiple cameras are centered together**, the structure between the cameras needs to be compact, and it does not support stitching tasks under the situation where the distance between multiple positions is far. Generally speaking, we require a large overlapping field of view between adjacent cameras(for example, occupying 1/5 of their field of view), but if some special techniques are used, the stitching problem in the case of a small overlapping field of view can also be solved.


Real-time stitching works on the X86/X64 backend platform(In theory, it can run on any platform with OpenGL, but we only apply it to X86/X64 at present).The graphics card performance requirements are extremely low, and the integrated graphics of a common INTEL CPU can meet the stitching tasks of normal resolution.

It's best to synchronize frames and imaging parameters(such as exposure) between cameras by hardware, otherwise the picture consistency is poor and the best visual experience cannot be obtained.


# Aquila User Manual
The traditional panoramic image stitching process consists of two parts. One part calculates the geometric and optical parameters between the cameras through image registration, called calibration. The other part projects the original images from each camera onto the stitching surface and fuses them into a panoramic image, i.e., stitching. When the relative attitudes between the cameras are fixed and the optical parameters of the camera itself are fixed, the calibration process only needs to be done once. Therefore, we separate the two processes. At the factory, we carry out a calibration process in a strictly controlled environment. The output parameters are written to the device, and read out directly during stitching. This not only avoids the impact of calibration on the end user's experience but also ensures the stitching effect.

The calibration process is divided into two steps. The first step is to estimate parameters such as lens distortion, focal length, and the position of the optical axis on the sensor by collecting specific patterns shot by each camera. This step is called "internal parameter calibration". The second step is to estimate their relative spatial relationship, such as rotation angle, by collecting scenes of the same distant view shot by adjacent cameras, and calculate the optimal parameters by integrating the relationships between all cameras. This step is called "external parameter calibration". After calibration, the calculated result is a calibration file with only a few KB to tens of KB. This file records all the information for stitching the multi-channel video stream collected by this device into a panoramic view and will be used as the initialization input for PanoRender. We recommend that device manufacturers embed this file into the multi-sensor panoramic camera so that the device parameters can be directly obtained during use to complete the stitching.

The two calibration tasks mentioned above are all completed by the panoramic camera calibration tool Aquila. Aquila is a command-line tool, and this chapter will introduce the purpose and usage of each command in each section.


## Equipment and Environment Preparation
System requirements: A PC equipped with an NVIDIA graphics card. The network is smooth, and the panoramic device to be calibrated is connected.

Camera requirements: Focus on clear distant scenes and fix the focal length. After the calibration process starts, you may not adjust the focus or change the camera's hardware structure. In principle, the focus cannot be adjusted after calibration. Changing the focal length will destroy the calibration and cause larger gaps in the stitching.

The internal parameter calibration is carried out indoors. A checkerboard pattern of about 0.6m x 0.6m size is required (it can fill the camera's field of view at a clear close distance of the lens). The surface of the calibration plate needs to be flat, which is crucial to the accuracy of the calibration results.

During the external parameter calibration phase, the algorithm requires an open scene, such as a scene within 100 meters outside the window without obstacles, and complex textures of high-rise buildings or mountains beyond 100 meters.

We require a large field of view overlap between adjacent cameras to accurately complete the external parameter calibration. For the very special situations of no overlap or low overlap, Aquila also provides a solution, but this is not within the publicly released functions, and it is only considered to be applied to special needs.


## Internal Parameter Calibration
In the internal parameter calibration, we use each camera to collect about 20-40 unblurred photos from different angles, containing a complete checkerboard, for the algorithm to estimate the internal parameters of each camera. Due to the lens distortion effect, the checkerboard collected  is severely distorted, but after the algorithm estimates the internal parameters, the distortion can be removed. (It should be noted that the illustration here is only for demonstration, the actual operation process of the stitching algorithm is much more complicated than the distortion removal here, because it can be noticed that even after distortion removal, there is still residual distortion at the edge of the image, and the field of view has become smaller. The stitching algorithm will solve the problem here with more detailed processing methods)


## External Parameter Calibration
The calibration process is carried out for every two adjacent cameras with overlapping fields of view. The open scene is placed in the overlapping field of view of the two cameras. The image collection is done with the relevant command for this pair of cameras. The calibration program will calculate the matching relationship between the two camera images in the overlapping field of view and display it on the screen. If the matching relationship observed by the naked eye meets the following conditions:

* Clear without motion blur;
* Stitching is correct without errors;
* The number of matching points is relatively large.
* Matching points are sufficiently dispersed in the image;

Then use the command to accept this collection and proceed to the next pair of cameras for collection.


##Calibration Example
Let's take the four-channel 180-degree panoramic camera as an example to illustrate the use of Aquila.

Create a directory to store calibration tasks.

mkdir ~/calib_tasks

export G_CALIBRATION_TASKS=~/calib_tasks

Launch Aquila.exe.


We have two ways to start a calibration task. One is to start from scratch, and the other is to start from a template.

start from scratch:

**createtask taskname:Word**

create a new task.

Example:

  createtask task0

**vertex vertices:List**

Create a group of vertices. Cameras are treated as vertices in a graph here, they are equivalent concepts.

Example:

  vertex [cam0 cam1 cam2 cam3]


**panotype panoTypeName:Word cropSetting:List**

Add panotype.

panoTypeName: the name of specified panotype. A list of all available panorama types will be listed in a separate section.

cropSetting: [leftCropRatio rightCropRatio topCropRatio bottomCropRatio] for the unwrapped panoramas, the final panorama needs to be cropped. The 
crop ratio coefficients of the four edges are set by these four parameters. The default value is [0.0 0.0 0.0 0.0], meaning no cropping.

Example:

  panotype ImmersionSemiSphere
  
  panotype UnwrappedCylinder180 [0.1 0.1 0.1 0.1]


**mode sourceMain:Word sourceSub:Word cameraType:Word gridWidth:UInt gridHeight:UInt cameras:List<Word>**

Add camera mode (for IPCs, it's the concept of main stream/sub stream). This command is utilized to introduce resolutions other than the one 
used for intrinsic calibration (we call it main mode here). Typically, this command only needs to be set once in the calibration template. During 
the execution of the command, two chessboard calibrations will be performed on the main mode and the mode to be added.The algorithm will calculate 
the parameters of the mode to be added from the two calibration results and assign them to the specified cameras.

sourceMain: video source for main mode.

sourceSub: video source for the mode to be added.

cameraType: lens type. 'normal' or 'fisheye'.

gridWidth: Chessboard width (count of inner corner points, i.e., number of cells minus one)

gridHeight: Chessboard height (count of inner corner points, i.e., number of cells minus one)

cameras: a list of cameras to which the mode will be added.

Example:

  mode vs0main vs0sub fisheye 9 7 [0 1 2 3 4]

After these settings are complete, you can continue with the calibration task, or you can suspend the task with the suspendtask command and use it as a template.

**suspendtask**

Suspend the current task. The task will be serialized and saved on disk.

Example:

  suspendtask


**loadtask taskname:Word templatename:Word**

Create a new calibration task from the calibration template.

taskname: The unique identifier for the calibration task.
templatename: Usually we use a pre-set calibration task as a template to calibrate the same type of equipment, so as to avoid the same repeated settings in this type of equipment.


**source sourceName:Word url:URL**

Create a video source from a URL.

Example:

source vs0 rtsp://192.168.1.4:554/stream=0


**link vertex:Word sourceName:Word**

Link a camera vertex to a video source. The concepts of camera vertex and video source are decoupled for flexibility, so they need to be linked when necessary.

Example:

  link cam0 vs0


**calib cameraName:Word cameraType:Word gridWidth:UInt gridHeight:UInt clearData:Bool cameraInit:Option<Path>**

Estimate the intrinsic parameters of a camera using chessboard calibration.

cameraName: Name of the camera to be calibrated

cameraType: lens type. 'normal' or 'fisheye'.

gridWidth: Chessboard width (count of inner corner points, i.e., number of cells minus one)

gridHeight: Chessboard height (count of inner corner points, i.e., number of cells minus one)

clearData: If this value is set to 'true', it will clear the previous calibration data. It's used to restart the internal parameter calibration when the calibration fails.

cameraInit: Optional parameter. Executes a calibration yml file used to initialize camera parameters.
Example: 

calib cam0 fisheye 9 6 true

(Note: The chessboard should be entirely within the field of view. In order to get a better calibration effect, you should move the calibration board so that it is collected at the four corners and the center of the screen. The calibration algorithm will be automatically triggered when about twenty images are collected. To close the window and quit the calibration, press the "q" key.)


**srccrop cameraName:Word value:Float**

Set the cropping parameters after a calibration. The undistorted video frame image needs to be cropped to avoid large distortion on four corners.

cameraName: The name of the camera for which the cropping parameters need to be set, it can be fixed to -all, i.e., crop all cameras
value: Crop parameter, the larger this parameter is set, the more edge parts of the image will be preserved.

Example:

srccrop cam0 10.0

srccrop -all 10.0

(set for all cameras)

**match vertex1:Word vertex2:Word**

Match feature points of images acquired by two cameras. A successful match results in an edge (if there was no edge between these two cameras).

vertex1: The name of a certain camera in the panoramic camera.

vertex2: The name of another camera that has an overlapping field of view with vertex1.

Example:

The names of the 4 cameras are: cam0,cam1,cam2,cam3,

match cam0 cam1

match cam1 cam2

match cam2 cam3

(Note: After inputting the command and pressing Enter, wait a moment for the image to come out. Click on the image with the mouse to activate the window, and press any key to skip to the next video frame. When the details of the distant view in the overlap area are rich, fix the camera and press Enter to calculate the match. When the calculation is complete, the match situation will be displayed on the image. If there is no error in the match, press "y" to accept. If there is an error in the match, press any key to continue or press "q" to exit. If the number of matches is too small (less than 50), you can repeat the match between two cameras to accumulate data.)


**motion edge:Option**

This command is used to compute the geometric relationship between adjacent cameras with matching data.

edge: 'edge' refers to the overlap area between two cameras, named after the connected cameras separated by an underscore.

Example:

Calculate for cam0 - cam1: motion cam0_cam1

Calculate for cam1 - cam2: motion cam1 cam2

Calculate for cam2 - cam3: motion cam2 cam3

**calc algorithm:Word**

After all cameras' data collection, matching, and geometric computations are completed, this command is used to start calculating the global stitching coefficients.

algorithm: lse/tree. specify the algorithm used to calculate global stitching coefficients. generally speaking, the lse method gives more stable results.

Example:

calc lse


**opt**

This command is used to globally optimize the stitching coefficients.



**centralize method:Word cameras:Option<List<Word>>**

This command is used to set the center position of the panoramic image.

method: specify a centralize method.

* 'direct': if the axis of the global coordinate frame has been set by the 'setaxis' cmd, you can use this option to set the global coordinate   system frame.  
* '1cam': we just choose one camera coordinate frame as the global one.
* '2cam': calculates a new coordinate system frame between these two cameras, and uses it as the global coordinate system frame.
* 'all': method will construct the global coordinate frame from all camera coordinate frames. Usually, this coordinate frame conforms to our cognition of 'centering'.
  
cameras: List of camera names (only required for '1cam' and '2cam' methods).


Example for a 4-camera setup:

  centralize direct

  centralize 1cam [cam0]

  centralize 2cam [cam0 cam1]

  centralize all


**setaxis axisName:Word axisCfg:List<Tuple<Word,Word,Float>>**

Set the global coordinate frame using known coordinate axis vectors of each camera in the form of a weighted sum.

axisName: viewpoint/up ('viewpoint' refers to the direction vector of the line of sight, 'up' refers to the direction vector in the upward direction).

axisCfg: a list used to configure the weighted sum. The three fields in the tuple are: camera name, coordinate axis, and weight.

Example:

  setaxis up [(cam0 y 0.25) (cam1 y 0.25) (cam2 y 0.25) (cam3 y 0.25)]


**genprf vertices:List**

The final step of calibration process, generate the panoramic camera profile. It is used to provide input for the real-time stitching module.

vertices: a list of cameras that needed to export to panoCamProfile.

File name: panoCamProfile.bin

File location: The working directory of the program


**quit**

This command is used to exit the program.




# PanoRender SDK Documentation
## Overview
PanoRender is a real-time panoramic video stitching and fusion module based on OpenGL. Its main features include:

* Create PanoRender from stitching parameter files(panoCamProfile.bin), and initialize the OpenGL rendering environment.
* Video frame input interface. Video frames can be inputted by multiple independent threads, each responsible for a single video stream.
* Sub/main-stream dynamic switching mechanism. If the resolution of the input video frame switches between the sub/main stream, PanoRender automatically adapts to the new resolution.
* Various types of panoramas. In addition to immersive hemispherical/spherical panoramas, various unwrapped panoramas are also supported, including but not limited to spherical 180°/360° unwrapped, cylindrical unwrapped, binocular 3D panorama unwrapped, etc. Different panorama types can be dynamically switched by calling the API during runtime.
* Interactive control. Immersive panoramas require users to be able to freely change the viewing angle and zooming factor, while unwrapped panoramas require users to be able to freely zoom in and drag the panorama. These functions are implemented by the interactive control interface. The interface provides three controllable parameters, which the caller maps to the mouse's horizontal/vertical coordinates and scroll wheel, respectively, to implement changes in viewing angle or dragging, and zooming in.
* Screenshot and output callback mechanism. Screenshots can retrieve a frame of the panorama at its original resolution, while the output callback is called after any frame of the panorama is rendered, enabling the feedback of panoramic video. At the same time, the output callback can output any scaling of the original resolution.
* Rendering callback mechanism. To meet the caller's need to participate in the rendering process (such as drawing simple patterns in the panoramic image), PanoRender provides two rendering callback interfaces that are called at the start and end stages of the OpenGL rendering process, allowing users to insert their own rendering code.
* The linkage between PanoRenders and PTZ cameras. This allows us to achieve local zooming in another panorama with a different projection type or a video from a PTZ camera.
* PanoRender provides a readout interface for calibration parameters.
* On screen/off screen rendering mode. Compared to the on screen rendering mode that outputs to the screen, the off screen mode only provides callback output, which can provide a higher rendering frame rate.



## Operating Environment
PanoRender requires the support of OpenGL 2.1 or higher. But only on OpenGL 3.0 or above, PanoRender is full-featured and has better performance.

## Thread-related
PanoRender is designed to be thread-safe.

Due to the requirements of OpenGL rendering, the caller needs to set the pixel format of the window before using PanoRender. This part needs to be done in the main thread of the window; otherwise, errors will occur. You can refer to the sample program for the code to set the pixel format.


## Interface Description
### callback types

---

```c++
typedef void(G_CALL_CONVENTION G_tRenderCallBack)(void pContext);
```

Render callback. The rendering loop of PanoRender will call the callback function before and after each frame rendering. Users can call OpenGL to do simple rendering tasks in this callback, but please avoid calling PanoRender API functions again in this callback, or doing large overhead operations.

pContext: User-defined Context.

Refer to the documents of G_SetPreRenderCallBack(), G_SetRenderCallBack() functions.

---

```c++
typedef void(G_CALL_CONVENTION G_tOutputCallBack)(unsigned int uiWidth, unsigned int uiHeight, const unsigned char pRGBData, void* pContext);
```

Panorama frame output callback.

uiWeight: Width of output frame;
uiHeight: Height of output frame;
pRGBData: Pointer to output frame data;
pContext: User-defined Context.

Refer to the documents of G_SetOutputCallBack() function.


---

### enum types

```c++
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
};
```

Panoramic projection type.

Refer to the documents of G_SwitchPanoType() function.

---

```c++
enum G_enInputFormat
{
	G_eInputFormatRGB = 0,
	G_eInputFormatYUV420P = 1,
	G_eInputFormatYV12 = 2,
	G_eInputFormatNV12 = 3,

	G_eInputFormatMaxNum = 512,
};
```
Input video frame format.

Refer to the documents of struct G_tPanoRenderConfig.


---

```c++
enum G_enPanoRenderErrorCode
{
	G_ePanoRenderErrorCodeNone = 0,
	G_ePanoRenderErrorCodeUnknown = -1,
	G_ePanoRenderErrorCodeInvalidPanoRenderIdx = -2,
	G_ePanoRenderErrorCodeUnsupportedOpenGLVersion = -3,

	G_ePanoRenderErrorCodeMaxNum = 4096,
};
```

API function return value error code definition.

---

```c++
enum G_enCameraModelType
{
	G_eCameraModelTypeNormal2r2t1r = 0,

	G_eCameraModelTypeFisheye4theta = 100,
	G_eCameraModelTypeFisheyeAnglePoly5 = 101,
};
```
Camera model type.

Refer to G_GetCameraParameters() function documentation.

---

```c++
enum G_enRenderingMode
{
	//With display output, real time.
	G_eRenderingModeOnScreen = 0,
	//Without display output, not real time. NOT IMPLEMENTED YET, use G_eRenderingModeOffScreenRealTime instead.
	G_eRenderingModeOffScreen = 1,
	//Without display output, real time.
	G_eRenderingModeOffScreenRealTime = 2,
};
```

Rendering mode.
PanoRender supports both on-screen and off-screen rendering modes. Note that you can set the output callback in both modes, but for the sake of running efficiency, please choose the working mode that meets your actual needs.

Refer to the document of struct G_tPanoRenderConfig.

---

```c++
struct G_tPanoRenderConfig
{
	G_enRenderingMode eRenderingMode;

	G_enInputFormat eInputFormat;
};
```

PanoRender initialization configuration.

Please refer to the G_CreatePanoRender() function documentation.

---

```c++
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
```
The ViewCamera structure used for interactive control.
In interactive rendering mode, users control the viewpoint/view angle and zooming through mouse dragging, wheel scrolling, etc. Usually, we normalize the horizontal and vertical drag distance of the user's mouse, and the scrolling angle of the wheel, and map them to the three variables in this structure as the increment of the original view camera in PanoRender.

For different PanoTypes, the meanings of fHorizontal and fVertical are different, please see the comments for details.

Please refer to the G_GetViewCamera(),G_SetViewCamera(),G_CalcViewCamera() function documentation.

---

```c++
struct G_tNormalizedPixelCoordinate
{
	float fX;
	float fY;
};
```

Normalized pixel coordinates. That is, the range of fX,fY is [0,1].

Please refer to the G_UnProject() function documentation.

---

```c++
struct G_tPhysicalDirection
{
	float fTheta;
	float fPhi;
};
```

Defines a direction in the physical space where the panoramic camera is located. Defining this direction requires an X-Y-Z coordinate frame attached to the camera, which is set by the relevant commands of the calibration tool Aquila during calibration.

Please refer to the concept explanation of "physical direction" in the following text.

Please refer to the documentation of the setaxis, centralize and rotate commands in the Aquila manual.

Please refer to the G_UnProject(),G_CalcViewCamera() function documentation.

---

### Creation, Destruction, and Initialization of Rendering Environment
```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_CreatePanoRender(G_tPanoRenderConfig panoRenderConfig, 
                                                               const char* pModel,
                                                               unsigned int uiModelSize_Byte,
                                                               int* piPanoRenderIdx);
```
Create a PanoRender object.

panoRenderConfig: PanoRender configuration.

pModel: pointer to the buffer containing the content of the device-related calibration file (profile).

iPanoRenderIdx: Output the handle of the PanoRender object. The handle value is always greater than zero.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_InitPanoRender(int iPanoRenderIdx, void* hWnd);
```
Initialize the OpenGL rendering environment.

iPanoRenderIdx: handle of the PanoRender object.

hWnd: The handle of the rendering window. In principle, a window handle should not be required in off-screen rendering mode, but due to OpenGL limitations, we are not yet able to fully achieve this.

On Windows, even if a display window is not required, it is necessary to create a hidden window and use its handle to initialize PanoRender.

On Linux platforms, it can be set to nullptr in Offline mode.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_DestroyPanoRender(int iPanoRenderIdx);
```
Release the PanoRender object.

iPanoRenderIdx: handle of the PanoRender object.

---

### Panoramic Rendering, Interaction, Panoramic Type Switching, Output Callback and Rendering Callback
```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetSupportedPanoTypeList(int iPanoRenderIdx, int* pSupportedPanoTypeList, int* pSize);
```
Get the list of panoramic types supported by the current profile.

iPanoRenderIdx: handle of the PanoRender object.

pSupportedPanoTypeList: pointer to an array that will store the supported panorama types. The array should have a size greater or equal to G_ePanoTypeMaxNum.

pSize: Output the number of supported panoramic types.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetViewCamera(int iPanoRenderIdx, G_tViewCamera* pViewCamera);
```
Retrieve the current view camera parameters.

iPanoRenderIdx: handle of the PanoRender object.

pViewCamera: pointer to a G_tViewCamera structure that will store the view camera parameters.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetOriginalPanoSize(int iPanoRenderIdx, unsigned int* puiWidth, unsigned int* puiHeight);
```
Get the original resolution size of the panoramic frame. The panoramic image output by G_GrabPano() is of this size.

iPanoRenderIdx: handle of the PanoRender object.

puiWidth/puiHeight: pointer to store the width/height of the original panorama.

Before setting up the image return operation, you need to prepare a sufficiently large buffer, otherwise G_GrabPano() will fail. This interface is used to get the length and width of the panoramic image before G_GrabPano(). The returned panoramic image format is BGR, and the required memory bytes can be calculated by length * width * 3.

In very special cases, the size returned by this interface may not be consistent with the actual size at the time of capture, resulting in a capture failure. See the description of the G_GrabPano() interface for reasons.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetViewCamera(int iPanoRenderIdx, G_tViewCamera viewCamera);
```
Set a new viewCamera to change the viewing angle, viewpoint, or zoom factor.

iPanoRenderIdx: handle of the PanoRender object.

viewCamera: a G_tViewCamera structure containing the new view camera parameters.

In typical applications, the caller needs to first call G_GetViewCamera() to get the current parameters of the view camera, then map the mouse movement amount to the increment of the view camera parameters, add it to the current parameters of the view camera, and call G_SetViewCamera() to set it back to PanoRender.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_UnProject(int iPanoRenderIdx, G_tNormalizedPixelCoordinate* pPoints, unsigned int uiPointsSize, G_tPhysicalDirection* pPhysicalDirection);
```
Calculates a set of physical directions corresponding to the pixel from a set of normalized pixel coordinates in the panoramic image.

iPanoRenderIdx: handle of the PanoRender object.

pPoints/uiPointsSize: the normalized pixel coordinates.

pPhysicalDirection: the output list of physical directions. An array of uiPointsSize should be pre-allocated.

pPoints, uiPointsSize: A set of normalized pixel coordinates, i.e., the pixel coordinate system with the image length and width both being 1.0.

pPhysicalDirection: The output array of physical directions. The caller needs to pre-allocate the space of uiPointsSize size.

This is used for PTZ linkage or local zooming between two PanoRenders. For the definition of physical direction angle, refer to the section of terminology explanation below.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_CalcViewCamera(int iPanoRenderIdx, G_tPhysicalDirection pPhysicalDirection, G_tViewCamera* pViewCamera);
```
Calculate the G_tViewCamera needed to turn the view to the specified physical direction. This function can be used in conjunction with G_UnProject() to achieve local zooming.

iPanoRenderIdx: handle of the PanoRender object.

pPhysicalDirection: the desired physical direction.

pViewCamera: pointer to a G_tViewCamera structure that will store the calculated view camera parameters.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetDrawingRegion(int iPanoRenderIdx, unsigned int uiX0, unsigned int uiY0, unsigned int uiWidth, unsigned int uiHeight);
```
Set the OpenGL drawing area.

iPanoRenderIdx: handle of the PanoRender object.

uiX0 / uiY0: coordinates of the upper left corner of the drawing area, relative to the current window.

uiWidth / uiHeight: width and height of the drawing area.

This function does not work in offline rendering mode, where the resolution is determined by the effective input resolution.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_InputFrame(int iPanoRenderIdx, int iCameraIdx
                                                    	, unsigned int uiWidth, unsigned int uiHeight, void* pData[3], unsigned int uiStep_Byte[3]);
```
Input frame data.

iPanoRenderIdx: handle of the PanoRender object.

iCameraIdx: index of the camera whose frame data is being input. This number is defined when the factory-generated profile file is produced. See the explanation of terminology below for details.

uiWidth, uiHeight: Width and height of the input frame.

pData: The starting addresses of the data for the three channels of the input frame.

uiStep_Byte: Sometimes, to improve performance through memory alignment, decoding programs output image formats with line-end padding. This parameter specifies the line spacing.

This function is thread-safe and can be called in any thread at any time.

The data should be stored in memory in a row-priority manner.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetCurrentPanoType(int iPanoRenderIdx, int* pCurrentPanoType);
```
Get the current panorama type.

iPanoRenderIdx: handle of the PanoRender object.

pCurrentPanoType: pointer to store the current panorama type.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SwitchPanoType(int iPanoRenderIdx, int eNewPanoType);
```
Switch the current panorama type to a new one.

iPanoRenderIdx: handle of the PanoRender object.

eNewPanoType: the new panorama type to switch to. If it is not supported, the function simply returns.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GrabPano(int iPanoRenderIdx, 
                                                       unsigned int* puiWidth, unsigned int* puiHeight,
                                                       unsigned char* pRGBData);
```
Capture the currently displayed panorama.

iPanoRenderIdx: handle of the PanoRender object.

puiWidth, puiHeight: Input-output parameters. Input the resolution of the pre-allocated buffer, if the actual image is larger than this size, the capture fails. If the capture is successful, the actual size of the image is returned by these two parameters.

pRGBData: Pointer to pre-allocated buffer. 

Output format: RGB image with original resolution (determined by effective input resolution).

In special cases, if the panorama size is obtained by calling G_GetOriginalPanoSize() and before calling G_GrabPano() an event occurs that changes the panorama output resolution (e.g. input video stream has a sub/main stream switch, or the drawing area is reset, etc.), the panorama size might be larger than the size of the pre-allocated memory, causing the capture to fail.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetOutputCallBack(int iPanoRenderIdx, G_tOutputCallBack outputCallBackFunc, float fScale, void* pContext);
```
Set output callback.

iPanoRenderIdx: handle of the PanoRender object.

outputCallBackFunc: the output callback function to be set. Set to nullptr to cancel the callback.

fScale: the ratio between the resolution of the output frame and the original resolution. 1.0 means the original resolution; negative values are not allowed.

pContext: pointer to user-defined data to be passed to the callback function.

This function supports multiple channels for callback output at different resolutions. To achieve this, set multiple different callbacks with different fScale values.
Setting the outputCallBackFunc corresponding to a fScale value to a null pointer can cancel the callback function corresponding to the current fScale value.

The output frame format is RGB.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetPreRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext);
```
Set the pre-render callback.

iPanoRenderIdx: handle of the PanoRender object.

renderCallBackFunc: the pre-render callback function to be set. This function will be called before rendering.

pContext: pointer to user-defined data to be passed to the callback function.

This callback will be called before each panorama rendering.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_SetRenderCallBack(int iPanoRenderIdx, G_tRenderCallBack renderCallBackFunc, void* pContext);
```
Set the post-render callback.

iPanoRenderIdx: handle of the PanoRender object.

renderCallBackFunc: the post-render callback function to be set. This function will be called after rendering and is commonly used for post-processing.

pContext: pointer to user-defined data to be passed to the callback function.

This callback will be called after each panorama rendering is completed, allowing for further processing of the panorama in the frame buffer.

---

```c++
G_PANO_RENDER_OGL_API int G_CALL_CONVENTION G_GetCameraParameters(int iPanoRenderIdx, int iCameraIdx, char* sCameraMode, float* fK, float* fDistortion, float* fR, G_enCameraModelType* peCameraModelType);
```
Read out the camera calibration parameters.

iPanoRenderIdx: handle of the PanoRender object.

sCameraMode: the camera has different parameters for different input resolutions. This argument is used to distinguish between different working modes of various resolutions. For example, for 1280x960, the string is "K0_1280_960". The prefix "K0_" is generic. For more information about Camera Mode, please refer to the glossary below.

fK, fDistortion, fR: the parameters of the camera. They will be interpreted differently depending on the camera model. Please refer to the instructions below.The caller should pre-allocate memory for fK, fDistortion, and fR.

fK: float[9]

fR: float[9]

fDistortion: float[8]

eCameraModelType: The camera model used by the specified camera. The meanings of each value are as follows:

* G_eCameraModelTypeNormal2r2t1r: fK is the 3x3 intrinsic matrix. fDistortion is the vector {r0, r1, t0, t1, r2}, where rk represents the radial distortion parameters, and tk represents the tangential distortion parameters. fR is the 3x3 rotation matrix.
* G_eCameraModelTypeFisheye4theta: fK and fR are the same as above. fDistortion is the distortion vector for fisheye cameras: {theta0, theta1, theta2, theta3}.

The above two intrinsic matrices are both defined on the normalized pixel coordinate system, which has a value range of [0.0, 1.0].

The camera models used above are consistent with those in OpenCV, so the opencv documentation can be referred to.

---

## Glossary

* ***Profile***: This is the stitching parameter file of the device, which is generated by Aquila, at the time of manufacturing and is written to the device. These parameters are determined by the physical and geometric optical properties of specific cameras and cannot be mixed. During the real-time stitching phase, the profile is read out and sent to PanoRender, providing necessary information for PanoRender to create a panoramic rendering environment. Information such as the sub-stream resolution supported by the device and the list of panorama types are included in the profile.

* ***View Camera***: When browsing panoramic video, the user observes the panorama through a virtual camera. Changing the parameters of this camera can change parameters such as viewing angle, zooming, and observation point. In PanoRender's interface, the parameters of the viewing camera are simplified to three, which control the rotation around the central axis of the sphere, the pitch of the viewing angle and the zooming in immersive hemispherical/spherical panorama; and control the left and right movement, up and down movement and zooming in unwrapped panorama. The units of the parameters can be referred to the interface header file.

* ***Panorama Type***: Different ways of rendering panoramas are characterized by the concept of panorama type. Panorama types fall into two categories, immersive panorama and unwrapped panorama. In the immersive panorama, the user can change the viewing angle to observe the panorama from different angles, creating an immersive experience. The unwrapped panorama unfolds the panorama into a flat surface, where the viewing angle cannot be changed during observation, but the viewpoint can be changed.

* ***Input Camera***: The panoramic camera is composed of multiple cameras, each of which provides an input video stream to PanoRender. These cameras are referred to as input cameras. Input cameras have an index, which is used to distinguish different input cameras when inputting video frames. For an N-channel panoramic camera, the range of these numbers is [0,N-1], which is bound to each camera by Aquila when generating the profile.

* ***Original Panorama Resolution***: The algorithm calculates the original resolution of the panorama, which does not lose pixels due to small size nor is it interpolated due to large size at this resolution. The snapshot function provides a panorama of the original resolution, while the output callback provides a panorama with a scale relative to the original resolution.

* ***Camera Mode***: PanoRender supports real-time switching of multiple input resolutions. For each input resolution, there is a specific intrinsic parameter matrix K, and we call each different resolution a different camera mode. The K of each camera mode can be read out through the G_GetCameraParameters() interface, and different camera modes are indexed by a string, which is in the form of "K0_width_height", where width/height is the input resolution under this camera mode.
























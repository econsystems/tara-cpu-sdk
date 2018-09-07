///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2018, e-con Systems.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS.
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT/INDIRECT DAMAGES HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////
/**********************************************************************
	Tara.h : 	Declares the commonly used functions in the shared library.
	eDisparity: Declares the methods to compute disparity map, estimate 
				the depth of a point selected.
	TaraCamParameters: Declares the methods to load the calibrated 
				file, rectify images. 
	CameraEnumeration : Declares the methods to enumerate
				the camera devices connected to the 
				system and gives the resolutions supported.
**********************************************************************/
#ifndef _TARA_H
#define _TARA_H

// Linux Header Files:
#include <linux/videodev2.h>
#include <libv4l2.h>
#include <glib.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <iostream>
#include <limits>

//Extension unit header
#include "xunit_lib_tara.h"

//OpenCV headers
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/ximgproc/disparity_filter.hpp"

#define SDK_VERSION			"2.0.6"
#define FRAMERATE 			60
#define MASTERMODE 			1
#define TRIGGERMODE 			0
#define IOCTL_RETRY 			4
#define DEBUG_ENABLED 			0
#define DEFAULT_BRIGHTNESS 		(4.0/7.0)
#define AUTOEXPOSURE 			1 
#define DISPARITY_OPTION 		1 // 1 - Best Quality Depth Map and Lower Frame Rate
					  // 0 - Low  Quality Depth Map and High  Frame Rate

namespace Tara
{
//Displays the Text on the image passed
int DisplayText(cv::Mat Image, cv::String Text, cv::Point Location);

//ioctl with a number of retries in the case of failure
int xioctl(int fd, int IOCTL_X, void *arg);

class TaraCamParameters
{
public:

	cv::Mat Q;

	//Constructor
	TaraCamParameters(void);
	
	//Destructor
	~TaraCamParameters(void);
	
	//Initialises and reads the camera Matrix
	BOOL Init();
	
	//Rectifying the images
	BOOL RemapStereoImage(cv::Mat mCamLeftFrame, cv::Mat mCamRightFrame, cv::Mat *rLeftImage, cv::Mat *rRightImage);

private:

	//Maximum width and height of the camera supported
	int gImageWidth, gImageHeight;

	//Variables to incorporate the intrinsic and extrinsic files
	cv::Mat M1, D1, M2, D2;
	cv::Mat R, T;
	cv::Mat map11, map12, map21, map22;	
	
	//Loading the camera param
	BOOL LoadCameraMatrix();

	//to support lower version of OpenCV
	BOOL GetMatforCV(cv::Mat Src, cv::Mat *Dest);

	//Computes the Q matrix
	BOOL ComputeRectifyPrams();

};

class Disparity
{
public:

	//Constructor
	Disparity();

	//Destructor
	~Disparity();
		
	//Local values to be passed to the functions
	cv::Mat gDisparityMap, gDisparityMap_viz;
	cv::Mat DepthMap;

	//Initialises the camera
	BOOL InitCamera(bool GenerateDisparity, bool FilteredDisparityMap);

	//Grabs the frame, converts it to 8 bit, splits the left and right frame and returns the rectified frame
	BOOL GrabFrame(cv::Mat *LeftImage, cv::Mat *RightImage);

	//Estimates the disparity of the camera
	BOOL GetDisparity(cv::Mat LImage, cv::Mat RImage, cv::Mat *mDisparityMap, cv::Mat *disp_filtered);

	//Estimates the Depth of the point passed.
	BOOL EstimateDepth(cv::Point Pt, float *DepthValue);
	
	//Sets the exposure of the camera
	BOOL SetExposure(int ExposureVal);
	
	//Gets the exposure of the camera
	BOOL GetExposure(int *ExposureVal);

	//Sets the camera to auto exposure
	BOOL SetAutoExposure();

	//Sets the Brightness Val of the  camera
	BOOL SetBrightness(double BrightnessVal);
	
	//Sets the Stream Mode of the  camera
	BOOL SetStreamMode(UINT32 StreamMode);

	//Gets the Stream Mode of the camera
	BOOL GetStreamMode(UINT32 *StreamMode);

private:
	//Disparity algorithm
	cv::Ptr<cv::StereoBM> bm_left;
	cv::Ptr<cv::StereoMatcher> bm_right;

	cv::Ptr<cv::StereoSGBM> sgbm_left;
	cv::Ptr<cv::StereoMatcher> sgbm_right;
	cv::Ptr<cv::ximgproc::DisparityWLSFilter> wls_filter;
	
	//BM method Parameters for computing Disparity Map
	int bm_preFilterSize;
	int bm_preFilterCap;
	int bm_SADWindowSize;
	int bm_minDisparity;
	int bm_numberOfDisparities;
	int bm_textureThreshold;
	int bm_uniquenessRatio;
	int bm_speckleWindowSize;
	int bm_speckleRange;
	int bm_disp12MaxDiff;

	//SGBM method Parameters for computing Disparity Map
	int sgbm_preFilterCap;
	int sgbm_SADWindowSize;
	int sgbm_minDisparity;
	int sgbm_numberOfDisparities;
	int sgbm_uniquenessRatio;
	int sgbm_speckleWindowSize;
	int sgbm_speckleRange;
	int sgbm_disp12MaxDiff;

	double e_DWSLFLambda;
	double e_DWSLFSigma;
	double e_ScaleDispMap;
	double e_ScaleImage;

	//Range Selection
	double LIMIT(double n, double lower, double upper);

	//disparity option Selected
	int e_DisparityOption; //{ 0 - Stereo_BM, 1 - Stereo_SGBM }

	//Image Resolution
	cv::Size ImageSize;	

	//Option to generate Filtered Disparity or Without filter - USER CHOICE
	bool gFilteredDisparity;

	//Range map to convert to color
	cv::Mat mRange;
	std::vector<cv::Mat> StereoFrames;
	cv::Mat InputFrame10bit, InterleavedFrame;

	//DeviceID to stream the camera
	int DeviceID;
	
	//Object to hold Camera device
	cv::VideoCapture _CameraDevice;

	//Setting up the parameters of Disparity Algorithm
	BOOL SetAlgorithmParam();

	//Initialises the Camera Device with the passed width and height
	BOOL Init(bool GenerateDisparity);

	//Object to access the Q matrix connected
	TaraCamParameters _TaraCamParameters; 
};

class CameraEnumeration
{
private:

	//Video node properties
	typedef struct _VidDevice
	{
		char *device;
		char *friendlyname;
		char *bus_info;
		char *vendor;
		char *product;
		short int deviceID;
	} VidDevice;

	//Enumerated devices list
	typedef struct _LDevices
	{
		VidDevice *listVidDevices;
		int num_devices;
	} LDevices;

	//Stores the device instances of all enumerated devices
	LDevices *DeviceInstances;

	//Stores the resolution of selected camera device
	std::vector<cv::Size> CameraResolutions;

	//function for finding number of devices connected,friendly name
	int GetListofDeviceseCon(void);

	//To free the device list created
	void freeDevices(void);

	//Reads the device ID from the camera
	BOOL GetDeviceIDeCon(int *DeviceID, cv::Size *ResolutionSelected);

	//Query the resolution for selected camera.
	void query_resolution(int DeviceID);
	
public:

	char *DeviceInfo; 			//Bus info of the selected device

	//Constructor
	CameraEnumeration(int *DeviceID, cv::Size *ResolutionSelected);
	
	//Destructor
	~CameraEnumeration(void);
	//Check for stereo camera
	BOOL IsStereoDeviceAvail(char *pid);
	
};
}
#endif // _TARA_H

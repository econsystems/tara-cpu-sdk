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
////////////////////////  ///////////////////////////////////////////////////

/**********************************************************************
	Tara.cpp : Defines the functions in the shared library.
	Disparity: Defines the methods to compute disparity map, estimate 
				the depth of a point selected.
	TaraCamParameters: Defines the methods to load the callibrated 
				file, rectify images 
	CameraEnumeration : Enumerates the camera devices connected to the 
				system and gives the resolutions supported.
**********************************************************************/
#include "Tara.h"

#define DEFAULT_FRAME_WIDTH		640
#define DEFAULT_FRAME_HEIGHT		480

//using namespace cv;
using namespace std;
using namespace cv::ximgproc;

namespace Tara
{
//Constructor
TaraCamParameters::TaraCamParameters(void)
{
	//Default resolution(higher) used in case of remap
	gImageWidth  = 752;
	gImageHeight = 480;	
}

//Destructor
TaraCamParameters::~TaraCamParameters(void)
{
	gImageWidth  = -1;
	gImageHeight = -1;	
}

//Constructor
BOOL TaraCamParameters::Init()
{
	//Loads all the matrix related to the camera
	return LoadCameraMatrix();	
}

//Loading the camera param
BOOL TaraCamParameters::LoadCameraMatrix()
{
	unsigned char *IntrinsicBuffer, *ExtrinsicBuffer;
	int LengthIntrinsic, LengthExtrinsic;

	//Read the data from the flash
	if(StereoCalibRead(&IntrinsicBuffer, &ExtrinsicBuffer, &LengthIntrinsic, &LengthExtrinsic))
	{
		cout << "\nLoadCameraMatrix : Read Intrinsic and Extrinsic Files\n";
	}
	else
	{
		cout << "\nLoadCameraMatrix : Failed Reading Intrinsic and Extrinsic Files\n";
		return FALSE;
	}
	
	FILE *IntFile=NULL, *ExtFile=NULL;

	IntFile = fopen("//usr//local//tara-sdk//bin//intrinsics.yml", "wb");
	ExtFile = fopen("//usr//local//tara-sdk//bin//extrinsics.yml", "wb");

	if(IntFile == NULL || ExtFile == NULL)
	{
		cout << "LoadCameraMatrix : Failed Opening Intrinsic and Extrinsic Files\n";
		if(IntFile != NULL)
			fclose(IntFile);
		if(ExtFile != NULL)
	        	fclose(ExtFile);
	
		return FALSE;
	}

	if(LengthIntrinsic <= 0 || LengthExtrinsic <= 0)
	{
		cout << "LoadCameraMatrix : Invalid Intrinsic and Extrinsic File Length\n";
		fclose(IntFile);
		fclose(ExtFile);
		return FALSE;
	}

	fwrite(IntrinsicBuffer, 1, LengthIntrinsic, IntFile);
	fwrite(ExtrinsicBuffer, 1, LengthExtrinsic, ExtFile);

	fclose(IntFile);
	fclose(ExtFile);

	
	const char* intrinsic_filename = "//usr//local//tara-sdk//bin//intrinsics.yml";
	const char* extrinsic_filename = "//usr//local//tara-sdk//bin//extrinsics.yml";

	//reading intrinsic parameters
	cv::FileStorage fs(intrinsic_filename, CV_STORAGE_READ);
	if(!fs.isOpened())
	{
		if(DEBUG_ENABLED)
			cout << "LoadCameraMatrix : Failed Loading Intrinsic Data\n";
		return FALSE;
	}

    fs["M1"] >> M1;
    fs["D1"] >> D1;
    fs["M2"] >> M2;
    fs["D2"] >> D2;

	// reading Extrinsic parameters
	fs.open(extrinsic_filename, CV_STORAGE_READ);
	if(!fs.isOpened())
	{
		if(DEBUG_ENABLED)
			cout << "LoadCameraMatrix : Failed Loading Extrinsic Data\n";		
		return FALSE;
	}

    fs["R"] >> R;
    fs["T"] >> T;

	//Computes the Q Matrix from the Files loaded
	ComputeRectifyPrams();

	if(DEBUG_ENABLED)
		cout << "LoadCameraMatrix : Loaded Extrinsic and Intrinsic files..!!\n";

	return TRUE;
}

//Computing the Q Matrix
BOOL TaraCamParameters::ComputeRectifyPrams()
{
	if(DEBUG_ENABLED)
		cout << "Q Matrix Computation !!" << endl;
		
	cv::Mat R1, P1, R2, P2;
	cv::Rect roi1, roi2;
	cv::Size img_size(gImageWidth, gImageHeight);
	
	stereoRectify( M1, D1, M2, D2, img_size, R, T, R1, R2, P1, P2, Q, cv::CALIB_ZERO_DISPARITY, 0, img_size, &roi1, &roi2 );
	
	initUndistortRectifyMap(M1, D1, R1, P1, img_size, CV_16SC2, map11, map12);
	initUndistortRectifyMap(M2, D2, R2, P2, img_size, CV_16SC2, map21, map22);

	return TRUE;
}

//Rectifying the images
BOOL TaraCamParameters::RemapStereoImage(cv::Mat mCamLeftFrame, cv::Mat mCamRightFrame, cv::Mat *rLeftImage, cv::Mat *rRightImage)
{
	int actualWidth = mCamRightFrame.cols;
	int actualHeight = mCamRightFrame.rows;

	//Resize to the higher resolution and rectify the image
	if(actualWidth != (gImageWidth) ||actualHeight != gImageHeight)
	{
		resize(mCamLeftFrame,  mCamLeftFrame,  cv::Size(gImageWidth, gImageHeight));
		resize(mCamRightFrame, mCamRightFrame, cv::Size(gImageWidth, gImageHeight));
	}

	remap(mCamLeftFrame, *rLeftImage, map11, map12, cv::INTER_LINEAR);
	remap(mCamRightFrame, *rRightImage, map21, map22, cv::INTER_LINEAR);
	

	//Resize back to the original size
	if(actualWidth != gImageWidth || actualHeight != gImageHeight)
	{
		resize(*rRightImage, *rRightImage,  cv::Size(actualWidth, actualHeight));
		resize(*rLeftImage, *rLeftImage, cv::Size(actualWidth, actualHeight));
	}
		
	return TRUE;
}

//Constructor
Disparity::Disparity()
{
	//Image Resolutions
	ImageSize.width = 0;   //CHECk Width
	ImageSize.height = 0;

	//Default
	e_DisparityOption = 1;
}

//Destructor
Disparity::~Disparity()
{
	//Release the camera device
	_CameraDevice.release();

	//Relase the vector
	vector<cv::Mat>().swap(StereoFrames);

	//Deinitialise the extension unit
	DeinitExtensionUnit();
}

BOOL Disparity::InitCamera(bool GenerateDisparity, bool FilteredDisparityMap)
{
	//Device ID thats to be streamed
	int DeviceID;

	cout << "SDK-Version : " << SDK_VERSION << endl;
	
	//Read the device ID to stream
	CameraEnumeration _CameraEnumeration(&DeviceID, &ImageSize);
	
	if(DeviceID < 0)//Check for a valid device ID
	{

		cout << "InitCamera : Please select a valid device\n";
		return FALSE;
	}

	//Open the device selected by the user.
	_CameraDevice.open(DeviceID);
		
	//Camera Device
	if(!_CameraDevice.isOpened())
	{			
		cout << "InitCamera : Camera opening failed\n";
		return FALSE;
	}
	
	if (DEFAULT_FRAME_WIDTH == ImageSize.width && DEFAULT_FRAME_HEIGHT == ImageSize.height)
	{
		_CameraDevice.set(CV_CAP_PROP_FRAME_WIDTH, 752);
		_CameraDevice.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	}

	//Setting up Y16 Format
	_CameraDevice.set(CV_CAP_PROP_FOURCC, CV_FOURCC('Y', '1', '6', ' '));

	//Setting up FrameRate
	_CameraDevice.set(CV_CAP_PROP_FPS, FRAMERATE);

	//Setting width and height
	_CameraDevice.set(CV_CAP_PROP_FRAME_WIDTH, ImageSize.width);
	_CameraDevice.set(CV_CAP_PROP_FRAME_HEIGHT, ImageSize.height);
	
	//y16 format support
	_CameraDevice.set(CV_CAP_PROP_CONVERT_RGB, 0);

	//Init the extension units
	if(!InitExtensionUnit(_CameraEnumeration.DeviceInfo))
	{			

		cout << "InitCamera : Extension Unit Initialisation Failed\n";
		return FALSE;
	}
	
	//Setting up the camera in Master mode
	if(!SetStreamModeStereo(MASTERMODE))
	{			
		cout << "InitCamera : Setting up Stream Mode Failed, initiating in the default mode\n";
	}

	//Setting to default Brightness
	SetBrightness(DEFAULT_BRIGHTNESS);

	//setting up auto Exposure
	SetExposure(SEE3CAM_STEREO_EXPOSURE_DEF);

	//Choose whether the disparity is filtered or not
	gFilteredDisparity = FilteredDisparityMap;

	//Initialise the disparity options
	if(!Init(GenerateDisparity))
	{
		if(DEBUG_ENABLED)
			cout << "InitCamera : Camera Matrix Initialisation Failed\n";
		return FALSE;
	}

	//Mat creation
	InterleavedFrame.create(ImageSize.height, ImageSize.width, CV_8UC2);

	return TRUE;
}

//Grabs the frame, converts it to 8 bit, splits the left and right frame and returns the rectified frame
BOOL Disparity::GrabFrame(cv::Mat *LeftImage, cv::Mat *RightImage)
{
	//Read the frame from camera
	//Y16 ==> CV_16UC1 2
	_CameraDevice.read(InputFrame10bit); 
		
	//Invalid Frame
	if(InputFrame10bit.empty())
	{
		cout << "\nGrabFrame : No Frame Received! Camera is Unavailable!\n";
		return FALSE;
	}
	//copy the data to the two channel image
	InterleavedFrame.data = InputFrame10bit.data;
			
	//Splitting the data
	split(InterleavedFrame, StereoFrames);

	//Rectify Frames
	_TaraCamParameters.RemapStereoImage(StereoFrames[0], StereoFrames[1], LeftImage, RightImage);

	return TRUE;
}

//initialise all the variables and create the Disparity parameters
BOOL Disparity::Init(bool GenerateDisparity) 
{
	//Init to read the Camera Matrix
	if(!_TaraCamParameters.Init())
	{
		if(DEBUG_ENABLED)
			cout << "Init : Camera Matrix Initialisation Failed\n";
		return 0;
	}

	//Depth Map 
	if(_TaraCamParameters.Q.empty()) //Matrix loading failed
	{
		if(DEBUG_ENABLED)
			cout << "Init : Q matrix Computation Failed \n";
		return FALSE;
	}

	//Copying to the local value
	DepthMap = _TaraCamParameters.Q;

	//Initialises only when the disparity option is enabled
	if(GenerateDisparity)
	{
		//BM method Parameters for computing Disparity Map
		bm_preFilterSize	=	5;
		bm_preFilterCap		=	25;
		bm_SADWindowSize	=	20; // must be odd, within 5..255 and not larger than image width or height
		bm_minDisparity		=	0;	
		bm_textureThreshold	=	5;
		bm_uniquenessRatio	=	1;	
		bm_speckleRange		=	31;
		bm_disp12MaxDiff	=	1;
		bm_numberOfDisparities	=	4;  // = val * 16
		bm_speckleWindowSize	=	350;

		//SGBM method Parameters for computing Disparity Map
		sgbm_preFilterCap	=	61;
		sgbm_SADWindowSize	=	8;
		sgbm_minDisparity	=	0; 
		sgbm_speckleRange	=	31;
		sgbm_disp12MaxDiff	=	1;	
		sgbm_uniquenessRatio	 =	0; 
		sgbm_speckleWindowSize	 =	200;
		sgbm_numberOfDisparities =	4;

		e_DWSLFLambda	=	8000.0;
		e_DWSLFSigma	=	1.5;
		e_ScaleDispMap	=	5.0;
		e_ScaleImage	=	0.60;
	
		if(DISPARITY_OPTION) // Disparity Map Quality and Frame Rate 
		{
			e_DisparityOption = 1;  // 1 - Stereo_SGBM
		}
		else
		{
			e_DisparityOption = 0;  // 0 - Stereo_BM
		}
	
		mRange = cv::Mat(cv::Size(50, ImageSize.height), CV_8UC1);
		for (int Row = 0; Row < ImageSize.height; Row++)
		{
			const float scaleR = float(255.0 / (float)ImageSize.height);
			mRange.row(Row).setTo(Row * scaleR);
		}

		//Algorithm Parameters
		SetAlgorithmParam();
	}	

	return TRUE;
}

//Setting up the parameters of Disparity Algorithm
BOOL Disparity::SetAlgorithmParam()
{	
	if(DEBUG_ENABLED)
		cout << "SetAlgorithmParam : Setting Up the Algorithm Parameters\n";
	int numberOfDisparities;
	if(!e_DisparityOption)  //STEREO_BM algorithm
	{
		bm_preFilterSize	=	int(LIMIT(bm_preFilterSize, 5, 63));
		bm_SADWindowSize	=	MAX(5, bm_SADWindowSize);
		if(bm_SADWindowSize % 2 == 0)
		{
			bm_SADWindowSize++;
		}

		bm_preFilterCap = int(LIMIT(bm_preFilterCap, 1, 63)); // must be within 1 and 63		

		if(bm_preFilterSize % 2 == 0)
		{
			bm_preFilterSize++;
		}

		numberOfDisparities = bm_numberOfDisparities * 16;
		numberOfDisparities = numberOfDisparities > 0 ? numberOfDisparities : ((ImageSize.width/8) + 15) & -16;
	
		bm_left = cv::StereoBM::create(numberOfDisparities, bm_SADWindowSize > 0 ? bm_SADWindowSize : 9);

		if(gFilteredDisparity) //Gives a Filtered Disparity
		{
			wls_filter = createDisparityWLSFilter(bm_left);//For Filtering Disparity Map
			bm_right   = createRightMatcher(bm_left);
		}

		bm_left->setROI1(cv::Rect(0, 0, ImageSize.width, ImageSize.height)); //CHECk Width
		bm_left->setROI2(cv::Rect(0, 0, ImageSize.width, ImageSize.height)); //CHECk Width
		bm_left->setPreFilterCap(bm_preFilterCap);
		bm_left->setBlockSize(bm_SADWindowSize > 0 ? bm_SADWindowSize : 9);
		bm_left->setMinDisparity(bm_minDisparity);
		bm_left->setNumDisparities(numberOfDisparities);
		bm_left->setTextureThreshold(bm_textureThreshold);
		bm_left->setUniquenessRatio(bm_uniquenessRatio);
		bm_left->setSpeckleWindowSize(bm_speckleWindowSize);
		bm_left->setSpeckleRange(bm_speckleRange);
		bm_left->setDisp12MaxDiff (bm_disp12MaxDiff);
		bm_left->setPreFilterType(CV_STEREO_BM_XSOBEL);
	}
	else //STEREO_3WAY
	{
		numberOfDisparities = sgbm_numberOfDisparities * 16;
		numberOfDisparities = numberOfDisparities 	> 0 ? numberOfDisparities  : ((ImageSize.width/8) + 15) & -16;

		sgbm_left = cv::StereoSGBM::create(0, numberOfDisparities, sgbm_SADWindowSize > 0 ? sgbm_SADWindowSize : 3);
	
		if(gFilteredDisparity) //Gives a Filtered Disparity
		{
			wls_filter = createDisparityWLSFilter(sgbm_left);
			sgbm_right = createRightMatcher(sgbm_left);
		}	

		int cn = 1;
		sgbm_left->setPreFilterCap(sgbm_preFilterCap);
		sgbm_left->setBlockSize (sgbm_SADWindowSize > 0 ? sgbm_SADWindowSize : 3);
		sgbm_left->setP1(8 * cn * sgbm_SADWindowSize * sgbm_SADWindowSize);
		sgbm_left->setP2(32 * cn * sgbm_SADWindowSize * sgbm_SADWindowSize);
		sgbm_left->setNumDisparities(numberOfDisparities);
		sgbm_left->setMinDisparity(sgbm_minDisparity);
		sgbm_left->setUniquenessRatio(sgbm_uniquenessRatio);
		sgbm_left->setSpeckleWindowSize(sgbm_speckleWindowSize); 
		sgbm_left->setSpeckleRange(sgbm_speckleRange);
		sgbm_left->setDisp12MaxDiff(sgbm_disp12MaxDiff);
	
		sgbm_left->setMode(cv::StereoSGBM::MODE_SGBM_3WAY);		
	}

	if(gFilteredDisparity) //Gives a Filtered Disparity
	{
		wls_filter->setLambda(e_DWSLFLambda);
		wls_filter->setSigmaColor(e_DWSLFSigma);		
	}

	return TRUE;
}

//Estimates the disparity of the camera
BOOL Disparity::GetDisparity(cv::Mat LImage, cv::Mat RImage, cv::Mat *mDisparityMap, cv::Mat *FilteredDisparity)
{
	cv::Mat LeftDisparity, RDisparity;
	cv::Mat LScaleImage, RScaleImage;
	cv::Mat disp_filtered, mDisp_WR, disparity_viz;
	cv::Mat ScaledDisparityMap_viz, ScaledDisparityMap;
	
	//Scale value
	e_ScaleImage = LIMIT(e_ScaleImage, 0.20, 1);
	
	if(e_ScaleImage != 1.0) //Scaling the Input to speed up the process
	{
		resize(LImage, LImage, cv::Size(), e_ScaleImage, e_ScaleImage, cv::INTER_AREA);
		resize(RImage, RImage, cv::Size(), e_ScaleImage, e_ScaleImage, cv::INTER_AREA);
	}
	 
	if(!e_DisparityOption)  //STEREO_BM algorithm
	{
		bm_left->compute(LImage, RImage, gDisparityMap);

		if(gFilteredDisparity) //Filtered disparity
		{
			bm_right->compute(RImage, LImage, RDisparity);
		}
	}
    else //STEREO_3WAY algorithm
	{
		sgbm_left->compute(LImage, RImage, gDisparityMap);

		if(gFilteredDisparity) //Filtered disparity
		{
			sgbm_right->compute(RImage, LImage, RDisparity);
		}
	}

	if(gFilteredDisparity) //filtered
	{
		wls_filter->setLambda(e_DWSLFLambda);
		wls_filter->setSigmaColor(e_DWSLFSigma);
		wls_filter->filter(gDisparityMap, LImage, disp_filtered, RDisparity);
		
		gDisparityMap = disp_filtered.clone();

	}

	//Disparity map to view
	getDisparityVis(gDisparityMap, *mDisparityMap, e_ScaleDispMap);

	if(e_ScaleImage  != 1.0) //Scale back the output image
	{			
		resize(gDisparityMap, gDisparityMap, cv::Size(ImageSize.width, ImageSize.height));
		resize(*mDisparityMap, *mDisparityMap, cv::Size(ImageSize.width, ImageSize.height));
	}
		
	//Color map for the Disparity image
	hconcat(*mDisparityMap, mRange, mDisp_WR);
	applyColorMap(mDisp_WR, *FilteredDisparity, cv::COLORMAP_JET);
	
	return TRUE;
}


//Estimates the Depth of the point passed.
BOOL Disparity::EstimateDepth(cv::Point Pt, float *DepthValue)
{
	cv::Mat_<float> vec(4, 1);
	cv::Mat disp_leftCrop, disp_32;
	cv::Mat Q_32;

	//Validate the point
	if(Pt.x <= 0  || Pt.y <= 0 || Pt.x > ImageSize.width || Pt.y > ImageSize.height)
	{
		*DepthValue = -1;
		return FALSE;
	}

	cv::Point WithinImage(-1, -1);

	//Handling points in the corner
	if((Pt.x + 20) > ImageSize.width)
	{
		WithinImage.x = ( ImageSize.width - (Pt.x + 20)) + Pt.x;
		WithinImage.y = Pt.y;
	}

	if((Pt.y + 20) > ImageSize.height)
	{
		WithinImage.y = ( ImageSize.height - (Pt.y + 20)) + Pt.y;
		if(WithinImage.x == -1) //In case x is within the range
			WithinImage.x = Pt.x;
	}

	if(WithinImage.x == -1 || WithinImage.y == -1)
	{
		WithinImage.x = Pt.x;
		WithinImage.y = Pt.y;
	}

	//Average depth of 20x20 around the selected point is taken
	cv::Rect recROI(WithinImage.x, WithinImage.y, 20, 20);
	
	//Cropping the region
	disp_leftCrop = gDisparityMap(recROI);

	//Converting to 32 bit 
	disp_leftCrop.convertTo(disp_32, CV_32FC1, 1.0 / 16); //CHANGES MADE HERE

	//Computing the mean of the selected point
	cv::Scalar MeanDisp = mean(disp_32);

	//Converting to 32 bit 
	DepthMap.convertTo(Q_32, CV_32FC1);
	
	//Intialising the Mat
	vec(0) = 0;
	vec(1) = 0;
	vec(2) = (float)MeanDisp.val[0];
	
	// Discard points with 0 disparity    
	if(vec(2) != 0)
	{
		vec(3) = 1.0;              
		vec = Q_32 * vec;		
		vec /= vec(3);

		// Discard points that are too far from the camera, and thus are highly unreliable
		*DepthValue = vec(2) * float(e_ScaleImage);
	}

	return TRUE;
}

//Range Selection
double Disparity::LIMIT(double n, double lower, double upper) 
{
	return max(lower, min(n, upper));
}

//Sets the exposure of the camera
BOOL Disparity::SetExposure(int ExposureVal)
{
	if(!SetManualExposureStereo(ExposureVal)) //Set the manual exposure
	{
		if(DEBUG_ENABLED)
			cout << "SetExposure : Exposure Setting Failed\n";
		return FALSE;
	}
	return TRUE;
}

//Sets the exposure of the camera
BOOL Disparity::GetExposure(int *ExposureVal)
{
	if(!GetManualExposureStereo(ExposureVal)) //Get the manual exposure
	{
		if(DEBUG_ENABLED)
			cout << "GetExposure : Exposure Getting Failed\n";
		return 0;
	}
	return 1;
}

//Sets the exposure of the camera
BOOL Disparity::SetAutoExposure()
{
	UINT32 StreamMode = -1;
	GetStreamMode(&StreamMode); //Read the stream mode

	if(StreamMode == TRIGGERMODE)
	{
		cout << endl << "Switch to Master Mode to set Auto Exposure!!" << endl;
	}
	else
	{
		int CurrentExpValue = 0;
		GetExposure(&CurrentExpValue); //Read the exposure value
		
		if(CurrentExpValue != AUTOEXPOSURE)
		{	
			//Setting up the exposure
			if(SetAutoExposureStereo())
			{
				cout << endl << "Switching to Auto Exposure!!" << endl;
			}
			else
			{
				cout << endl << "Failed!! Switching to Auto Exposure!!" << endl;
			}
		}
		else
		{
			cout << endl << "Already in Auto Exposure!!" << endl;
		}
	}

	return TRUE;
}

//Sets the Brightness Val of the  camera
BOOL Disparity::SetBrightness(double BrightnessVal)
{
	//Sets the brightness of the Camera
	return _CameraDevice.set(CV_CAP_PROP_BRIGHTNESS, BrightnessVal);	 
}

//Sets the Stream Mode of the  camera
BOOL Disparity::SetStreamMode(UINT32 StreamMode)
{
	UINT32 CurrentMode = -1;
	GetStreamModeStereo(&CurrentMode); 
	
	//checking up if the selected mode is Trigger mode
	if(CurrentMode != StreamMode)
	{
		if(StreamMode == TRIGGERMODE)
		{
			int ExposureValue = 0;
			GetManualExposureStereo(&ExposureValue);
			
			if(ExposureValue == AUTOEXPOSURE) //Check whether it is in Auto Exposure
				SetExposure(SEE3CAM_STEREO_EXPOSURE_DEF);
			
			cout << endl << "Changing to Manual Exposure to set to Trigger Mode" << endl;		
		}
		
		SetStreamModeStereo(StreamMode);
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

//Gets the Stream Mode of the camera
BOOL Disparity::GetStreamMode(UINT32 *StreamMode)
{	
	//Read the current stream mode
	GetStreamModeStereo(StreamMode); 
	return TRUE;
}

//Constructor
CameraEnumeration::CameraEnumeration(int *DeviceID, cv::Size *SelectedResolution)
{
	//Gets the Input from the user
	GetDeviceIDeCon(DeviceID, SelectedResolution);
}

//Destructor
CameraEnumeration::~CameraEnumeration()
{
	//Freeing the memory allocated for devices list.
	freeDevices();
	
	//Free the vectors
	vector<cv::Size>().swap(CameraResolutions);
}

//Reads the Input from the user
BOOL CameraEnumeration::GetDeviceIDeCon(int *DeviceID, cv::Size *SelectedResolution)
{
	if(DEBUG_ENABLED)
		cout << "Get DeviceID eCon" << endl;
			
	short int index=-1, NoDevicesConnected = -1;
	int ResolutionID = -1;
	
	//Get the list of devices connected
	NoDevicesConnected = GetListofDeviceseCon();
	if(DEBUG_ENABLED)
		cout << "\nNumber of connected devices : " << NoDevicesConnected << endl;

	//Check for a valid ID
	if(NoDevicesConnected <= 0)
	{
		cout << "No devices connected\n\n";
		*DeviceID = -1;
		return FALSE;
	}

	//Print the name of the devices connected
	cout << "\nDevices List :\n";
	cout << "----------------\n";
	for(int i = 0; i < NoDevicesConnected; i++)
	{
		cout << "Device ID: " << DeviceInstances->listVidDevices[i].deviceID << ", Device Name: " << DeviceInstances->listVidDevices[i].friendlyname << endl;
	}

	//User Input of the Device ID
	cout << "\nEnter the Device ID to Process : ";
	cin >> *DeviceID;

	//Check for a valid ID
	if(*DeviceID < 0)
	{
		cout << "Please enter a valid Device Id\n\n";
		*DeviceID = -1;
		return FALSE;
	}

	//Finding the index of the selected device ID
	for(int i = 0; i < NoDevicesConnected; i++)
    {
		if(DeviceInstances->listVidDevices[i].deviceID == *DeviceID)
		{
			index = i;
			break;
		}		
    }

	//Check for a valid ID
	if(index == -1)
	{
		cout << "Please enter a valid Device Id\n\n";
        *DeviceID = -1;
        return FALSE;
	}		
	

	//Check whether the selected device is Stereo
	if(!IsStereoDeviceAvail(DeviceInstances->listVidDevices[index].product))
	{
		cout << "Please select a stereo camera\n\n";
		*DeviceID = -1;
		return FALSE;
	}

	cout << "\nResolutions Supported : \n"; 
	cout << "-------------------------\n";	
	query_resolution(index);

	//Resolution Supported
	for(unsigned int i = 0; i < CameraResolutions.size(); i++)
	{
		cout << "ID: " << i << ", Resolution: " << CameraResolutions[i].width << "x" <<CameraResolutions[i].height << endl;
	}

	//User Input of the Device ID
	cout << "\nEnter the Resolution ID to Stream : ";
	cin >> ResolutionID;

	if(ResolutionID > int(CameraResolutions.size() - 1) || ResolutionID < 0) //In case of wrong selection
	{
		cout << "\nInvalid ResolutionID\n";
		*DeviceID = -1;
		return FALSE;
	}

	//Bus info of the selected device.
	DeviceInfo = DeviceInstances->listVidDevices[index].bus_info;
		
	//Selected Resolution
	SelectedResolution->width = CameraResolutions[ResolutionID].width;
	SelectedResolution->height = CameraResolutions[ResolutionID].height;

	return TRUE;
}
	
//function for finding number of devices connected,friendly name
int CameraEnumeration::GetListofDeviceseCon(void)
{
	if(DEBUG_ENABLED)
		cout << "Get List of Devices eCon";
		
	struct udev_enumerate *enumerate;
    struct udev_list_entry *devices, *dev_list_entry;
    struct udev_device *dev;

    int num_dev = 0;
    int fd = 0;
    struct v4l2_capability v4l2_cap;
	struct udev *udev = udev_new();

    if (!udev)
    {
        /*use fall through method (sysfs)*/
        g_print("Can't create udev...using sysfs method\n");
    }

	DeviceInstances = NULL;
	DeviceInstances = g_new0( LDevices, 1);
    DeviceInstances->listVidDevices = NULL;

    /* Create a list of the devices in the 'v4l2' subsystem. */
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "video4linux");
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);
    /* For each item enumerated, print out its information.
        udev_list_entry_foreach is a macro which expands to
        a loop. The loop will be executed for each member in
        devices, setting dev_list_entry to a list entry
        which contains the device's path in /sys. */
    udev_list_entry_foreach(dev_list_entry, devices)
    {
        const char *path;

        /* Get the filename of the /sys entry for the device
            and create a udev_device object (dev) representing it */
        path = udev_list_entry_get_name(dev_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        /* usb_device_get_devnode() returns the path to the device node
            itself in /dev. */
        const gchar *v4l2_device = udev_device_get_devnode(dev);

        /* open the device and query the capabilities */
        if ((fd = v4l2_open(v4l2_device, O_RDWR | O_NONBLOCK, 0)) < 0)
        {
            g_printerr("ERROR opening V4L2 interface for %s\n", v4l2_device);
            v4l2_close(fd);
            continue; /*next dir entry*/
        }

        if (xioctl(fd, VIDIOC_QUERYCAP, &v4l2_cap) < 0)
        {
            perror("VIDIOC_QUERYCAP error");
            g_printerr("   couldn't query device %s\n", v4l2_device);
            v4l2_close(fd);
            continue; /*next dir entry*/
        }
        v4l2_close(fd);

        num_dev++;
        /* Update the device list*/
        DeviceInstances->listVidDevices = g_renew(VidDevice,
            DeviceInstances->listVidDevices,
            num_dev);
        DeviceInstances->listVidDevices[num_dev-1].device = g_strdup(v4l2_device);
	DeviceInstances->listVidDevices[num_dev-1].deviceID = atoi(DeviceInstances->listVidDevices[num_dev-1].device+10);
        DeviceInstances->listVidDevices[num_dev-1].friendlyname = g_strdup((gchar *) v4l2_cap.card);
        DeviceInstances->listVidDevices[num_dev-1].bus_info = g_strdup((gchar *) v4l2_cap.bus_info);
        

        /* The device pointed to by dev contains information about
            the v4l2 device. In order to get information about the
            USB device, get the parent device with the
            subsystem/devtype pair of "usb"/"usb_device". This will
            be several levels up the tree, but the function will find
            it.*/
        dev = udev_device_get_parent_with_subsystem_devtype(
                dev,
                "usb",
                "usb_device");
        if (!dev)
        {
            cout << "Unable to find parent usb device.";
		    DeviceInstances->listVidDevices[num_dev-1].vendor = NULL;
		    DeviceInstances->listVidDevices[num_dev-1].product = NULL;
            continue;
        }

        /* From here, we can call get_sysattr_value() for each file
            in the device's /sys entry. The strings passed into these
            functions (idProduct, idVendor, etc.) correspond
            directly to the files in the directory which represents
            the USB device. Note that USB strings are Unicode, UCS2
            encoded, but the strings returned from
            udev_device_get_sysattr_value() are UTF-8 encoded. */
       
        DeviceInstances->listVidDevices[num_dev-1].vendor = g_strdup((gchar*)udev_device_get_sysattr_value(dev, "idVendor"));
        DeviceInstances->listVidDevices[num_dev-1].product =  g_strdup((gchar*)udev_device_get_sysattr_value(dev, "idProduct"));

        udev_device_unref(dev);
    }
    /* Free the enumerator object */
    udev_enumerate_unref(enumerate);

    DeviceInstances->num_devices = num_dev;
    return(num_dev);
}

//check whether the selected device is e-con's Stereo
BOOL CameraEnumeration::IsStereoDeviceAvail(char *pid)
{
	if(pid == NULL) 
		return FALSE;

	if(strcmp(pid, See3CAM_STEREO) == 0)
		return TRUE;
	else
		return FALSE;
}

//Query the resolution for selected camera.
void CameraEnumeration::query_resolution(int deviceid)
{
	int fd = 0;
	
	/* open the device and query the capabilities */
	if ((fd = v4l2_open(DeviceInstances->listVidDevices[deviceid].device, O_RDWR | O_NONBLOCK, 0)) < 0)
    	{
        	g_printerr("ERROR opening V4L2 interface for %s\n", DeviceInstances->listVidDevices[deviceid].device);
	        v4l2_close(fd);
       		return;
    	}

	//Query framesizes to get the supported resolutions for Y16 format.
	struct v4l2_frmsizeenum frmsize;
	frmsize.pixel_format = V4L2_PIX_FMT_Y16;
	frmsize.index = 0;
	    
	while (xioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0)
	{
 		if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
		{
			CameraResolutions.push_back(cv::Size(frmsize.discrete.width, frmsize.discrete.height));
       		}
	        else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
       		{
		        CameraResolutions.push_back(cv::Size(frmsize.stepwise.max_width, frmsize.stepwise.max_height));
      		}
	       frmsize.index++;
	}
	v4l2_close(fd);
}

//To free the device list created
void CameraEnumeration::freeDevices(void)
{
	for(int i=0;i<(DeviceInstances->num_devices);i++)
	{
		if(DeviceInstances->listVidDevices[i].device != NULL)
			g_free(DeviceInstances->listVidDevices[i].device);
		if(DeviceInstances->listVidDevices[i].friendlyname != NULL)
			g_free(DeviceInstances->listVidDevices[i].friendlyname);
		if(DeviceInstances->listVidDevices[i].bus_info != NULL)
			g_free(DeviceInstances->listVidDevices[i].bus_info);
		if(DeviceInstances->listVidDevices[i].vendor != NULL)
			g_free(DeviceInstances->listVidDevices[i].vendor);
		if(DeviceInstances->listVidDevices[i].product != NULL)
			g_free(DeviceInstances->listVidDevices[i].product);
	}

	if(DeviceInstances->listVidDevices != NULL)
		g_free(DeviceInstances->listVidDevices);
	if(DeviceInstances != NULL)
		g_free(DeviceInstances);
}

//Displays the Text on the image passed
int DisplayText(cv::Mat Image, cv::String Text, cv::Point Location)
{
	//Display the text
	putText(Image, Text, Location, 2, 1.0, cv::Scalar(255, 0, 150)  , 2, 8, false);
	return TRUE;
}

/* ioctl with a number of retries in the case of failure*/
int xioctl(int fd, int IOCTL_X, void *arg)
{
	int ret = 0;
	int tries= IOCTL_RETRY;
	do
	{
		ret = ioctl(fd, IOCTL_X, arg);
	}
	while (ret && tries--);

	return (ret);
}

}

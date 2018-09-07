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
 VolumeEstimation:  Estimates the volume of the box using edge detection
					the height of the base is estimated initially.
					Contour of the box is found and distance is calculated
					to find the length and breadth of the box.
**********************************************************************/

#include "VolumeEstimation.h"

using namespace cv;
using namespace std;
using namespace Tara;

//Constructor
VolumeEstimation::VolumeEstimation(void)
{
	BaseDepth = -1;
}

//Initialises all the necessary files
int VolumeEstimation::Init()
{
	cout << endl  << "		Volume Estimation Application " << endl  << endl;
	cout << " Displays the dimensions of the box placed!" << endl << " The Base height is calculated in the start of the application!" << endl;
	cout << " Displays the Left Frame and the disparity map!" << endl << endl;

	//Camera Init
	if(!_Disparity.InitCamera(true, true))
	{
		if(DEBUG_ENABLED)
			cout << "Camera Initialisation Failed\n";
		return FALSE;
	}

	//Streams the camera and process the height
	BaseDepthEstimation();

	return TRUE;
}

//Streams the input from the camera
int VolumeEstimation::CameraStreaming()
{
	char WaitKeyStatus;
	bool RImageDisplay = false;
	int BrightnessVal = 4;		//Default value
	int ManualExposure = 0;

	//Window Creation
	namedWindow("Left Image", WINDOW_AUTOSIZE);

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Estimates the volume of the box in the scene
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}

		//Process the image
		ProcessImages(LeftImage, RightImage);

		//Display the Iamges
		imshow("Left Image",  LeftImage);

		if(RImageDisplay) //Dispaly if enabled
			imshow("Right Image", RightImage);

		//waits for the Key input
		WaitKeyStatus = waitKey(5);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{
			destroyAllWindows();
			break;
		}
		else if(WaitKeyStatus == 'r' || WaitKeyStatus == 'R') //Show the right image
		{
			if(!RImageDisplay)
			{
				RImageDisplay = true;
				namedWindow("Right Image", WINDOW_AUTOSIZE);
			}
			else
			{
				RImageDisplay = false;
				destroyWindow("Right Image");
			}
		}
		else if(WaitKeyStatus == 'b' || WaitKeyStatus == 'B') //Brightness
		{
			cout << endl << "Enter the Brightness Value, Range(1 to 7): " << endl;

			BrightnessVal = 0;
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats and Alphanumeric strings
			{
				std::stringstream ss(Inputline);
				if (ss >> BrightnessVal)
				{
					if (ss.eof())
					{
						//Setting up the brightness of the camera
						if (BrightnessVal >= 1  && BrightnessVal <= 7)
						{
							//Setting up the brightness
			                //In opencv-linux 3.1.0, the value needs to be normalized by max value (7)
			                _Disparity.SetBrightness((double)BrightnessVal / 7.0);
						}
						else
						{
							 cout << endl << " Value out of Range - Invalid!!" << endl;
						}
						break;
					}
				}
				BrightnessVal = -1;
				break;
			}

			if(BrightnessVal == -1)
			{
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}
		}
		//Sets up Auto Exposure
		else if(WaitKeyStatus == 'a' || WaitKeyStatus == 'A' ) //Auto Exposure
		{
			_Disparity.SetAutoExposure();
		}
		else if(WaitKeyStatus == 'e' || WaitKeyStatus == 'E') //Set Exposure
		{
			cout << endl << "Enter the Exposure Value Range(10 to 1000000 micro seconds): " << endl;

			ManualExposure = 0;
			cin >> ws; //Ignoring whitespaces

			while(getline(std::cin, Inputline)) //To avoid floats and Alphanumeric strings
			{
				std::stringstream ss(Inputline);
				if (ss >> ManualExposure)
				{
					if (ss.eof())
					{
						if(ManualExposure >= SEE3CAM_STEREO_EXPOSURE_MIN && ManualExposure <= SEE3CAM_STEREO_EXPOSURE_MAX)
						{
							//Setting up the exposure
							_Disparity.SetExposure(ManualExposure);
						}
						else
						{
							cout << endl << " Value out of Range - Invalid!!" << endl;
						}
						break;
					}
				}
				ManualExposure = -1;
				break;
			}

			if(ManualExposure == -1)
			{
				cout << endl << " Value out of Range - Invalid!!" << endl;
			}
		}
	}

	return TRUE;
}

//Processes the input images and displays the results
int VolumeEstimation::ProcessImages(Mat LImage, Mat RImage)
{
	vector<Point> LeftPoints, RightPoints;

	//Detects the Edge and returns the valid corners
	LeftPoints = EdgeDetection(LImage);

	//Detects the Edge and returns the valid corners
	RightPoints = EdgeDetection(RImage);

	LeftImage = LImage.clone();
	RightImage = RImage.clone();

	//Displays the calculated values
	DisplayVolume(LeftPoints, RightPoints);

	vector<Point>().swap(LeftPoints);
	vector<Point>().swap(RightPoints);
	return TRUE;
}

//Estimates the depth of the base
int VolumeEstimation::BaseDepthEstimation()
{
	char WaitKeyStatus;
	int NoFrames = 0;
	float DepthValue = 0;

	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press r/R on the Image Window to see the right image" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	//Estimates the depth of the base from 20 frames and averages it
	while(NoFrames < 20)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}

		//Get Disparity map
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);

		//Estimating the depth
		_Disparity.EstimateDepth((Point(300, 230)), &DepthValue);

		//Taking Average
		BaseDepth  += (DepthValue / 10);
		DepthValue = 0;

		//Text display
		DisplayText(gDisparityMap_viz, "Base Depth Estimation", Point(30, 30));

		//Display the Disparity Map
		imshow("Disparity Image", gDisparityMap_viz);

		//waits for the Key input
		WaitKeyStatus = waitKey(100);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 27 || WaitKeyStatus == 'Q')
		{
			destroyAllWindows();
			break;
		}

		NoFrames++;
	}

	//Closes all the windows
	destroyAllWindows();

	//Base depth value calculation	// dividing by i for the average
	BaseDepth = BaseDepth / NoFrames;

	if(DEBUG_ENABLED)
		cout << "Volume Estimation: Base Depth Found: " << BaseDepth << endl;

	//Invoke the function to find the box
	CameraStreaming();

	return TRUE;

}

//Displays the volume
int VolumeEstimation::DisplayVolume(vector<Point> LeftPoints, vector<Point> RightPoints)
{
	vector<Point3f> LeftAxis, RightAxis;
	vector<Point3f> Left3DPoints, Right3DPoints;
	float DepthValue  = 0;

	//2D to 3D conversion
	if(LeftPoints.size() == 4 && RightPoints.size() == 4)
	{
		//Conversion
		for(size_t i = 0; i < RightPoints.size(); i++)
		{
			if(LeftPoints[i].x < 3)
			{
				return 0; //Avoiding the corners to reduce the false detections
			}

			Point3d DetectedPt;
			//Left Point
			DetectedPt.x = LeftPoints[i].x;
			DetectedPt.y = LeftPoints[i].y;
			DetectedPt.z = LeftPoints[i].x - RightPoints[i].x;

			LeftAxis.push_back(DetectedPt);

			//Left Point
			DetectedPt.x = RightPoints[i].x;
			DetectedPt.y = RightPoints[i].y;
			DetectedPt.z = RightPoints[i].x - LeftPoints[i].x;

			RightAxis.push_back(DetectedPt);
		}

		//Projection to 3D Matrix
		perspectiveTransform(LeftAxis,  Left3DPoints,  _Disparity.DepthMap);
		perspectiveTransform(RightAxis, Right3DPoints, _Disparity.DepthMap);

		//Estimate the distance for the Left image
		float DistanceX = (float)norm(Left3DPoints[0] - Left3DPoints[1]);
		float DistanceY = (float)norm(Left3DPoints[0] - Left3DPoints[2]);

		//Estimate the distance for the Right image
		float RDistanceX = (float)norm(Right3DPoints[0] - Right3DPoints[1]);

		if(abs(DistanceX - RDistanceX) > 5 && DistanceX > 5 && DistanceY > 5) // To avoid detection of boxes in case of its absense
		{
			return 0;
		}

		DistanceX /= 10;
		DistanceY /= 10;

		//Plot the corners
		for (size_t i = 0; i < LeftPoints.size() - 1; i++)
		{
			circle(LeftImage, LeftPoints[i], 5, Scalar::all(0), 3, 8);
		}

		//Plot the lines
		line(LeftImage, LeftPoints[0], LeftPoints[1], Scalar::all(0), 3, 8);
		line(LeftImage, LeftPoints[0], LeftPoints[2], Scalar::all(0), 3, 8);

		//Depth calculation Subtracting the base depth from the corner depth
		DepthValue = (abs(Left3DPoints[2].z/(float)10.0 - BaseDepth) + abs(Left3DPoints[1].z/(float)10.0 - BaseDepth) + abs(Left3DPoints[0].z/(float)10.0 - BaseDepth)) / 3;

		//Restricting to single precision
		int iDisx = int (DistanceX  * 10);
		int iDisY = int (DistanceY  * 10);
		int iDisz = int (DepthValue * 10);

		DistanceX  = (float) iDisx / (float) 10.0;
		DistanceY  = (float) iDisY / (float) 10.0;
		DepthValue = (float) iDisz / (float) 10.0;

		if(DistanceX < float(2.0) || DistanceY < float(2.0)) //False detection
		{
			return 0;
		}

		#pragma region Display the Distance

			DisplayText(LeftImage, "Measurement in cm",  Point(10, 25));

			stringstream ss;
			ss.precision(1); //Restricting the precision after decimal

			ss << "X:" << fixed << DistanceX;
			DisplayText(LeftImage, ss.str(), Point(10, 60));
			ss.str(string());

			ss << "Y:" << fixed << DistanceY;
			DisplayText(LeftImage, ss.str(), Point(10, 100));
			ss.str(string());

			ss << "Z:" << fixed << DepthValue;
			DisplayText(LeftImage, ss.str(), Point(10, 140));
			ss.str(string());

		#pragma endregion Display the Distance
	}

	#pragma endregion Display Images

	//Display the updated Image with the Volume text
	imshow("Left Image", LeftImage);

	//Free all the vectors
	vector<Point3f>().swap(LeftAxis);
	vector<Point3f>().swap(RightAxis);
	vector<Point3f>().swap(Left3DPoints);
	vector<Point3f>().swap(Right3DPoints);

	return TRUE;
}

//Detects the edge points in the scene
vector<Point> VolumeEstimation::EdgeDetection(Mat InputImage)
{
	double CannyAccThresh;
	Mat BlurImage, CannyImage, ThresholdingImage;
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	vector<Point> Corners;

	//Gaussian blur
	GaussianBlur(InputImage, BlurImage, Size(5, 5), 0, 0, 4);

	//Automatic Generation of Threshold to Canny Edge Detection
	CannyAccThresh = threshold(BlurImage, ThresholdingImage, 0, 255, THRESH_OTSU);

	//Canny edge detection
	Canny(BlurImage, CannyImage, 0.1 * CannyAccThresh, CannyAccThresh, 3);

	//find the contours
	findContours(CannyImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

	//Push the contours
	for (size_t i = 0; i < contours.size(); i++)
	{
		if(contourArea(contours[i]) > 5)
		{
			for (size_t j = 0; j < contours[i].size(); j++)
			{
				Corners.push_back(contours[i][j]);
			}
		}
	}

	vector<Vec4i>().swap(hierarchy);
	vector<vector<Point> >().swap(contours);

	//Check the points and return those
	if(Corners.size() > 0)
		return Sorting(Corners);
	else
		return vector<Point>();

}

//Sort the points passed and returns the valid corners
vector<Point> VolumeEstimation::Sorting(vector<Point> corners)
{
	vector<Point> CornersDetected;
	int LowestIndexX = 0, LowestIndexY = 0, HighestIndexY = -1;
	int maxPointY = 0;
	int minPointX = (int)corners[0].x;
	int minPointY = (int)corners[0].y;

	//Find the lowest y
	for(size_t i = 0; i < corners.size(); i++ )
	{
		if(corners[i].y <= minPointY) //Lowest Y
		{
			LowestIndexY = i;
			minPointY = (int)corners[i].y;
		}

		if(corners[i].x <= minPointX )  //Lowest X
		{
			LowestIndexX = i;
			minPointX = (int)corners[i].x;
		}

		if(corners[i].y > maxPointY) //Highest Y
		{
			HighestIndexY = i;
			maxPointY = (int)corners[i].y;
		}
	}

	//Push the lowest X point
	CornersDetected.push_back(corners[LowestIndexX]);

	//Check the points and push the values
	if(LowestIndexY != -1 && HighestIndexY != -1)
	{
		CornersDetected.push_back(corners[LowestIndexY]);
		CornersDetected.push_back(corners[HighestIndexY]);
		CornersDetected.push_back(corners[LowestIndexX]);
	}

	return CornersDetected;
}

//Main Function
int main()
{
	if(DEBUG_ENABLED)
	{
		cout << "Volume Calculation - Application\n";
		cout << "--------------------------------\n\n";
	}

	//Create a object and init the method
	VolumeEstimation _VolumeEstimation;
	_VolumeEstimation.Init();

	if(DEBUG_ENABLED)
		cout << "Exit : Volume Calculation - Application\n";
	return TRUE;
}

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
	FaceDepth: Estimates the depth of the person from the camera by 
			    detecting the face of the person
**********************************************************************/
#include "FaceDepth.h"
#define RIGHTMATCH 150 //disparity range starts from 150 so in case of the point being less than 150 it means that the face is not fully covered in the right.

using namespace cv;
using namespace std;
using namespace Tara;

//Constructor
FaceDepth::FaceDepth(void)
{
	ss.precision(2); //Restricting the precision after decimal
}

//Destructor
FaceDepth::~FaceDepth(){};

//Initialises all the variables and methods
int FaceDepth::Init()
{	
	cout << endl << "		Face Detection Application " << endl  << endl;
	cout << " Detects multiple faces in the frame!" << endl << " Uses OpenCV Haarcascade file in the Face folder!" << endl;
	cout << " Rectangle around the detected faces with the depth is displayed!" << endl << " Only the left window is displayed!" << endl << endl;

	//Loads the cascade file
	FaceCascade = CascadeClassifier("//usr//local//tara-sdk//bin//Face//haarcascade_frontalface_alt2.xml");

	//If the file is not loaded properly
	if(FaceCascade.empty())
	{		
		cout << "\n\nError : haarcascade_frontalface_alt2.xml File Missing in the path /usr/local/tara-sdk/bin/Face!\n";		
		return FALSE;
	}
	
	if(DEBUG_ENABLED)
		cout << "Loaded Haarcascade Classifier File!" << endl;

	if(!_Disparity.InitCamera(true, true)) //Initialise the camera
	{
		if(DEBUG_ENABLED)
			cout << "Camera Initialisation Failed!\n";
		return FALSE;
	}

	//Streams the camera and process the height
	CameraStreaming();

	return TRUE;
}

//Unscaling the point to the actual image size for the lower resolutions
Point FaceDepth::unscalePoint(Point Pt, Size CurrentSize, Size TargetSize)
{
	Point DesPt;

	DesPt.x = int( (Pt.x /(float) CurrentSize.width)  * TargetSize.width );
	DesPt.y = int( (Pt.y /(float) CurrentSize.height) * TargetSize.height);
 
	return DesPt;
}

//Streams the input from the camera
int FaceDepth::CameraStreaming()
{	
	stringstream ss;
	vector<Rect> LFaces;
	float DepthValue;
	bool RImageDisplay = false;
	int BrightnessVal = 4;		//Default value
	int ManualExposure = 0;
	Mat gDisparityMap, gDisparityMap_viz, RightImage;
	Point g_SelectedPoint;
	
	//user key input
	char WaitKeyStatus;

	//Window Creation
	namedWindow("Left Image", WINDOW_AUTOSIZE);
	
	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" <<endl;
	cout << endl << "Press r/R on the Image Window to see the right image" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Estimates the depth of the face using Haarcascade file of OpenCV
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}	
	
		//Get disparity
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);

		//Detect the faces
		LFaces = DetectFace(LeftImage);
	
		//Estimate the Depth of the point selected
		for (size_t i = 0; i < LFaces.size(); i++)
		{
			//Pointing to the center of the face
			g_SelectedPoint = Point(LFaces[i].x + LFaces[i].width / 2, LFaces[i].y + LFaces[i].height / 2);

			Point scaledPoint = unscalePoint(g_SelectedPoint, LeftImage.size(),  gDisparityMap.size());
			
			//Find the depth of the point passed
			_Disparity.EstimateDepth(scaledPoint, &DepthValue);
			
			if(g_SelectedPoint.x > RIGHTMATCH && DepthValue > 0) //Mark the point selected by the user
			{				
				ss << DepthValue / 10 << " cm\0" ;
				DisplayText(LeftImage, ss.str(), Point(LFaces[i].x, LFaces[i].y));
				ss.str(string());
			}
		}
		
		//Display the Images
		imshow("Left Image",  LeftImage);

		if(RImageDisplay)
        {
        	imshow("Right Image", RightImage);
        }
			

		//waits for the Key input
		WaitKeyStatus = waitKey(10);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{	
			vector<Rect>().swap(LFaces);
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
 
//Detects the faces in the image passed
vector<Rect> FaceDepth::DetectFace(Mat InputImage)
{		
	vector<Rect> FacesDetected;

	//Detects the faces in the scene
	FaceCascade.detectMultiScale(InputImage, FacesDetected, 1.3, 5);

	//Marks the faces on the image
	for(size_t i = 0; i < FacesDetected.size(); i++)
	{
		rectangle(InputImage, FacesDetected[i], Scalar(255, 0, 0), 2);  
	}

	//Return the detected faces
	return FacesDetected;
}

//Main Function
int main()
{
	if(DEBUG_ENABLED)
	{
		cout << "Face Depth Estimation - Application\n";
		cout << "-----------------------------------\n\n";
	}

	//Object creation
	FaceDepth _FaceDepth;
	_FaceDepth.Init();

	if(DEBUG_ENABLED)
		cout << "Exit: Face Depth Estimation - Application!\n";
	return TRUE;
}

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

/*************************************************************************
	HeightCalibration: Finds the depth of the base from the camera which
				is later used by the Height estimation for estimating the 
				human height.
*************************************************************************/
#include "HeightCalibration.h"

using namespace cv;
using namespace std;
using namespace Tara;

Point DepthPoint(-1, -1);

//Call back Function for the mouse click on the input
void onMouse(int event, int x, int y, int flags, void* userdata);

//Constructor
HeightCalibration::HeightCalibration(void)
{
	DepthPointSelection = true;
}

//Initialises all the necessary files
int HeightCalibration::Init()
{
	cout << endl  << "		Height Calibration Application " << endl  << endl;
	cout << " Calibrates the height of the base from the camera!" << endl << " Select the point of which the depth is to estimated!" << endl << endl;
	
	//Initialise the camera 
	if(!_Disparity.InitCamera(true, true))
	{
		if(DEBUG_ENABLED)
			cout << "Camera Initialisation Failed\n";
		return FALSE;
	}

	//Streams the camera and process the height
	CameraStreaming();

	return TRUE;
}

//Streams the input from the camera
int HeightCalibration::CameraStreaming()
{	
	//Images
	Mat LeftImage, RightImage;
	int BrightnessVal = 4;		//Default value
	bool RImageDisplay = false;
	float Depth = 0, BaseDepth = 0;
	int FrameCount = 0, ManualExposure = 0;
	char WaitKeyStatus;
	
	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press r/R on the Image Window to see the right image" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Estimates the depth of the base from 10 frames and averages it 
	while(FrameCount < 10)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}

		//DisparityMap
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);
		
		if(DepthPointSelection)
		{
			DepthPoint = SelectDepthPoint();
		}
		else
		{
			circle(gDisparityMap_viz, DepthPoint, 7, Scalar::all(0), -1, 8);
			DisplayText(gDisparityMap_viz, "Selected Point", DepthPoint);
		
			//Returns the deoth of the point specified
			_Disparity.EstimateDepth(DepthPoint, &Depth);
			
			BaseDepth += Depth;
			FrameCount++;

			namedWindow("Disparity Map", WINDOW_AUTOSIZE);
			imshow("Disparity Map", gDisparityMap_viz);			
		}
		
		//Display the images
		namedWindow("Left Frame", WINDOW_AUTOSIZE);
		imshow("Left Frame", LeftImage);

		if(RImageDisplay)
        {
        	imshow("Right Frame", RightImage);
        }

			
		//waits for the Key input
		WaitKeyStatus = waitKey(100);
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
				namedWindow("Right Frame", WINDOW_AUTOSIZE);
            }
            else
            {
                RImageDisplay = false;
                destroyWindow("Right Frame");
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
		}//Sets up Auto Exposure
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

	//Average value of 10 frames
	//mm to cm conversions
	BaseDepth = BaseDepth / FrameCount / 10;
	
	//Generates a file with the depth value
	DepthFileGeneration(BaseDepth);

	//Closes all the windows
	destroyAllWindows();

	return TRUE;
}

//writes the Depth Value to a file
int HeightCalibration::DepthFileGeneration(float Depth)
{
	FILE *fp = NULL;
	stringstream ss;
	ss << Depth;
	
	//Open file
	fp = fopen("//usr//local//tara-sdk//bin//Height//BaseHeight.txt", "wb");

	if(fp == NULL)
	{
		cout << "\nError : BaseHeight.txt file creation in the path /usr/local/tara-sdk/bin/Height failed!!\n";
		return FALSE;
	}
	fwrite(ss.str().c_str(), 1, sizeof(char) * ss.str().size(), fp);
	fclose(fp);
	
	return TRUE;
}

//User gets to select the point for which the depth has to be determined
Point HeightCalibration::SelectDepthPoint()
{
	//Put text
	DisplayText(gDisparityMap_viz, "Select Depth Point", Point(10, 50));

	namedWindow("Depth Point Selection", WINDOW_AUTOSIZE);
	imshow("Depth Point Selection", gDisparityMap_viz);

	//Call back
	setMouseCallback("Depth Point Selection", onMouse, NULL);

	if(DepthPoint.x != -1)
	{		
		DepthPointSelection = false;
		
	}
	
	if(!DepthPointSelection)
		destroyWindow("Depth Point Selection");

	return DepthPoint;
}

//Call back Function for the mouse click on the input
void onMouse(int event, int x, int y, int flags, void* userdata)
{
	if(event == EVENT_LBUTTONDOWN) //Selects a point
	{	
		DepthPoint.x = x;
		DepthPoint.y = y;
    }
}

//Main Application
int main()
{
	if(DEBUG_ENABLED)
	{
		cout << "Height Calibration - Application\n";
		cout << "--------------------------------\n\n";
	}

	//Object Creation
	HeightCalibration _HeightCalibration;
	_HeightCalibration.Init();

	if(DEBUG_ENABLED)
		cout << "Exit : Height Calibration - Application\n";
	return TRUE;
}

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
 DisparityViewer: Displays the disparity image.
**********************************************************************/

#include "DisparityViewer.h"

using namespace cv;
using namespace std;
using namespace Tara;

//Initialises all the necessary files
int DisparityViewer::Init()
{
	cout << endl  << "		Disparity Viewer Application " << endl  << endl;
	cout << " Disparity Viewer - Displays the Disparity between the two frames" << endl << " Closer objects appear in Red and Farther objects appear in Blue Color!"<< endl;
	cout << " Displays the actual disparity without any filter" << endl << endl;
	//Initialise the camera
	if(!_Disparity.InitCamera(true, false))
	{
		if(DEBUG_ENABLED)
			cout << "Camera Initialisation Failed\n";
		return FALSE;
	}

	//Camera Streaming
	CameraStreaming();

	return TRUE;
}

//Streams the input from the camera
int DisparityViewer::CameraStreaming()
{
	Mat gDisparityMap, gDisparityMap_viz;
	Mat LeftImage, RightImage;
	char WaitKeyStatus;
	bool GrayScaleDisplay = false;
	int  BrightnessVal = 4;		//Default value
	int ManualExposure = 0;

	//Window creation
	namedWindow("Disparity Image", WINDOW_AUTOSIZE);
	namedWindow("Left Image", WINDOW_AUTOSIZE);
	namedWindow("Right Image", WINDOW_AUTOSIZE);

	cout << endl << "Press q/Q/Esc on the Image Window to quit the application!" << endl;
	cout << endl << "Press b/B on the Image Window to change the brightness of the camera" << endl;
	cout << endl << "Press d/D on the Image Window to see the grayscale disparity map" << endl;
	cout << endl << "Press a/A on the Image Window to change to Auto exposure  of the camera" << endl;
	cout << endl << "Press e/E on the Image Window to change the exposure of the camera" << endl << endl;

	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	string Inputline;

	//Displays the actual disparity of the Camera without any filter applied to the image
	while(1)
	{
		if(!_Disparity.GrabFrame(&LeftImage, &RightImage)) //Reads the frame and returns the rectified image
		{
			destroyAllWindows();
			break;
		}

		//Get disparity
		_Disparity.GetDisparity(LeftImage, RightImage, &gDisparityMap, &gDisparityMap_viz);

		//Display the Images
		imshow("Disparity Image", gDisparityMap_viz);
		imshow("Left Image", LeftImage);
		imshow("Right Image", RightImage);

		if(GrayScaleDisplay) //if Enabled
		{
			imshow("Disparity Map GrayScale", gDisparityMap);
		}

		//waits for the Key input
		WaitKeyStatus = waitKey(1);
		if(WaitKeyStatus == 'q' || WaitKeyStatus == 'Q' || WaitKeyStatus == 27) //Quit
		{
			destroyAllWindows();
			break;
		}
		else if(WaitKeyStatus == 'd' || WaitKeyStatus == 'D')
		{
			if(!GrayScaleDisplay)
			{
				GrayScaleDisplay = true;
				namedWindow("Disparity Map GrayScale", WINDOW_AUTOSIZE);
			}
			else
			{
				GrayScaleDisplay = false;
				destroyWindow("Disparity Map GrayScale");
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

//main application
int main()
{
	if(DEBUG_ENABLED)
	{
		cout << "Disparity Viewer\n";
		cout << "----------------\n\n";
	}

	//Disparity Viewer
	DisparityViewer _DisparityViewer;
	_DisparityViewer.Init();

	if(DEBUG_ENABLED)
		cout << "Exit : Disparity Viewer\n";

	return TRUE;
}

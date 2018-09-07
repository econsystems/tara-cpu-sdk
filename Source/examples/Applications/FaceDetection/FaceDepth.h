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

#pragma once
#include "Tara.h"

class FaceDepth
{
public:
	
	//Constructor
	FaceDepth(void);
	 
	//Destructor
	~FaceDepth();

	//Initalises all the methods
	int Init();
			
private:

	//classifier
	cv::CascadeClassifier FaceCascade;
	
	//Image
	cv::Mat LeftImage;

	//Detects the faces in the scene
	std::vector<cv::Rect> DetectFace(cv::Mat img);

	//Unscaling the point to the actual image size for the lower resolutions
	cv::Point unscalePoint(cv::Point Pt, cv::Size CurrentSize, cv::Size TargetSize);

	//Streams the input from the camera 
	int CameraStreaming(); 

	std::stringstream ss;  //To convert int to text
		
	//Object creation
	Tara::Disparity _Disparity;
};

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

class VolumeEstimation
{
public:
	//Constructor
	VolumeEstimation();

	//Initialises the methods
	int Init();

private:

	//Images
	cv::Mat LeftImage, RightImage;
	cv::Mat gDisparityMap, gDisparityMap_viz;

	//Depth of the base
	float BaseDepth;

	//Streams the input from the camera
	int CameraStreaming();

	//Detects the edge points in the scene
	std::vector<cv::Point> EdgeDetection(cv::Mat Image);

	//Processes the input images and displays the results
	int ProcessImages(cv::Mat LImage, cv::Mat RImage);

	//Sort the points passed and returns the valid corners
	std::vector<cv::Point> Sorting(std::vector<cv::Point> corners);

	//Projects the 3D Points and calculates the volume
	int DisplayVolume(std::vector<cv::Point> LP, std::vector<cv::Point> RP);

	//Estimates the depth of the base
	int BaseDepthEstimation();

	Tara::Disparity _Disparity;
};

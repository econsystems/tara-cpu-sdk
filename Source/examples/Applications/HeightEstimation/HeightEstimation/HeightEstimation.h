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

class HeightEstimation
{
public:

	//Constructor
	HeightEstimation(void);

	//Initalises all the methods
	int Init();

private:

	//int to text conversion
	std::stringstream ss;

	//Images
	cv::Mat LeftImage, RightImage;
	cv::Mat gDisparityMap_viz, gDisparityMap;

	//Depth of the base
	float BaseDepth, HeadDepth;

	//Streams the input from the camera
	int CameraStreaming();

	//Detects the minimum depth at the top of the image
	double EstimateHeight();

	//Reads the depth of the base from the camera
	int BaseHeightReading();

	//Object creation
	Tara::Disparity _Disparity;
};

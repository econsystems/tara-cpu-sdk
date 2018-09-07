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

#include "boost/thread/thread.hpp"
#include "pcl/common/common_headers.h"
#include "pcl/visualization/pcl_visualizer.h"
#include "pcl/visualization/cloud_viewer.h"
#include "pcl/common/transforms.h"

class PointCloudApp
{
public:

	//Constructor
	PointCloudApp(void);

	//Initialise
	int Init();

private:

	//Mat to hold disparity
	cv::Mat gDisparityMap, gDisparityMap_viz;
	cv::Mat LeftFrame, RightFrame;

	//PCL Viz
	float Trans_x, Trans_y, Trans_z;
	float Rot_x, Rot_y, Rot_z;
	double m_MinDepth;

	//Display the Point Cloud
	int ShowPointCloud();

	//Camera Streaming
	int CameraStreaming();

	//Object to access Disparity
	Tara::Disparity _Disparity;
};

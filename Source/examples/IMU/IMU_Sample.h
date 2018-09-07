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
	IMU_Sample.h : 	Declares the class to be used in the application.
**********************************************************************/
#pragma once
#include "Tara.h"
#include <math.h>
#include <pthread.h>
#include <signal.h>

#define		M_PI				3.14159265358979323846
#define		HALF_PI				(M_PI / 2)
#define		DEG2RAD				(M_PI / 180.f)
#define		RAD2DEG				(180.f / M_PI)

class IMU_Sample
{
public:
	//Constructor
	IMU_Sample(void);

	//Initialises the variables
	int Init();

	// Function declarations
	void getInclination(double w_x, double w_y, double w_z, double a_x, double a_y, double a_z);

private:

	double angleX, angleY, angleZ; // Rotational angle for cube [NEW]
	double RwEst[3];
	double glIMU_Interval;

	TaraRev g_eRev;
	double squared(double x);

	/* Drawing Points in circles for illustration*/
	cv::Point PointOnCircle(double radius, double angleInDegrees, cv::Point origin);

	/* Drawing angles in circles for illustration */
	void updateCircles();

	/*  Returns the interval time for sampling the values of the IMU. */
	double GetIMUIntervalTime(IMUCONFIG_TypeDef	lIMUConfig);
}IMU_SampleObj;

IMUDATAINPUT_TypeDef			glIMUInput;
BOOL						    glIMUAbortThread;
pthread_mutex_t				    IMUDataReadyEvent;

//Pthread call routine
void*	GetIMUValueThread(void *lpParameter);

void*	UpdateIMUValueThread(void *lpParameter);

//Keyboard hit detection
int 	kbhit(void);

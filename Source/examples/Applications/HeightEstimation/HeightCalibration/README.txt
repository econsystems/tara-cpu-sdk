========================================================================
    CONSOLE APPLICATION : HeightCalibration Project Overview
========================================================================
Height Calibration:

	This is used for calibrating the height of the base from the camera. 
	The base is determined by averaging(minimum of 10 frames) the depth of a point selected by a user on the disparity map.
	The depth calculated is written to the file named "BaseHeight" and placed under the folder named "Height" which is used by the height estimation to determine the height.
	This file is used by the height estimation to determine the height.

Note: 
	Make sure the folder "Height" is created in prior to launching the application.


Command to create HeightCalibration binary:
===========================================
To Build:
		$ make
		
To clean:
		$ make clean	
===========================================
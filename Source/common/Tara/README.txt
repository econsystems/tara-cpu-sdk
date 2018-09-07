========================================================================
    SHARED OBJECT LIBRARY : Tara Project Overview
========================================================================
/********** Options of the Project**********/

DISPARITY_OPTION :
==================
	when set to  1 - Best Quality Depth Map and Lower Frame Rate
	when set to  0 - Low  Quality Depth Map and High  Frame Rate

	
Tara namespace :
=================
Tara namespace Tara has 3 Classes

1. TaraCamParameters:
	Its used to load camera parameters i.e the calibrated files from the camera flash and compute the Q matrix.

2. Disparity:
	This class contains methods to estimate the disparity, get depth of the point selected, remap/ rectify the images. 

3. CameraEnumeration:
	This class enumerates the camera device connected to the PC and list outs the resolution supported. Initialises the camera with the resolution selected. 
	Initialises the Extension unit.

	
Command to create libecon_tara.so:
==================================
To Build:
		$ make
		
To clean:
		$ make clean
====================================

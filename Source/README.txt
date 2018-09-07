common
======
1. commonly used functions are put into a single class for modularity, to reuse them in the examples. 
2. Extension unit commands are build as separate library for modularity.

examples
========
	Contains different sets of applications using the stereo camera Tara.
	
To Build and install the samples on your system:
================================================
To Build and Install:
	$ ./configure.sh
	$ make
	$ sudo make install

To Uninstall:
	$ sudo make uninstall

To clean:
	$ make clean
	
To know what these commands do, please refer to the Building_SDK_Solutions.pdf in the Documents folder

Note :
	Before trying to build the applications, Make sure the configure shell script in the Source directory has run atleast once in your system. To run the applications, the libs placed in the common folder should be built prior in order to link with the applications.

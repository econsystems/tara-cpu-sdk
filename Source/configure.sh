#!/bin/bash
# Configuration script to make sure the dependency files to be used are installed

#Formatting options
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'
RET=0

#Commands and variables
MKDIR_P="/bin/mkdir -p"
PWD=`pwd`
OPENCV_INSTALL_PREFIX="/usr/local/tara-opencv"

#Creating necessary directories
if [ -d ./build ];then
	sudo rm -rf ./build
fi
$MKDIR_P build
cd build


#Install the following dependencies for compiling OpenCV and to use Point cloud Library.
echo -e "${GREEN}${BOLD}Dependencies to be installed for OpenCV and to use Point cloud Library\n${NC}"
echo -e "${BLUE}${BOLD}Some 8 sets of dependencies is going to be downloaded and installed"
echo -e "It will take some time. Please Wait..\n${NC}"

if [[ `lsb_release -rs` == "14.04" ]]
then
	sudo add-apt-repository "deb http://archive.ubuntu.com/ubuntu $(lsb_release -sc) universe"
	sudo add-apt-repository -y ppa:v-launchpad-jochen-sprickerhof-de/pcl
	sudo apt-get -y update
fi

if [ $? != 0 ]; then
	echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
	echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
	RET=1;
else

	echo -e "${RED}(1 of 8) ${BLUE} Some general development libraries\n${NC}"
	sudo apt-get -y install build-essential make cmake cmake-qt-gui g++ wget
	
	if [ $? != 0 ]; then
		echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
		echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
		RET=1;
	else

		echo -e "\n${RED}(2 of 8) ${BLUE} libav video input/output and glib development libraries\n${NC}"
		sudo apt-get -y install libavformat-dev libavutil-dev libswscale-dev libglib2.0-dev libtbb-dev
		
		if [ $? != 0 ]; then
			echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
			echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
			RET=1;
		else

			echo -e "\n${RED}(3 of 8) ${BLUE} Video4Linux camera development libraries\n${NC}"
			sudo apt-get -y install libv4l-dev
		
			if [ $? != 0 ]; then
				echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
				echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
				RET=1;
			else

				echo -e "\n${RED}(4 of 8) ${BLUE} Eigen3 math development libraries\n${NC}"
				sudo apt-get -y install libeigen3-dev
			
				if [ $? != 0 ]; then
					echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
					echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
					RET=1;
				else

					echo -e "\n${RED}(5 of 8) ${BLUE} OpenGL development libraries (to allow creating graphical windows)\n${NC}"
					sudo apt-get -y install libglew-dev
		
					if [ $? != 0 ]; then
						echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
						echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
						RET=1;
					else

						echo -e "\n${RED}(6 of 8) ${BLUE} GTK development libraries (to allow creating graphical windows)\n${NC}"
						sudo apt-get -y install libgtk2.0-dev
	
						if [ $? != 0 ]; then
							echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
							echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
							RET=1;
						else

							echo -e "\n${RED}(7 of 8) ${BLUE} Udev development libraries (to allow access to device information)\n${NC}"
							sudo apt-get -y install libudev-dev
		
							if [ $? != 0 ]; then
								echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
								echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
								RET=1;
							else

								echo -e "\n${RED}(8 of 8) ${BLUE} Point Cloud Library\n${NC}"
								if [[ `lsb_release -rs` == "16.04" ]]
								then
									sudo apt-get -y install libpcl-dev
								elif [[ `lsb_release -rs` == "14.04" ]]
								then
									sudo apt-get -y install libpcl-all
								fi
		
								if [ $? != 0 ]; then
									echo -e "${RED}${BOLD}Make sure system time is updated properly${NC}"
									echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
									RET=1;
								else

									echo -e "\n${GREEN}${BOLD}All Necessary Dependencies installed\n${NC}"

									#Building and Installing OpenCV
									echo -e "\n${GREEN}${BOLD}Building OpenCV\n${NC}"
	
									echo -e "${BLUE}${BOLD}Downloading OpenCV version 3.4.1\n${NC}"
									wget -O opencv-3.4.1.zip https://github.com/opencv/opencv/archive/3.4.1.zip
									if [ $? != 0 ]; then
										echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
										echo -e "${RED}${BOLD}Downloading OpenCV 3.4.1 failed${NC}"
										RET=1;
									else
	
										echo -e "\n${BLUE}${BOLD}Downloading OpenCV Extra Modules\n${NC}"
										wget -O opencv_contrib-3.4.1.zip https://github.com/opencv/opencv_contrib/archive/3.4.1.zip
										if [ $? != 0 ]; then
											echo -e "${RED}${BOLD}Make sure internet is connected${NC}"
											echo -e "${RED}${BOLD}Downloading OpenCV contrib modules failed${NC}"
											RET=1;
										else
											echo -e "\n${BLUE}${BOLD} Unzipping the OpenCV source package \n${NC}"
											unzip -q opencv_contrib-3.4.1.zip 
											unzip -q opencv-3.4.1.zip
											if [ $? != 0 ]; then
												echo -e "${RED}${BOLD}Unzipping the OpenCV failed${NC}"
												RET=1;
											else
												echo -e "\n${BLUE}${BOLD}Build Started, It will take some time. Please Wait..\n${NC}"
												cd opencv-3.4.1
												$MKDIR_P build
												cd build
	
												cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_PREFIX -D WITH_TBB=OFF -D BUILD_TBB=OFF -D WITH_V4L=ON -D WITH_LIBV4L=OFF -D BUILD_TESTS=OFF -D BUILD_PERF_TESTS=OFF -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.4.1/modules -D BUILD_opencv_aruco=OFF -D BUILD_opencv_bgsegm=OFF -D BUILD_opencv_bioinspired=OFF -D BUILD_opencv_ccalib=OFF -D BUILD_opencv_datasets=OFF -D BUILD_opencv_dnn=OFF -D BUILD_opencv_dpm=OFF -D BUILD_opencv_face=OFF -D BUILD_opencv_fuzzy=OFF -D BUILD_opencv_line_descriptor=OFF -D BUILD_opencv_optflow=OFF -D BUILD_opencv_plot=OFF -D BUILD_opencv_reg=OFF -D BUILD_opencv_rgbd=OFF -D BUILD_opencv_saliency=OFF -D BUILD_opencv_stereo=OFF -D BUILD_opencv_structured_light=OFF -D BUILD_opencv_surface_matching=OFF -D BUILD_opencv_text=OFF -D BUILD_opencv_tracking=OFF -D BUILD_opencv_xfeatures2d=OFF -D BUILD_opencv_xobjdetect=OFF -D BUILD_opencv_xphoto=OFF -D BUILD_opencv_ximgproc=ON ..
	
												sudo make -j4 install
												if [ $? != 0 ]; then
													echo -e "${RED}${BOLD}Building OpenCV failed${NC}"
													RET=1;
												else
	
													echo -e "\n${BLUE}${BOLD}Creating a conf file ${RED}(/etc/ld.so.conf.d/01-tara-opencv.conf) ${BLUE}${BOLD}to link the shared libraries required for Tara-SDK applications${NC}"
	
													if [ -f /etc/ld.so.conf.d/01-tara-opencv.conf ];then
														sudo rm /etc/ld.so.conf.d/01-tara-opencv.conf
													fi
													echo "#Tara opencv dependency Lib directory" | sudo tee -a /etc/ld.so.conf.d/01-tara-opencv.conf
													echo "$OPENCV_INSTALL_PREFIX/lib" | sudo tee -a /etc/ld.so.conf.d/01-tara-opencv.conf
													sudo ldconfig
	
													echo -e "\n${GREEN}${BOLD}OpenCV 3.4.1 is built and ready to be used\n${NC}"
												fi
											fi
										fi
									fi
								fi
							fi
						fi
					fi
				fi
			fi
		fi
	fi
fi

if [ $RET == 0 ]; then
	echo -e "\n${GREEN}${BOLD}Configured successfully\n${NC}"
else
	echo -e "\n${RED}${BOLD}Configuration failed\n${NC}"
fi

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

#Install the following dependencies for compiling OpenCV and to use Point cloud Library.
echo -e "${GREEN}${BOLD}Dependencies to be installed for OpenCV and to use Point cloud Library\n${NC}"
echo -e "${BLUE}${BOLD}Some 8 sets of dependencies is going to be downloaded and installed"
echo -e "Make sure internet connection is available, It will take some time. Please Wait..\n${NC}"

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
	echo -e "\n${GREEN}${BOLD}All Necessary Dependencies successfully installed\n${NC}"
else
	echo -e "\n${RED}${BOLD}Installation failed, Some dependencies are not installed properly\n${NC}"
fi

#!/bin/bash
# installation script for tara-sdk 

#Finding the OS, Arch and LTS version
. /etc/lsb-release
OS=$DISTRIB_ID
ARCH=$(uname -m | sed 's/x86_//;s/i[3-6]86/32/')
VER=$DISTRIB_RELEASE

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

#Setting the directories
TARA_INSTALL_PREFIX="/usr/local/tara-sdk"
TARA_BIN_DIR="${TARA_INSTALL_PREFIX}/bin"
TARA_LIB_DIR="${TARA_INSTALL_PREFIX}/lib"

#commands 
MKDIR_P="/bin/mkdir -p"
PWD=`pwd`

if [ "$EUID" -ne 0 ]; then
	echo -e "\n${RED}${BOLD}Please run as root${NC}"
	echo -e "\n${RED}${BOLD}cmd : sudo ./install.sh${NC}"
	RET=1;
else
	#Printing the found OS, Arch and LTS version
	echo -e "\n${GREEN}${BOLD}Installing Tara SDK${NC}\n"
	echo -e "${BLUE}${BOLD}OS : ${BLUE}"$OS
	echo -e "${BLUE}${BOLD}Architecure : ${BLUE}"$ARCH
	echo -e "${BLUE}${BOLD}LTS version : ${BLUE}"$VER
	echo ""


	#Checking if the OS is Ubuntu
	if [ "$OS" != "Ubuntu" ]; then
		echo -e "${RED}${BOLD}Not a ubuntu version${NC}"
		RET=1;
	else
		#Checking if there is a prebuilt directory for the LTS version
		if [ -d $PWD/$OS-$VER ]; then
			echo -e "${GREEN}Directory for the current LTS version is found${NC}"

			#if [ $ARCH -eq 32 ]; then
			#		cd $PWD/$OS-$VER/binary && chmod +x *
			if [ $ARCH -eq 64 ]; then
					cd $PWD/$OS-$VER/binary_x64 && chmod +x *
			else
				RET=1;
			fi
			
			if [ $RET != 1 ]; then

				if [ $? != 0 ]; then
					echo -e "${RED}No files inside the directory${NC}"
					RET=1;
				else

				
					currentdir=$PWD
					
					#1. Remove binary from the TARA_INSTALL_PREFIX
					echo -e "\n${RED}${BOLD}Removing the already installed directory if any${NC}"
					if test -d "$TARA_INSTALL_PREFIX"; then
						cd "$TARA_INSTALL_PREFIX" && rm -rf *;
					fi


					#2. Install the binary from the current path to the TARA_INSTALL_PREFIX
					echo -e "\n${BLUE}${BOLD}Installing binaries${NC}"
					echo -e "${BLUE}Creating a bin directory${NC}"		
					test -z "$TARA_BIN_DIR" || $MKDIR_P "$TARA_BIN_DIR";
					echo -e "${BLUE}Installing the binaries to the bin directory created${NC}"	
					cp -arpf  $currentdir/* $TARA_BIN_DIR;

					if [ $? != 0 ]; then
						echo -e "${RED}Copying Binaries failed${NC}"
						RET=1;
					else
				
						#Install the libraries to the TARA_INSTALL_PREFIX
						echo -e "\n${BLUE}${BOLD}Installing libraries${NC}"
						echo -e "${BLUE}Creating a lib directory${NC}"		
						test -z "$TARA_LIB_DIR" || $MKDIR_P "$TARA_LIB_DIR";
						echo -e "${BLUE}Installing the libraries to the lib directory created${NC}"

						#if [ $ARCH -eq 32 ]; then      
						#	cp -rpf  $currentdir/../lib/* $TARA_LIB_DIR;
						if [ $ARCH -eq 64 ]; then
							cp -arpf  $currentdir/../lib_x64/* $TARA_LIB_DIR;
						fi

						if [ $? != 0 ]; then
							echo -e "${RED}Copying libraries failed${NC}"
							RET=1;
						else


							#3. Creating a .conf file for library loading
							if [ -f /etc/ld.so.conf.d/00-tara.conf ];then
								rm  /etc/ld.so.conf.d/00-tara.conf
							fi
			
							echo -e "\n${BLUE}${BOLD}Creating a conf file ${RED}(/etc/ld.so.conf.d/00-tara.conf) ${BLUE}${BOLD}to link the shared libraries required for Tara-SDK applications${NC}"	
							echo "#Tara dependency Lib directory" >> /etc/ld.so.conf.d/00-tara.conf
							echo "$TARA_LIB_DIR" >> /etc/ld.so.conf.d/00-tara.conf
							ldconfig
							
							#4. Changing permission of /usr/local/tara-sdk path
							echo -e "\n${BLUE}${BOLD}Changing permission of ${RED}/usr/local/tara-sdk path${NC}"
							chmod 0777 ${TARA_INSTALL_PREFIX} -R												
							
							#5. Adding a .rules file
							if [ -f /etc/udev/rules.d/60-tara.rules ];then
								sudo rm /etc/udev/rules.d/60-tara.rules
							fi
							
							echo -e "\n${BLUE}${BOLD}Adding a .rules file ${RED}(/etc/udev/rules.d/60-tara.rules)${NC}"
							echo "#Changing the permission to access hidraw" >> /etc/udev/rules.d/60-tara.rules
							echo "KERNEL==\"hidraw*\", SUBSYSTEM==\"hidraw\",  ATTRS{idVendor}==\"2560\", ATTRS{idProduct}==\"c114\", MODE=\"0666\""	>> /etc/udev/rules.d/60-tara.rules
							sudo udevadm trigger						
				
							#6. Adding the path to use sudo	
							sed -i '/tara/d' /etc/profile
							echo -e "\n${BLUE}${BOLD}Adding the path variable in ${RED}/etc/profile${NC}"
							echo "export PATH=\$PATH:$TARA_BIN_DIR" >> /etc/profile

							#7. Adding the path to bashrc for autocomplete
							echo -e "\n${BLUE}${BOLD}Adding the path variable in ${RED}~/.bashrc ${BLUE}${BOLD}for auto complete${NC}"
							if [ -f $HOME'/'.bashrc ];then
								sed -i '/tara/d'  $HOME/.bashrc			
								echo "export PATH=\$PATH:$TARA_BIN_DIR" >> $HOME'/'.bashrc
							else
								echo -e "${RED}${BOLD}bashrc not found${NC}"
							fi
						fi
					fi
				fi
			else
				echo -e "${RED}${BOLD}Directory for the current architecture is not found${NC}"
				echo -e "${BLUE}${BOLD}Use the Makefiles in the source folder to build SDK for your Ubuntu version${NC}\n"
				RET=1;
			fi
			
		else
			echo -e "${RED}${BOLD}Directory for the current LTS version is not found${NC}"
			echo -e "${BLUE}${BOLD}Use the Makefiles in the source folder to build SDK for your Ubuntu version${NC}\n"
			RET=1;
		fi
	fi
fi

if [ $RET == 0 ]; then
	echo -e "\n${GREEN}${BOLD}Installation success${NC}"	
	echo -e "${BLUE}Tara-SDK installed in the directory ${RED}$TARA_INSTALL_PREFIX${NC}"
	echo -e "\n${BLUE}${BOLD}Source the bashrc to reflect the environment changes${NC}"
	echo -e "\n${RED}${BOLD}. ~/.bashrc${NC}\n"
else
	echo -e "\n${RED}${BOLD}Installation failed${NC}\n"
fi

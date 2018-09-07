#Makefile to generate the common libs and binaries
#While executing make, common libs and binaries will be generated

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m

#commands 
MKDIR_P=/bin/mkdir -p
PWD=`pwd`

#Setting the directories
TARA_INSTALL_PREFIX = /usr/local/tara-sdk
TARA_BIN_DIR   = ${TARA_INSTALL_PREFIX}/bin
TARA_LIB_DIR   = ${TARA_INSTALL_PREFIX}/lib
FACE_DET_DIR   = ./examples/Applications/FaceDetection
HEIGHT_CAL_DIR = ./examples/Applications/HeightEstimation/HeightCalibration
HEIGHT_EST_DIR = ./examples/Applications/HeightEstimation/HeightEstimation
VOLUME_EST_DIR = ./examples/Applications/VolumeEstimation
IMU_DIR	       = ./examples/IMU
PCL_DIR        = ./examples/PointCloud
TARA_CAM_DIR   = ./examples/Samples/TaraCamViewer
TARA_DEP_DIR   = ./examples/Samples/TaraDepthViewer
TARA_DIS_DIR   = ./examples/Samples/TaraDisparityViewer
LIB_TARA_DIR   = ./common/Tara
LIB_XUNIT_DIR  = ./common/xunit
HAAR_FACE_XML  = ./build/opencv-3.4.1/data/haarcascades_cuda/

#Building Targets
default:  libs binaries
	@echo "\n${GREEN}${BOLD}SDK build completed${NC}"	

libs:
	@echo "\n${BLUE}${BOLD}Building SDK solution${NC}"
	@make -C ./common

binaries:
	@make -C ./examples

clean:
	@echo "\n${BLUE}${BOLD}Cleaning the SDK solution${NC}"
	@make clean -C ./common
	@make clean -C ./examples
	@echo "\n${GREEN}${BOLD}SDK built files removed${NC}"

install: 	
	@echo "\n${GREEN}${BOLD}Installing Tara SDK${NC}"
	@#1. Removing the already installed directory TARA_INSTALL_PREFIX
	@echo "\n${RED}${BOLD}Removing the already installed directory if any${NC}"
	@if test -d $(TARA_INSTALL_PREFIX); then\
		cd $(TARA_INSTALL_PREFIX) && rm -rf *;\
	fi	

	@#2. Install the binaries to TARA_BIN_DIR
	@echo "\n${BLUE}${BOLD}Installing binaries${NC}"
	@echo "${BLUE}Creating a bin directory${NC}"		
	@test -z "$(TARA_BIN_DIR)" || $(MKDIR_P) "$(TARA_BIN_DIR)";
	@test -z "$(TARA_BIN_DIR)/Face" || $(MKDIR_P) "$(TARA_BIN_DIR)/Face";
	@test -z "$(TARA_BIN_DIR)/Height" || $(MKDIR_P) "$(TARA_BIN_DIR)/Height";
	@echo "\n${BLUE}Installing the binaries to the bin directory created${NC}"
	
	@echo "${RED} (1 of 9)${BLUE} Installing FaceDetection${NC}"
	@cp $(FACE_DET_DIR)/FaceDetection $(TARA_BIN_DIR);
	
	@echo "${RED} (2 of 9)${BLUE} Installing HeightCalibration${NC}"
	@cp $(HEIGHT_CAL_DIR)/HeightCalibration $(TARA_BIN_DIR);
	
	@echo "${RED} (3 of 9)${BLUE} Installing HeightEstimation${NC}"
	@cp $(HEIGHT_EST_DIR)/HeightEstimation $(TARA_BIN_DIR);
	
	@echo "${RED} (4 of 9)${BLUE} Installing VolumeEstimation${NC}"
	@cp $(VOLUME_EST_DIR)/VolumeEstimation $(TARA_BIN_DIR);	
	
	@echo "${RED} (5 of 9)${BLUE} Installing ImuApplication${NC}"
	@cp $(IMU_DIR)/ImuApplication $(TARA_BIN_DIR);	
	
	@echo "${RED} (6 of 9)${BLUE} Installing PointCloud${NC}"
	@cp $(PCL_DIR)/PointCloud $(TARA_BIN_DIR);	
	
	@echo "${RED} (7 of 9)${BLUE} Installing TaraCamViewer${NC}"
	@cp $(TARA_CAM_DIR)/TaraCamViewer $(TARA_BIN_DIR);
	
	@echo "${RED} (8 of 9)${BLUE} Installing TaraDepthViewer${NC}"
	@cp $(TARA_DEP_DIR)/TaraDepthViewer $(TARA_BIN_DIR);
	
	@echo "${RED} (9 of 9)${BLUE} Installing TaraDisparityViewer${NC}"
	@cp $(TARA_DIS_DIR)/TaraDisparityViewer $(TARA_BIN_DIR);
	
	@echo "\n${BLUE}Copying haarcascade_frontalface_alt2.xml from the OpenCV data directory${NC}"
	@cp $(HAAR_FACE_XML)/haarcascade_frontalface_alt2.xml $(TARA_BIN_DIR)/Face
		
	@echo "\n${GREEN}${BOLD}Binaries Installed${NC}"	

	@#Install the libraries to the TARA_LIB_DIR
	@echo "\n${BLUE}${BOLD}Installing libraries${NC}"
	@echo "${BLUE}Creating a lib directory${NC}"		
	@test -z "$(TARA_LIB_DIR)" || $(MKDIR_P) "$(TARA_LIB_DIR)";
	@echo "\n${BLUE}Installing the libraries to the lib directory created${NC}"
	
	@echo "${RED} (1 of 2)${BLUE} Installing libecon_tara.so${NC}"
	@cp $(LIB_TARA_DIR)/libecon_tara.so $(TARA_LIB_DIR);
	
	@echo "${RED} (2 of 2)${BLUE} Installing libecon_xunit.so${NC}"
	@cp $(LIB_XUNIT_DIR)/libecon_xunit.so $(TARA_LIB_DIR);
	
	@echo "\n${GREEN}${BOLD}Libraries Installed${NC}"
	
	@#3. Creating a .conf file for library loading
	@if test -f /etc/ld.so.conf.d/00-tara.conf;then\
		rm  /etc/ld.so.conf.d/00-tara.conf;\
	fi
	@echo "\n${BLUE}${BOLD}Creating a conf file ${RED}(/etc/ld.so.conf.d/00-tara.conf) ${BLUE}${BOLD}to link the shared libraries required for Tara-SDK applications${NC}"
	@echo "#Tara dependency Lib directory" >> /etc/ld.so.conf.d/00-tara.conf
	@echo $(TARA_LIB_DIR) >> /etc/ld.so.conf.d/00-tara.conf
	@ldconfig
				
	@#4. Changing permission of /usr/local/tara-sdk path
	@echo "\n${BLUE}${BOLD}Changing permission of ${RED}/usr/local/tara-sdk path${NC}"
	@chmod 0777 ${TARA_INSTALL_PREFIX} -R
	
	@#5. Adding a .rules file
	@if test -f /etc/udev/rules.d/60-tara.rules;then\
		rm /etc/udev/rules.d/60-tara.rules;\
	fi				
	@echo "\n${BLUE}${BOLD}Adding a .rules file ${RED}(/etc/udev/rules.d/60-tara.rules)${NC}"
	@echo "#Changing the permission to access hidraw" >> /etc/udev/rules.d/60-tara.rules
	@echo "KERNEL==\"hidraw*\", SUBSYSTEM==\"hidraw\",  ATTRS{idVendor}==\"2560\", ATTRS{idProduct}==\"c114\", MODE=\"0666\""	>> /etc/udev/rules.d/60-tara.rules
	@sudo udevadm trigger
	
	@#6. Adding the path to use sudo	
	@sed -i '/tara/d' /etc/profile
	@echo "\n${BLUE}${BOLD}Adding the path variable in ${RED}/etc/profile${NC}"
	@echo "export PATH=$$"PATH":$(TARA_BIN_DIR)" >> /etc/profile
	
	@#7. Adding the path to bashrc for autocomplete
	@echo "\n${BLUE}${BOLD}Adding the path variable in ${RED}~/.bashrc ${BLUE}${BOLD}for auto complete${NC}"
	@if test -f $(HOME)/.bashrc; then sed -i '/tara/d' $(HOME)/.bashrc; \
		echo "export PATH=$$"PATH":$(TARA_BIN_DIR)" >> $(HOME)'/'.bashrc;\
	else\
		echo "bashrc not found";\
	fi
	
	@echo "\n${GREEN}${BOLD}Installation success${NC}"	
	@echo "${BLUE}Tara-SDK installed in the directory ${RED}$(TARA_INSTALL_PREFIX)${NC}"
	@echo "\n${BLUE}${BOLD}Source the bashrc to reflect the environment changes${NC}"
	@echo "\n${RED}${BOLD}. ~/.bashrc${NC}\n"

uninstall:
	@echo "\n${GREEN}${BOLD}Uninstalling Tara SDK${NC}\n"
	
	@#1.Removing Tara installed directory
	@echo "${BLUE}${BOLD}Removing Tara installed directory${NC}"
	@if test -d $(TARA_INSTALL_PREFIX); then rm -rf $(TARA_INSTALL_PREFIX); echo "${BLUE}Removed : tara-sdk folder in /usr/local${NC}"; else echo "${RED}tara-sdk not found${NC}"; fi
	
	@#2.Removing a .conf file for library loading
	@echo "\n${BLUE}${BOLD}Removing a conf file created to load libraries${NC}"
	@if test -f /etc/ld.so.conf.d/00-tara.conf; then rm  /etc/ld.so.conf.d/00-tara.conf; echo "${BLUE}Removed : conf file created to link the shared libraries${NC}"; else echo "${RED}conf file not found${NC}"; fi
	@ldconfig
	
	@#3. Removing a .rules file added
	@echo "\n${BLUE}${BOLD}Removing a rules file created${NC}"
	@if test -f /etc/udev/rules.d/60-tara.rules;then rm /etc/udev/rules.d/60-tara.rules; echo "${BLUE}Removed : 60_tara.rules file created${NC}"; else echo "${RED}60-tara.rules file not found{NC}"; fi
	@sudo udevadm trigger
	
	@#4.Removing the path to use sudo    
	@echo "\n${BLUE}${BOLD}Removing the path variable added to use sudo${NC}"
	@sed -i '/tara/d' /etc/profile
	@echo "${BLUE}Removed : path exported to /etc/profile${NC}"
	
	@#5.Removing the path to bashrc for autocomplete
	@echo "\n${BLUE}${BOLD}Removing the path variable added to use autocomplete${NC}"
	@if test -f $(HOME)'/'.bashrc; then sed -i '/tara/d'  $(HOME)/.bashrc; echo "${BLUE}Removed : path exported to bashrc${NC}"; else echo "${RED}${BOLD}bashrc not found${NC}"; fi
	
	@echo "\n${GREEN}${BOLD}Uninstallation Success\n${NC}"

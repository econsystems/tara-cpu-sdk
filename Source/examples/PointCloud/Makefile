#Makefile to generate the PCL Application
#While executing make, PointCloud binary will be generated

#Variables and Constants
CC=g++
OUTPUT=PointCloud
OPENCV_INSTALL_PREFIX=/usr/local/tara-opencv
COMMON_LIBS_PREFIX=./../../common

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m

#Includes and libs
CFLAGS=-I $(COMMON_LIBS_PREFIX)/include -I $(OPENCV_INSTALL_PREFIX)/include `pkg-config --cflags glib-2.0 eigen3 pcl_common-1.7` -I /usr/include/vtk-6.2 
LIBS=-lboost_system -lpcl_visualization
ECON_LIBS=-L $(COMMON_LIBS_PREFIX)/Tara -lecon_tara -L $(COMMON_LIBS_PREFIX)/xunit -lecon_xunit 
OPENCV_LIBS=-L $(OPENCV_INSTALL_PREFIX)/lib -lopencv_core -lopencv_calib3d -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_ximgproc


#Building Targets
default: $(OUTPUT)
 
$(OUTPUT): PointCloudApp.cpp common_libs
	@echo "\n${BLUE}${BOLD}Building PCL Application${NC}"
	@$(CC) $< -o $@ $(CFLAGS) -Wno-deprecated $(LIBS) $(ECON_LIBS) $(OPENCV_LIBS)
	@echo "\n${GREEN}${BOLD}PCL Application build completed${NC}"		

common_libs:
	@make -C $(COMMON_LIBS_PREFIX)

clean:
	@echo "\n${BLUE}${BOLD}Removing PCL Application${NC}"
	@rm $(OUTPUT) 
	@echo "\n${GREEN}${BOLD}PCL Application removed${NC}"		

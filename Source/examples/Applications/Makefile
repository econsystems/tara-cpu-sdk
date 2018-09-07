#Makefile to generate the SDK Applications
#While executing make, 4 binary files will be generated

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m


#Building Targets
default: faceDet heightCalib heightEst volumeEst
	@echo "\n${GREEN}${BOLD}SDK Applications build completed${NC}"	

faceDet:
	@echo "\n${BLUE}${BOLD}Building SDK Applications${NC}"
	@make -C ./FaceDetection

heightCalib:
	@make -C ./HeightEstimation/HeightCalibration

heightEst:
	@make -C ./HeightEstimation/HeightEstimation

volumeEst:
	@make -C ./VolumeEstimation

clean:
	@echo "\n${BLUE}${BOLD}Cleaning the SDK Applications${NC}"
	@make clean -C ./FaceDetection
	@make clean -C ./HeightEstimation/HeightCalibration
	@make clean -C ./HeightEstimation/HeightEstimation
	@make clean -C ./VolumeEstimation
	@echo "\n${GREEN}${BOLD}SDK Applications removed${NC}"

#Makefile to generate the SDK samples and Applications
#While executing make, 9 Applications will be generated

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m


#Building Targets
default: samples applications imu pcl
	@echo "\n${GREEN}${BOLD}SDK samples and applications build completed${NC}"	

samples:
	@echo "\n${BLUE}${BOLD}Building SDK samples and applications${NC}"
	@make -C ./Samples

applications:
	@make -C ./Applications

imu:
	@make -C ./IMU

pcl:
	@make -C ./PointCloud

clean:
	@echo "\n${BLUE}${BOLD}Cleaning the SDK samples and applications${NC}"
	@make clean -C ./Samples
	@make clean -C ./Applications
	@make clean -C ./IMU
	@make clean -C ./PointCloud
	@echo "\n${GREEN}${BOLD}SDK samples and applications removed${NC}"

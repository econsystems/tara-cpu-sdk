#Makefile to generate the SDK Samples
#While executing make, 3 binary files will be generated

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m


#Building Targets
default: camviewer depthviewer disparityviewer
	@echo "\n${GREEN}${BOLD}SDK Samples build completed${NC}"	

camviewer:
	@echo "\n${BLUE}${BOLD}Building SDK Samples${NC}"
	@make -C ./TaraCamViewer

depthviewer:
	@make -C ./TaraDepthViewer

disparityviewer:
	@make -C ./TaraDisparityViewer


clean:
	@echo "\n${BLUE}${BOLD}Cleaning the SDK Samples${NC}"
	@make clean -C ./TaraCamViewer
	@make clean -C ./TaraDepthViewer
	@make clean -C ./TaraDisparityViewer
	@echo "\n${GREEN}${BOLD}SDK Samples removed${NC}"

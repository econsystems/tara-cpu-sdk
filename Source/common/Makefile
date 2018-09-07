#Makefile to generate the tara and xunit lib
#While executing make, libecon_tara.so and libecon_xunit.so will be generated

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m


#Building Targets
default: lib_tara lib_xunit
	@echo "\n${GREEN}${BOLD}Common libs build completed${NC}"	

lib_tara:
	@echo "\n${BLUE}${BOLD}Building common libs${NC}"
	@make -C ./Tara

lib_xunit:
	@make -C ./xunit

clean:
	@echo "\n${BLUE}${BOLD}Cleaning the common libs${NC}"
	@make clean -C ./Tara
	@make clean -C ./xunit
	@echo "\n${GREEN}${BOLD}Common libs removed${NC}"

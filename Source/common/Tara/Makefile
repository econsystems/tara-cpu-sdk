#Makefile to generate the tara lib
#While executing make, libecon_tara.so will be generated

#Variables and Constants
CC=g++
OUTPUT=libecon_tara.so
OPENCV_INSTALL_PREFIX=/usr/local/tara-opencv

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m

#Includes and libs
CFLAGS=-I ./../include -I $(OPENCV_INSTALL_PREFIX)/include `pkg-config --cflags glib-2.0`
LIBS=-ludev -lv4l2


#Building Targets
default: $(OUTPUT)

$(OUTPUT): Tara.cpp
	@echo "\n${RED}Building libecon_tara.so${NC}"
	@$(CC) -Wall -g -fPIC -shared $^ -o $@ $(CFLAGS) $(LIBS)
	@echo "${RED}Tara lib built${NC}"

clean:
	@echo "\n${RED}Removing libecon_tara.so${NC}"
	@rm $(OUTPUT) 
	@echo "${RED}Tara lib removed${NC}"

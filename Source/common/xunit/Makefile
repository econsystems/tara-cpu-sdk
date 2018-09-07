#Makefile to generate the extension unit lib
#While executing make, libecon_xunit.so will be generated

#Variables and Constants
CC=g++
OUTPUT=libecon_xunit.so

#Formatting options
RED=\033[0;31m
GREEN=\033[0;32m
BLUE=\033[0;34m
NC=\033[0m # No Color
BOLD=\033[1m

#Includes and libs
CFLAGS=-I ./../include


#Building Targets
default: $(OUTPUT)

$(OUTPUT): xunit_lib_tara.cpp
	@echo "\n${RED}Building libecon_xunit.so${NC}"
	@$(CC) -Wall -g -fPIC -shared $^ -o $@ $(CFLAGS)
	@echo "${RED}xunit lib built${NC}"	

clean:
	@echo "\n${RED}Removing libecon_xunit.so${NC}"
	@rm $(OUTPUT) 
	@echo "${RED}xunit lib removed${NC}"	

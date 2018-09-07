#!/bin/bash
#Uninstallation script for tara-sdk

#Formatting options
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color
BOLD='\033[1m'

#Setting the directories
TARA_INSTALL_PREFIX="/usr/local/tara-sdk"

if [ "$EUID" -ne 0 ]; then
	echo -e "\n${RED}${BOLD}Please run as root${NC}"
	echo -e "\n${RED}${BOLD}cmd : sudo ./uninstall.sh${NC}"
else
	echo -e "\n${GREEN}${BOLD}Uninstalling Tara SDK${NC}\n"

	#1.Removing Tara installed directory
	echo -e "${BLUE}${BOLD}Removing Tara installed directory${NC}"

	if [ -d $TARA_INSTALL_PREFIX ];then
		rm -rf $TARA_INSTALL_PREFIX
		echo -e "${BLUE}Removed : tara-sdk folder in /usr/local${NC}"
	else
		echo -e "${RED}tara-sdk not found${NC}"
	fi


	#2.Removing a .conf file for library loading
	echo -e "\n${BLUE}${BOLD}Removing a conf file created to load libraries${NC}"

	if [ -f /etc/ld.so.conf.d/00-tara.conf ];then
		rm  /etc/ld.so.conf.d/00-tara.conf
		echo -e "${BLUE}Removed : conf file created to link the shared libraries${NC}"
	else
		echo -e "${RED}conf file not found${NC}"
	fi
	ldconfig

	#3. Removing .rules file added
	echo -e "\n${BLUE}Removing a .rules file added${NC}"
	
	if [ -f /etc/udev/rules.d/60-tara.rules ];then
		rm /etc/udev/rules.d/60-tara.rules
		echo -e "${BLUE}Removed : 60-tara.rules file created${NC}"
	else
		echo -e "${RED}60-tara.rules file not found${NC}"
	fi
	sudo udevadm trigger
	


	#3.Removing the path to use sudo    
	echo -e "\n${BLUE}${BOLD}Removing the path variable added to use sudo${NC}"
	sed -i '/tara/d' /etc/profile
	echo -e "${BLUE}Removed : path exported to /etc/profile${NC}"


	#4.Removing the path to bashrc for autocomplete
	echo -e "\n${BLUE}${BOLD}Removing the path variable added to use autocomplete${NC}"
	if [ -f $HOME'/'.bashrc ];then
		sed -i '/tara/d'  $HOME/.bashrc
		echo -e "${BLUE}Removed : path exported to bashrc${NC}"
	else
		echo -e "${RED}${BOLD}bashrc not found${NC}"
	fi

	echo -e "\n${GREEN}${BOLD}Uninstallation Success\n${NC}"
fi

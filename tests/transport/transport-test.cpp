/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskj�r				<frhag10@student.sdu.dk>
#						Kent Stark Olsen				<keols09@student.sdu.dk>
#						Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#						Leon Bonde Larsen				<lelar09@student.sdu.dk>
#						Rudi Hansen						<rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File:     transport-test.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# 
********************************************************************************/

//#include <boost/lexical_cast.hpp> 
#include <iostream>
#include "transport.h"

void testPortAssign();
void portList();

int main()
{

	testPortAssign();


	return 0;
}

void testPortAssign()
{
	Transport trans[6];

	for (int i=0; i<6; i++)
	{
		try {
			trans[i].setPort();
		}
		catch(char *str) {
			std::cout << "EXCEPTION: " << str << std::endl;
		}
		std::cout << i << (char)9 << (int)trans[i].getPort() << std::endl;
	}

	//std::cout << "isSet =" << (char)9 << (int)trans[4].isPortSet() << std::endl;

	std::cout << std::endl << "Press any key to continue..." << std::endl;
	std::cin.get();

	Transport transTwo;
	transTwo.setPort(34);

	std::cout << std::endl << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << std::endl;
	bool *portTable = trans[1].getPortTable();
	int j1, j2, j3, j4;
	for (int k=0; k<64; k++) {
		j1=k;
		j2=k+64;
		j3=k+128;
		j4=k+192;
		std::cout << j1 << (char)9 << (int)portTable[j1] << (char)9 
		          << j2 << (char)9 << (int)portTable[j2] << (char)9
				  << j3 << (char)9 << (int)portTable[j3] << (char)9
				  << j4 << (char)9 << (int)portTable[j4] << std::endl;
	}

	std::cout << std::endl << "Press any key to continue..." << std::endl;
	std::cin.get();
}


void portList()
{
	Transport trans;

	std::cout << std::endl << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << std::endl;
	bool *portTable = trans.getPortTable();
	int j1, j2, j3, j4;
	for (int k=0; k<64; k++) {
		j1=k;
		j2=k+64;
		j3=k+128;
		j4=k+192;
		std::cout << j1 << (char)9 << (int)portTable[j1] << (char)9 
		          << j2 << (char)9 << (int)portTable[j2] << (char)9
				  << j3 << (char)9 << (int)portTable[j3] << (char)9
				  << j4 << (char)9 << (int)portTable[j4] << std::endl;
	}
}
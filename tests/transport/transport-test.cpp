/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskjær				<frhag10@student.sdu.dk>
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
# Version:  
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# 
********************************************************************************/

#include <iostream>
#include <iomanip>
#include <bitset>
#include "transport.h"

void portAssignTest();
void packetMakeTest();
void portList();


int main()
{
	portAssignTest();

	return 0;
}


void packetMakeTest()
{
	Packet packet[2];
	unsigned char data[45] = {1,5,7,3,2,4,5};
	unsigned char length = 45;

	try {
		packet[0].make("syn",2,78);
		packet[1].make("ack",1,1,1,1,data,length,true);
	}
	catch (char *str) {
		std::cout << "EXCEPTION: " << str << std::endl;
	}

	std::cout << packet[0].checksumValid();
	std::cout << packet[1].checksumValid();
}

void portAssignTest()
{
	Transport trans[6];

	for (int i=0; i<6; i++)
	{
		try {
			trans[i].setPort();
		}
		catch(char *str) {
			std::cout << str << std::endl;
		}
		std::cout << i << (char)9 << (int)trans[i].port() << std::endl;
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
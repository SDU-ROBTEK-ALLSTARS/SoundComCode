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
#include <boost/circular_buffer.hpp>
#include <iostream>
#include <iomanip>
#include <bitset>
#include "transport.h"
#include "packet.h"


void portAssignTest();
void packetMakeTest();
void portList();
void testToFromCharPacket();

int main()
{
	testToFromCharPacket();

	return 0;
}


void testToFromCharPacket()
{
	Transport tp;
	tp.setPort();

	Packet packet;

	boost::circular_buffer<unsigned char> buf(255);
	boost::circular_buffer<unsigned char> *cb = &buf;

	unsigned char data[45] = {1,5,7,3,2,4,5,1,5,7,5,6,47,4,9,7};
	unsigned char length = 45;

	try {
		packet.make("ack",1,1,1,1,data,length,true);
	
		std::cout << "Checksum validity: " << packet.checksumValid() << std::endl;
		std::cout << "Total package length: " << (int)packet.totalLength() << std::endl;

		unsigned char *packetAsArray = packet.getAsArray();
		
		//fill boost circ buffer
		for (unsigned char i=0; i<packet.totalLength(); i++) {
			cb->push_back(packetAsArray[i]);
			std::cout << (int)packetAsArray[i] << " ";
		}
		std::cout << std::endl;

		std::cout << "Circ buffer length: "  << cb->size() << std::endl;
		std::cout << "Circ buffer space available: " << (cb->capacity() - cb->size()) << std::endl;
		//see the data in buffer
		for (unsigned char i=0; i<packet.totalLength(); i++) {
			std::cout << (int)cb->at(i) << " "; //Data is off-set by HLEN
		}
		std::cout << std::endl;


		tp.decode(cb, cb); //

		std::cout << "Circ buffer length: "  << cb->size() << std::endl;
		std::cout << "Circ buffer space available: " << (cb->capacity() - cb->size()) << std::endl;
		//see the data in buffer
		for (unsigned char i=0; i<packet.totalLength(); i++) {
			std::cout << (int)cb->at(i) << " "; //Data is off-set by HLEN
		}
		std::cout << std::endl;

	}
	catch (char *str) {
		std::cout << "EXCEPTION: " << str << std::endl;
	}
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
/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik HagelskjÊr				<frhag10@student.sdu.dk>
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
# File:     test_2x_dll.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik HagelskjÊr				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-10-30
********************************************************************************
# Description
#
# The purpose of this test is to have two independent data link layer objects
# communicate to transfer data.
#                           (Denne header er dikteret af den onde diktator Kent)
*******************************************************************************/

//*****     DEFINES     *****
//buffer i/o
#define 	BUFFERSIZE 		300
#define 	INPUTUP 		"inputRight.dat"
#define 	INPUTDOWN 		"inputLeft.dat"
#define 	OUTPUTUP 		"outputRight.dat"
#define 	OUTPUTDOWN 		"outputLeft.dat"
#define		COUNTER			20000000
#define		TEST_ITERATIONS	5
#define		PACKET_SIZE		2

//*****     INCLUDES     *****
//general
#include 	<iostream>
#include 	<fstream>
#include 	<string>
#include 	<boost/circular_buffer.hpp>

//physical layer
#include "../src/physical/DtmfPhysical.h"
#include "../src/portaudio.h"

//data link layer
#include 	"../src/data_link/DtmfDatalinkLayer.h"
#include	"../src/buffers/frame.h"

//transport layer
#include	"../src/buffers/packet.h"

//*****     FUNCTIONS     *****
void wait(int time)
{
	double count = 0;
	int i;
	for(i=0;i<time+3;i++)
	{
	while(count<COUNTER)
		count++;
	count=0;
	}
}

//*****     MAIN ENTRY POINT     *****
int main()
{
	//i/o streams
	std::ofstream outputright;
	std::ofstream outputleft;
	outputright.open(OUTPUTUP);
	outputleft.open(OUTPUTDOWN);

	//instantiate buffers
	boost::circular_buffer< Packet > inputRight(BUFFERSIZE);   //downwards input Right
	boost::circular_buffer< Packet > inputLeft(BUFFERSIZE);   //downwards input Left

	boost::circular_buffer< unsigned int > outputRight(BUFFERSIZE); //upwards output Right
	boost::circular_buffer< unsigned int > outputLeft(BUFFERSIZE);  //upwards output Left

	boost::circular_buffer< Frame > movingLeft(BUFFERSIZE);   		//upwards from phys to left dll
	boost::circular_buffer< Frame > movingRight(BUFFERSIZE);   	//downwards from left dll to phys

	#ifdef DEBUG
	DEBUG_OUT << "----------   ### INITIALIZING BUFFERS  ###   ----------" << std::endl;
	#endif

	//make a packet
	Packet test1;
	unsigned char data[PACKET_SIZE];
	for(int k=0;k<PACKET_SIZE;k++)
		data[k]=k;
	unsigned char length = PACKET_SIZE;
	bool flags[8] = {false};
	flags[PACKET_FLAG_ACK] = true;
	test1.make(5,2,flags,7,8,length,data);

	//Put packets in buffers
	test1.setRecvAddr(0);
	inputLeft.push_back(test1);
	inputLeft.push_back(test1);

	#ifdef DEBUG
	DEBUG_OUT << "Checksum validity: " << test1.checksumValid() << std::endl;
	DEBUG_OUT << "Total package length: " << (int)test1.totalLength() << std::endl;
	unsigned char* arrayPacket = test1.getAsArray();
	for (int i=0; i<test1.totalLength(); i++)
		DEBUG_OUT << (int)arrayPacket[i] << " ";
	DEBUG_OUT << std::endl;
	#endif

	test1.setRecvAddr(1);
	inputRight.push_back(test1);

	#ifdef DEBUG
	DEBUG_OUT << "Checksum validity: " << test1.checksumValid() << std::endl;
	DEBUG_OUT << "Total package length: " << (int)test1.totalLength() << std::endl;
	arrayPacket = test1.getAsArray();
	for (int i=0; i<test1.totalLength(); i++)
		DEBUG_OUT << (int)arrayPacket[i] << " ";
	DEBUG_OUT << std::endl;
	#endif

	//instantiate data link layer
	DtmfDataLinkLayer Leftdll(1,1); //addr = 1, has token
	DtmfDataLinkLayer Rightdll(0,0); //addr = 0, has no token

	//call encode or decode with arguments downInput, downOutput, upInput, upOutput
	for(int k=0;k<30;k++)
	{
	#ifdef DEBUG
		DEBUG_OUT << std::endl << " LEFT DATA LINK LAYER:";
	#endif
	Leftdll.decode(&inputLeft, &movingRight, &movingLeft, &outputLeft);
	wait(1);
	Leftdll.encode(&inputLeft, &movingRight, &movingLeft, &outputLeft);

	#ifdef DEBUG
	DEBUG_OUT << std::endl << " RIGHT DATA LINK LAYER:";
	#endif
	Rightdll.decode(&inputRight, &movingLeft, &movingRight, &outputRight);
	wait(1);
	Rightdll.encode(&inputRight, &movingLeft, &movingRight, &outputRight);
	wait(1);
	}

#ifdef DEBUG
DEBUG_OUT << std::endl << "----------   ### WRITING BUFFERS TO FILES ###   ----------" << std::endl;
#endif

	//write output buffers to files. Results in "Assertion failed:" if buffer is empty, but executes anyway
	while(true)
		{
			if(outputLeft.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing datagram..." << outputLeft.front() << std::endl;
				#endif
				outputleft << (int)outputLeft.front() << std::endl;
				outputLeft.pop_front();
			}
		}

	while(true)
		{
			if(outputRight.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing datagram..." << outputRight.front() << std::endl;
				#endif
				outputright << (int)outputRight.front() << std::endl;
				outputRight.pop_front();
			}
		}
	return 0;
}

//End Of File

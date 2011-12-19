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
# File:     test_2x_dll.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
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
#define		COUNTER			10000000
#define		PACKET_SIZE		17
#define		MAX_ITERATIONS	300
#define		DEBUG

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
	//variables
	time_t timestampStart;
	time_t timestampEnd;
	int ERROR_PERCENTAGE;
	int k;

	//i/o streams
	std::ofstream report;

	report.open("test_report.txt");

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
	inputRight.push_back(test1);

	#ifdef DEBUG
	DEBUG_OUT << "Checksum validity: " << test1.checksumValid() << std::endl;
	DEBUG_OUT << "Total package length: " << (int)test1.totalLength() << std::endl;
	arrayPacket = test1.getAsArray();
	for (int i=0; i<test1.totalLength(); i++)
		DEBUG_OUT << (int)arrayPacket[i] << " ";
	DEBUG_OUT << std::endl;
	#endif

	outputLeft.push_back(0);
	outputRight.push_back(0);

	//instantiate data link layer
	DtmfDataLinkLayer Leftdll(1,1); //addr = 1, has token
	DtmfDataLinkLayer Rightdll(0,0); //addr = 0, has no token

	for(int l=0;l<=81;l++)
	{
		ERROR_PERCENTAGE = l;
		Leftdll.ERROR_PERCENTAGE = ERROR_PERCENTAGE;
		Rightdll.ERROR_PERCENTAGE = ERROR_PERCENTAGE;

		//timestamp
		time ( &timestampStart );

		//call encode or decode with arguments downInput, downOutput, upInput, upOutput
		for(k=0;k<MAX_ITERATIONS;k++)
		{
#ifdef DEBUG
			DEBUG_OUT << std::endl << " LEFT DATA LINK LAYER:" << std::endl;
#endif
			Leftdll.decode(&inputLeft, &movingRight, &movingLeft, &outputLeft);
			wait(1);
			Leftdll.encode(&inputLeft, &movingRight, &movingLeft, &outputLeft);
			wait(1);

#ifdef DEBUG
			DEBUG_OUT << std::endl << " RIGHT DATA LINK LAYER:" << std::endl;
#endif
			Rightdll.decode(&inputRight, &movingLeft, &movingRight, &outputRight);
			wait(1);
			Rightdll.encode(&inputRight, &movingLeft, &movingRight, &outputRight);
			wait(1);

			if(outputRight.size()>=(PACKET_SIZE+8) && outputLeft.size()>=(PACKET_SIZE+8))
				break;
		}
		outputRight.clear();
		outputLeft.clear();
		time ( &timestampEnd );

		report << "----------   ### REPORT ###   ----------" << std::endl;
		report << "Four packets of " << PACKET_SIZE << " bytes transferred in " <<
				(MAX_TIME_TO_REPLY-(timestampStart-timestampEnd)) << " seconds" << std::endl;
		report << "Error percentage setting: " << ERROR_PERCENTAGE << "%" << std::endl;
		report << "Number of errors generated. Left: " << Leftdll.generatedError << " Right: "
				<< Rightdll.generatedError << ". Total: " << Leftdll.generatedError+Rightdll.generatedError << std::endl;
		report << "Right data link layer discarded " << Rightdll.lostFrameCount << " frames." << std::endl;
		report << "Right data link layer transferred " << Rightdll.successFrameCount << " frames." << std::endl;
		report << std::endl;
		report << "Left data link layer discarded " << Leftdll.lostFrameCount << " frames." << std::endl;
		report << "Left data link layer transferred " << Leftdll.successFrameCount << " frames." << std::endl;
		report << std::endl;
		report << std::endl;

#ifdef DEBUG
		DEBUG_OUT << "----------   ### REPORT ###   ----------" << std::endl;
		DEBUG_OUT << "Four packets of " << PACKET_SIZE << " bytes transferred in " <<
				(MAX_TIME_TO_REPLY-(timestampStart-timestampEnd)) << " seconds" << std::endl;
		DEBUG_OUT << "Error percentage setting: " << ERROR_PERCENTAGE << "%" << std::endl;
		DEBUG_OUT << "Number of errors generated. Left: " << Leftdll.generatedError << " Right: "
				<< Rightdll.generatedError << ". Total: " << Leftdll.generatedError+Rightdll.generatedError << std::endl;
		DEBUG_OUT << "Right data link layer discarded " << Rightdll.lostFrameCount << " frames." << std::endl;
		DEBUG_OUT << "Right data link layer transferred " << Rightdll.successFrameCount << " frames." << std::endl;
		DEBUG_OUT << std::endl;
		DEBUG_OUT << "Left data link layer discarded " << Leftdll.lostFrameCount << " frames." << std::endl;
		DEBUG_OUT << "Left data link layer transferred " << Leftdll.successFrameCount << " frames." << std::endl;
		DEBUG_OUT << std::endl;
		DEBUG_OUT << std::endl;
#endif
	}


	return 0;
}

	//End Of File

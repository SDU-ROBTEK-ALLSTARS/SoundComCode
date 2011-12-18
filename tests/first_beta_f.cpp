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
//	unsigned int byte1;
	int timeToWait;

	//i/o streams
//	std::ifstream inputright;
//	std::ifstream inputleft;
	std::ofstream outputright;
	std::ofstream outputleft;

//	//open files
//	inputright.open(INPUTUP);
//	inputleft.open(INPUTDOWN);
//	if(inputright.fail() || inputleft.fail())
//	{
//		DEBUG_OUT << "Error opening input file";
//		exit(1);
//	}
	outputright.open(OUTPUTUP);
	outputleft.open(OUTPUTDOWN);

	//instantiate buffers
	boost::circular_buffer< Packet > inputBufferRight(BUFFERSIZE);   //downwards input Right
	boost::circular_buffer< Packet > inputBufferLeft(BUFFERSIZE);   //downwards input Left

	boost::circular_buffer< unsigned int > outputRight(BUFFERSIZE); //upwards output Right
	boost::circular_buffer< unsigned int > outputLeft(BUFFERSIZE);  //upwards output Left

	boost::circular_buffer< Frame > physLeftUp(BUFFERSIZE);   		//upwards from phys to left dll
	boost::circular_buffer< Frame > physLeftDown(BUFFERSIZE);   	//downwards from left dll to phys
	boost::circular_buffer< Frame > physRightUp(BUFFERSIZE);   		//upwards from phys to right dll
	boost::circular_buffer< Frame > physRightDown(BUFFERSIZE);   	//downwards from right dll to phys


#ifdef DEBUG
DEBUG_OUT << "----------   ### INITIALIZING BUFFERS  ###   ----------" << std::endl;
#endif
//make a packet
Packet test1;
unsigned char data[247];
for(int k=0;k<247;k++)
 data[k]=k;
unsigned char length = 247;
bool flags[8] = {false};
flags[PACKET_FLAG_ACK] = true;
test1.make(5,2,flags,7,8,length,data);
#ifdef DEBUG
DEBUG_OUT << "Checksum validity: " << test1.checksumValid() << std::endl;
DEBUG_OUT << "Total package length: " << (int)test1.totalLength() << std::endl;
unsigned char* arrayPacket = test1.getAsArray();
for (int i=0; i<test1.totalLength(); i++) {
	DEBUG_OUT << (int)arrayPacket[i] << " ";
}
#endif

//make another packet
//Packet test2;
//unsigned char data1[30] = {1,5,7,3,2,4,5,1,5,7,5,6,47,4,9,7,1,2,3,4,5,255,254,253,252,251};
//unsigned char length1 = 30;
//bool flags1[8] = {false};
//flags1[PACKET_FLAG_ACK] = true;
//test1.make(200,201,flags1,202,203,length1,data1);
//#ifdef DEBUG
//DEBUG_OUT << "Checksum validity: " << test1.checksumValid() << std::endl;
//DEBUG_OUT << "Total package length: " << (int)test1.totalLength() << std::endl;
//#endif

inputBufferLeft.push_back(test1);
inputBufferRight.push_back(test1);

////fill input buffers
//	while( !inputleft.eof() )
//	{
//		inputleft >> byte1;
//		#ifdef DEBUG
//		DEBUG_OUT << "Buffering datagram byte from left file...";
//		for(int i=7;i>=0;i--)
//			DEBUG_OUT << (bool)(byte1 & (1<<i));
//		DEBUG_OUT << "(" << byte1 << ")"<< std::endl;
//		#endif
//		inputLeft.push_back(byte1);
//	}
//
//	while( !inputright.eof() )
//	{
//		inputright >> byte1;
//		#ifdef DEBUG
//		DEBUG_OUT << "Buffering datagram byte from right file...";
//		for(int i=7;i>=0;i--)
//			DEBUG_OUT << (bool)(byte1 & (1<<i));
//		DEBUG_OUT << "(" << byte1 << ")"<< std::endl;
//		#endif
//		inputRight.push_back(byte1);
//	}

	//instantiate data link layer
	DtmfDataLinkLayer Leftdll(1,1); //addr = 1, has token
	//DtmfDataLinkLayer Rightdll(0,0); //addr = 0, has no token

	//instantiate physical layer
	DtmfPhysical physicalLayer(paFloat32, 2, 500, 8000, paFloat32, 2, 250, 8000);
	physicalLayer.startDataStream();

	//call encode and decode with arguments downInput, downOutput, upInput, upOutput
	while(true)
	{
		//left side
		#ifdef DEBUG
		DEBUG_OUT << std::endl << " LEFT SIDE:";
		#endif
		Leftdll.decode(&inputBufferLeft, &physLeftDown, &physLeftUp, &outputLeft);
		Leftdll.encode(&inputBufferLeft, &physLeftDown, &physLeftUp, &outputLeft);
		wait(1);
		if(!physLeftDown.empty())
		{
			#ifdef DEBUG
			DEBUG_OUT << std::endl << "----------   ###   SEND   ###   ----------" << std::endl;
			#endif
			timeToWait = physLeftDown.end() - physLeftDown.begin();
			#ifdef DEBUG
			DEBUG_OUT << timeToWait << " frames to send" << std::endl;
			#endif
			physicalLayer.send(&physLeftDown);
			physLeftDown.clear();				//TODO: temporary hack. send method should pop, but does not
			wait(timeToWait);
		}
		physicalLayer.receive(&physRightUp);

		////right side
		//#ifdef DEBUG
		//DEBUG_OUT << std::endl << " RIGHT SIDE:";
		//#endif
		//Rightdll.decode(&inputBufferRight, &physRightDown, &physRightUp, &outputRight);
		//Rightdll.encode(&inputBufferRight, &physRightDown, &physRightUp, &outputRight);
		//wait(1);
		//if(!physRightDown.empty())
		//{
		//	#ifdef DEBUG
		//	DEBUG_OUT << std::endl << "----------   ###   SEND   ###   ----------" << std::endl;
		//	#endif
		//	timeToWait = physRightDown.end() - physRightDown.begin();
		//	#ifdef DEBUG
		//	DEBUG_OUT << timeToWait << " frames to send" << std::endl;
		//	#endif
		//	physicalLayer.send(&physRightDown);
		//	physRightDown.clear();			//TODO: temporary hack. send method should pop, but does not
		//	wait(timeToWait);
		//}
		//physicalLayer.receive(&physLeftUp);
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

	//print report
	#ifdef DEBUG
	DEBUG_OUT << "----------   ### REPORT ###   ----------" << std::endl;
	/*DEBUG_OUT << "Right data link layer discarded " << Rightdll.lostFrameCount << " frames." << std::endl;
	DEBUG_OUT << "Right data link layer transferred " << Rightdll.successFrameCount << " frames." << std::endl;*/
	DEBUG_OUT << std::endl;
	DEBUG_OUT << "Left data link layer discarded " << Leftdll.lostFrameCount << " frames." << std::endl;
	DEBUG_OUT << "Left data link layer transferred " << Leftdll.successFrameCount << " frames." << std::endl;
	#endif

	return 0;
}

//End Of File

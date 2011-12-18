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
#define 	BUFFERSIZE 		100
#define 	INPUTUP 		"inputRight.dat"
#define 	INPUTDOWN 		"inputLeft.dat"
#define 	OUTPUTUP 		"outputRight.dat"
#define 	OUTPUTDOWN 		"outputLeft.dat"

//*****     INCLUDES     *****
//general
#include 	<iostream>
#include 	<fstream>
#include 	<string>
#include 	<boost/circular_buffer.hpp>


//data link layer
#include 	"data_link/dataLinkLayer.h"
#include	"buffers/frame.h"

//transport layer
#include	"transport/packet.h"

int main()
{
	//variables
	unsigned int byte1;
	double count;

	//i/o streams
	std::ifstream inputright;
	std::ifstream inputleft;
	std::ofstream outputright;
	std::ofstream outputleft;

	//open files
	inputright.open(INPUTUP);
	inputleft.open(INPUTDOWN);
	if(inputright.fail() || inputleft.fail())
	{
		DEBUG_OUT << "Error opening input file";
		exit(1);
	}
	outputright.open(OUTPUTUP);
	outputleft.open(OUTPUTDOWN);

	//instantiate buffers
	boost::circular_buffer< Packet > inputRight(BUFFERSIZE);   //downwards input Right
	boost::circular_buffer< Packet > inputLeft(BUFFERSIZE);   //downwards input Left

	boost::circular_buffer< unsigned int > outputRight(BUFFERSIZE);   //upwards output Right
	boost::circular_buffer< unsigned int > outputLeft(BUFFERSIZE);   //upwards output Left
	boost::circular_buffer< Frame > movingRightDll(BUFFERSIZE);   //dll communication Right
	boost::circular_buffer< Frame > movingLeftDll(BUFFERSIZE);   //dll communication Left

#ifdef DEBUG
DEBUG_OUT << "----------   ### INITIALIZING BUFFERS FROM FILES ###   ----------" << std::endl;
#endif

	//fill input buffers
	while( !inputleft.eof() )
	{
		inputleft >> byte1;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering datagram byte from file...";
		for(int i=7;i>=0;i--)
			DEBUG_OUT << (bool)(byte1 & (1<<i));
		DEBUG_OUT << std::endl;
		#endif
		inputLeft.push_back(byte1);
	}

	while( !inputright.eof() )
	{
		inputright >> byte1;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering datagram byte from file...";
		for(int i=7;i>=0;i--)
			DEBUG_OUT << (bool)(byte1 & (1<<i));
		DEBUG_OUT << std::endl;
		#endif
		inputRight.push_back(byte1);
	}

	//instantiate data link layer
	DataLinkLayer Leftdll(0,1); //addr = 0, has token
	DataLinkLayer Rightdll(1,0); //addr = 1, has no token

	//call encode or decode with arguments downInput, downOutput, upInput, upOutput
	for(int k=0;k<30;k++)
	{
	#ifdef DEBUG
		DEBUG_OUT << std::endl << " LEFT DATA LINK LAYER:";
	#endif
	Leftdll.decode(&inputLeft, &movingRightDll, &movingLeftDll, &outputLeft);
	count = 0;
	while(count<50000000)
		count++;
	Leftdll.encode(&inputLeft, &movingRightDll, &movingLeftDll, &outputLeft);
	count = 0;
	while(count<50000000)
		count++;
	#ifdef DEBUG
	DEBUG_OUT << std::endl << " RIGHT DATA LINK LAYER:";
	#endif
	Rightdll.decode(&inputRight, &movingLeftDll, &movingRightDll, &outputRight);
	count = 0;
	while(count<50000000)
		count++;
	Rightdll.encode(&inputRight, &movingLeftDll, &movingRightDll, &outputRight);
	count = 0;
	while(count<50000000)
		count++;
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

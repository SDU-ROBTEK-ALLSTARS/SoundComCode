/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskjær				<frhag10@student.sdu.dk>
#						Kent Stark Olsen				<keols09@student.sdu.dk>
#						Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#						Leon Bonde Larsen				<lelar09@student.sdu.dk>
#						Rudi Hansen						<dimx@dimx.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File:     main.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-12-085
********************************************************************************
# Description
#
# This program tests the performance of the physical layer on one computer.
*******************************************************************************/
//	STL include
#include <iostream>
#include <windows.h>
#include <ctime>
//	Boost include
#include <boost/circular_buffer.hpp>
//	Own includes
#include "../../src/physical/DtmfPhysical.h"
#include "../../src/portaudio.h"
#include "../../src/buffers/frame.h"

#define SAMPLE_RATE					(8000)
#define NUMBER_OF_OUTPUT_SAMPLES	(1000)
#define NUMBER_OF_INPUT_SAMPLES		(NUMBER_OF_OUTPUT_SAMPLES/2)
#define BUFFER_SIZE					(100)

void gotoxy( short x, short y )
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y }; 
	SetConsoleCursorPosition( hStdout, position );
}

int random (int min, int max)
{	
	return ((std::rand() % max) + min);
}

int main (void)
{
	std::cout << "Initializing...";
	
	//	Seed random generator
	std::srand((unsigned)std::time(0));

	//	Buffers
	boost::circular_buffer<Frame> outputBuffer(BUFFER_SIZE + 2);
	boost::circular_buffer<Frame> inputBuffer(BUFFER_SIZE);
			
	for (int i = 0 + outputBuffer.size(); i < (int)outputBuffer.capacity(); i++)
	{
		outputBuffer.push_back(Frame(random(0, 255),random(0, 255),random(0, 255)));
	}

	boost::circular_buffer<Frame> originalBuffer(outputBuffer);

	//	Physical layer
	DtmfPhysical dtmfPhysical(paFloat32, 2, NUMBER_OF_OUTPUT_SAMPLES, SAMPLE_RATE, paFloat32, 2, NUMBER_OF_INPUT_SAMPLES, SAMPLE_RATE);
	dtmfPhysical.startDataStream();
		
	std::cout << "\n\nReady for performance test...\n\n";

	std::system("PAUSE");

	std::cout << "\nRunning performance test...";
	
	//	Send frames to physical layer
	dtmfPhysical.send(&outputBuffer);
	float startSize = dtmfPhysical.sendBufferSize(), percent;

	//	Loop until finished playing
	std::clock_t startTime, stopTime;
	startTime = std::clock();
	while (true)
	{
		if (!dtmfPhysical.sendBufferSize()) break;
		if (!((std::clock() - startTime) % 20))
		{
			percent = (startSize - (float)dtmfPhysical.sendBufferSize()) / startSize * 100.0f;
			gotoxy(0, 8);
			std::cout << percent << " % finished         ";
		}
	}
	stopTime = std::clock();
	gotoxy(0, 8);
	std::cout << "100.000 % finished         ";
	
	//	Receive frames from pysical layer
	dtmfPhysical.receive(&inputBuffer);

	//	Frames ok???
	unsigned int okFrames = 0;
	for (int i = 0; i < inputBuffer.size(); i++)
	{
		if (inputBuffer[i].checkParity()) okFrames++;
	}

	//	Calculations
	float dt = (float)(stopTime - startTime) / 1000.0f;
	float bitPerSec = (float)(BUFFER_SIZE - 2) * 3.0f * 6.0f * 4.0f / dt;		//	numberOfFrames * 3bytes * 6Nibbles * 4bit / dt
	float reliablility = (float)inputBuffer.size() * 100.0f / (float)(originalBuffer.size() - 2);
	float reliablilityFramesOk = (float)okFrames * 100.0f / (float)(originalBuffer.size() - 2);
	float actualBitPerSec = bitPerSec * reliablility / 100.0f;
	float actualFramesOkBitPerSec = bitPerSec * reliablilityFramesOk / 100.0f;
	
	//	Output results
	std::cout << "\n\nPerformance results (" << dt << " s):";
	std::cout << "\n------------------------------------------------------------------";
	std::cout << "\nBuffer info:";
	std::cout << "\nOriginal buffer size (frames sent): " << (float)(originalBuffer.size() - 2);
	std::cout << "\nInput buffer size (frames received): " << (float)inputBuffer.size();
	std::cout << "\nParity check (frames ok): " << okFrames;
	std::cout << "\n------------------------------------------------------------------";
	std::cout << "\nSending bitrate: " << bitPerSec;
	std::cout << "\nReceiving bitrate (frames recived): " << actualBitPerSec;
	std::cout << "\nReceiving bitrate (frames ok): " << actualBitPerSec;
	std::cout << "\nReliability (frames recived): " << reliablility << " %";
	std::cout << "\nReliability (frames ok): " << reliablilityFramesOk << " %";
	std::cout << "\n------------------------------------------------------------------\n\n";
	
	std::system("PAUSE");

	std::cout << "\nKilling data streams...\n\n";

	dtmfPhysical.stopDataStream();
	
	return 0;
}
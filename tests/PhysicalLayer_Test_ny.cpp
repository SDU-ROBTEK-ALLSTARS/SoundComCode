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
# File:     PhysicalLayer_Test.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-12-08
********************************************************************************
# Description
#
# This test the functionality of the physical layer which include the interface
# class BufferedIOSound.
*******************************************************************************/
#include "physical/PhysicalLayer.h"
#include "portaudio.h"
#include <iostream>
#include <boost/circular_buffer.hpp>
#include "buffers/frame.h"

int main (void)
{
	std::cout << "Initializing...";
	
	boost::circular_buffer<Frame> outputBuffer(5);
	boost::circular_buffer<Frame> inputBuffer(5);
	
	PhysicalLayer physicalLayer(paFloat32, 2, 1000, 8000, paFloat32, 2, 500, 8000);
	physicalLayer.startDataStream();

	std::cout << "\nReady for playback...\n\n";

	std::system("PAUSE");
		
	outputBuffer.push_back(Frame(255,255,255));
	outputBuffer.push_back(Frame(0,0,0));
	outputBuffer.push_back(Frame(1,2,3));
	outputBuffer.push_back(Frame(4,5,6));
	outputBuffer.push_back(Frame(7,8,9));

	physicalLayer.send(&outputBuffer);

	std::system("PAUSE");

	std::cout << "\n\n----------------------------------------------------------------------------\n";
	std::cout << "Recorded frames:\n";

	physicalLayer.receive(&inputBuffer);
		
	for (int i = 0; i < inputBuffer.size(); i++)
	{
		Frame funFrame = inputBuffer[i];
		funFrame.coutHeader();
	}

	std::cout << "\n\n----------------------------------------------------------------------------\n";
	
	std::system("PAUSE");

	std::cout << "\nStopping data streams...\n\n";

	physicalLayer.stopDataStream();
	
	return 0;
}

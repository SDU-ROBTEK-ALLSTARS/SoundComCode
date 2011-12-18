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
	int foobah;
	std::cout << "Initializing...";

	PhysicalLayer physicalLayer(paFloat32, 2, 1000, 8000, paFloat32, 2, 500, 8000);
	physicalLayer.startDataStream();

	std::cout << "\nReady for playback...\n\n";

	std::cin >> foobah;

	boost::circular_buffer<Frame> frameBuffer(5);
	frameBuffer.push_back(Frame(255,255,255));
	frameBuffer.push_back(Frame(0,0,0));
	frameBuffer.push_back(Frame(1,2,3));
	frameBuffer.push_back(Frame(4,5,6));
	frameBuffer.push_back(Frame(7,8,9));

	physicalLayer.send(&frameBuffer);

	std::cin >> foobah;

	std::cout << "\n\n----------------------------------------------------------------------------\n";
	std::cout << "Recorded frames:\n";

	boost::circular_buffer<Frame> temp = physicalLayer.receive();
		
	for (int i = 0; i < temp.size(); i++)
	{
		Frame funFrame = temp[i];
		funFrame.coutHeader();
	}

	std::cout << "\n\n----------------------------------------------------------------------------\n";

	std::cin >> foobah;

	//frameBuffer.resize(12);
	frameBuffer.clear();
	frameBuffer.push_back(Frame(130,110,235));
	frameBuffer.push_back(Frame(157,50,25));
	frameBuffer.push_back(Frame(112,50,25));
	frameBuffer.push_back(Frame(1,2,3));
	frameBuffer.push_back(Frame(4,5,6));
	frameBuffer.push_back(Frame(7,8,9));
	frameBuffer.push_back(Frame(10,100,102));
	frameBuffer.push_back(Frame(123,251,25));
	frameBuffer.push_back(Frame(255,26,255));
	frameBuffer.push_back(Frame(0,102,0));
	frameBuffer.push_back(Frame(100,50,25));
	frameBuffer.push_back(Frame(32,29,5));

	physicalLayer.send(&frameBuffer);
	
//	std::cin >> foobah;

	std::cout << "\n\n----------------------------------------------------------------------------\n";
	std::cout << "Recorded frames:\n";

	temp = physicalLayer.receive();
		
	for (int i = 0; i < temp.size(); i++)
	{
		Frame funFrame = temp[i];
		funFrame.coutHeader();
	}

	std::cout << "\n\n----------------------------------------------------------------------------\n";



	std::cout << "\nStopping data streams...\n\n";

	physicalLayer.stopDataStream();
	
	return 0;
}

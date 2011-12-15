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
# File:     PhysicalLayer.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-11-21
********************************************************************************
# Description
#
# This class implements the physical layer.
*******************************************************************************/
#include "PhysicalLayer.h"

PhysicalLayer::PhysicalLayer(void)
{
	this -> communicationInterface = new BufferedSoundIO();
	this -> outputSequenceBuffer = new boost::circular_buffer<unsigned int>();
	this -> inputFrameBuffer = new boost::circular_buffer<Frame>();
}

PhysicalLayer::PhysicalLayer(	PaSampleFormat outputSampleFormat,	unsigned int outputNumberOfChannels,
								unsigned long outputBufferSize,		unsigned long outputSampleRate,
								PaSampleFormat inputSampleFormat,	unsigned int inputNumberOfChannels,
								unsigned long inputBufferSize,		unsigned long inputSampleRate)
{
	communicationInterface = new BufferedSoundIO(	outputSampleFormat,
													outputNumberOfChannels,
													outputBufferSize,
													outputSampleRate,
													inputSampleFormat,
													inputNumberOfChannels,
													inputBufferSize,
													inputSampleRate);

	this -> outputSequenceBuffer = new boost::circular_buffer<unsigned int>();
	this -> inputFrameBuffer = new boost::circular_buffer<Frame>();
}

PhysicalLayer::~PhysicalLayer(void)
{
	delete this -> communicationInterface;
	delete this -> outputSequenceBuffer;
	delete this -> inputFrameBuffer;
}

void PhysicalLayer::startDataStream (void)
{
	this -> communicationInterface -> startOutputStream();
	this -> communicationInterface -> startInputStream();
}

void PhysicalLayer::stopDataStream (void)
{
	this -> communicationInterface -> stopOutputStream();
	this -> communicationInterface -> stopInputStream();
}

int PhysicalLayer::send(void *buffer)
{
	//	Check if sending is already in progress
	if (this -> communicationInterface -> isOutputStreamRunning()) return 1;

	//	Cast sampleBuffer to proper type
	boost::circular_buffer<Frame> *data = (boost::circular_buffer<Frame>*)buffer;
	
	unsigned int bufferSize = data -> size();

	//	Temporary variables
	unsigned int j;
	unsigned int previous_nibble;

	//	Resize output sequence buffer
	this -> outputSequenceBuffer -> resize(bufferSize * MAX_NIBBLES_PER_FRAME);
	this -> outputSequenceBuffer -> clear();

	//  Generate sequence buffer
	for (unsigned int i = 0; i < bufferSize; i++)	
	{
		this -> outputSequenceBuffer -> push_back(16);

		Frame frame = Frame(data -> at(i));

		unsigned int nibbles[NUMBER_OF_NIBBLES_PER_FRAME] = {	frame.getNibble(0,0),
																frame.getNibble(1,0),
																frame.getNibble(0,1),
																frame.getNibble(1,1),
																frame.getNibble(0,2),
																frame.getNibble(1,2)};

		for (j = 0; j < NUMBER_OF_NIBBLES_PER_FRAME; j++)
		{
			if (j == 0)
			{
				this -> outputSequenceBuffer -> push_back(nibbles[j]);
				previous_nibble = nibbles[j];
			}
			else
			{
				if (nibbles[j] == previous_nibble)
				{
					this -> outputSequenceBuffer -> push_back(18);
					this -> outputSequenceBuffer -> push_back(nibbles[j]);
				}
				else
					this -> outputSequenceBuffer -> push_back(nibbles[j]);
				previous_nibble = nibbles[j];
			}
		}

		this -> outputSequenceBuffer -> push_back(17);
	}
	
	data -> clear();

	return (this -> communicationInterface -> pushSamples(this -> outputSequenceBuffer));
}

void PhysicalLayer::receive(void *buffer)
{
	return this -> communicationInterface -> pullFrames(buffer);
}
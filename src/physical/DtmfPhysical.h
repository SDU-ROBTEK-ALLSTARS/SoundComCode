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
# File:     DtmfPhysical.h
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
#pragma once
//	Boost include
#include <boost/circular_buffer.hpp>
//	Own includes
#include "BufferedSoundIO.h"
#include "../buffers/frame.h"

#define NUMBER_OF_NIBBLES_PER_FRAME	(6)
#define MAX_NIBBLES_PER_FRAME		(13)
#define FRAME_SIZE					(3)		//	[bytes]

class DtmfPhysical
{
private:
	BufferedSoundIO *communicationInterface;
	boost::circular_buffer<unsigned int> *outputSequenceBuffer;
	boost::circular_buffer<Frame> *inputFrameBuffer;
	
public:
	DtmfPhysical(void);
	DtmfPhysical(	PaSampleFormat outputSampleFormat,	unsigned int outputNumberOfChannels,
					unsigned long outputBufferSize,		unsigned long outputSampleRate,
					PaSampleFormat inputSampleFormat,	unsigned int inputNumberOfChannels,
					unsigned long inputBufferSize,		unsigned long inputSampleRate);
	~DtmfPhysical(void);

	void startDataStream (void);
	void stopDataStream (void);
	int send(void *buffer);
	void receive(void *buffer);
	unsigned int sendBufferSize (void);
	unsigned int receiveBufferSize (void);
};

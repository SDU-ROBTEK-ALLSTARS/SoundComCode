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
# File:     DtmfBackbone.h
# Project:  DtmfBackbone
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
# The Backbone class is the main engine, dispatching the different layers of the
# library to do work on the buffers. It constructs the various layers(except API),
# buffers and a runner thread, when it is constructed and destroys all those members when deleted.
# The backbone class should not be instantiated by any class other than the API class, 
# and not be deleted by any other class either.
*******************************************************************************/

#ifndef DTMFBACKBONE_H
#define DTMFBACKBONE_H

 
//Default values for buffers, these will be adjusted during testing, 
#ifdef DEBUG

//Put final values here
const unsigned int T_PFRAME_MAX=				(10);	
const unsigned int T_PFRAME_MIN=				(1);	// Just make sure there is "something" ...

const unsigned int FRAME_BUFFER_IN_SIZE=		100;
const unsigned int FRAME_BUFFER_OUT_SIZE=		100;
const unsigned int T_FRAME_MAX=					80;
const unsigned int T_FRAME_MIN=					2;

const unsigned int DATAGRAM_BUFFER_IN_SIZE=		50;
const unsigned int DATAGRAM_BUFFER_OUT_SIZE=	50;
const unsigned int T_DGRAM_MAX=					45;
const unsigned int T_DGRAM_MIN=					1;

const unsigned int SLEEPTIME_MSEC =				1;
const unsigned int MAX_FRAMES_PR_DATAGRAM =		100;
const unsigned int MAX_DATAGRAM_PR_MESSAGE =	100;
const unsigned int OUTPUT_BUFFER_SIZE =          500;
const unsigned int OUTPUT_SAMPLE_RATE =		    8000;
const unsigned int INPUT_BUFFER_SIZE =			250;
const unsigned int INPUT_SAMPLE_RATE =			8000;
#endif DEBUG



















#include <boost/thread.hpp>
#include "../buffers/DtmfBuffer.h"
#include "../data_link/DtmfDatalinkLayer.h"
#include "../transport/DtmfTransport.h"
#include "../DtmfThread.h"
#include "../physical/DtmfPhysical.h"

class DtmfApi;

class DtmfBackbone : public DtmfThread
{
private:
	int i;
	DtmfApi * dtmfapi_;
	DtmfTransport * dtmfTransport_;
	DtmfDataLinkLayer * dtmfDatalink_;
	DtmfPhysical * dtmfPhysical_;
	
	boost::mutex ** callbackMainLoopMutex_;
	void main();
#ifdef DEBUG
public:
#endif
	DtmfBuffer * dtmfBuffer_;
	bool hasRoomForDatalinkAction();
	bool hasRoomForMessageEncode();
	void moveFrameIn();
	void decodeDatagram();
	void encodeMessage();
	void dataLinkAction();
	void moveFrameOut();
	
#ifndef DEBUG
public:
#endif DEBUG
	DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferDown,DtmfMsgBuffer *& msgBufferUp,boost::mutex ** callbackMainLoopMutex,bool &hasToken,unsigned char thisAddress);
	~DtmfBackbone();
	
};
#endif DTMFBACKBONE_H

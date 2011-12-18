/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskj�r				<frhag10@student.sdu.dk>
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
#			Frederik Hagelskj�r				<frhag10@student.sdu.dk>
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
#ifdef _DEBUG
#define DEBUG
#endif _DEBUG
#define DEBUG
 
//Default values for buffers, these will be adjusted during testing, 
#ifdef DEBUG
const unsigned int T_PFRAME_MAX=				(10);
const unsigned int T_PFRAME_MIN=				(20);
const unsigned int FRAME_BUFFER_IN_SIZE=		(1000);
const unsigned int FRAME_BUFFER_OUT_SIZE=		(1000);
const unsigned int T_FRAME_MAX=					(5);
const unsigned int T_FRAME_MIN=					(10);
const unsigned int DATAGRAM_BUFFER_IN_SIZE=		(200);
const unsigned int DATAGRAM_BUFFER_OUT_SIZE=	(200);
const unsigned int T_DGRAM_MAX=					(2);
const unsigned int T_DGRAM_MIN=					(5);
const unsigned int SLEEPTIME=					(10);
const unsigned int SLEEPTIME_MSEC =				(1);
const unsigned int ADRESS =								(5);
const unsigned int MAX_FRAMES_PR_DATAGRAM =				(8);
const unsigned int MAX_DATAGRAM_PR_MESSAGE =				(10);
const unsigned int OUTPUT_BUFFER_SIZE =         500;
const unsigned int OUTPUT_SAMPLE_RATE =		    8000;
const unsigned int INPUT_BUFFER_SIZE =			250;
const unsigned int INPUT_SAMPLE_RATE =			8000;
#endif DEBUG
#ifndef DEBUG
//Put final values here
const unsigned int T_PFRAME_MAX=				(10);	
const unsigned int T_PFRAME_MIN=				(1);	// Just make sure there is "something" ...
const unsigned int FRAME_BUFFER_IN_SIZE=		100;
const unsigned int FRAME_BUFFER_OUT_SIZE=		
const unsigned int T_FRAME_MAX=					
const unsigned int T_FRAME_MIN=					
const unsigned int DATAGRAM_BUFFER_IN_SIZE=		
const unsigned int DATAGRAM_BUFFER_OUT_SIZE=	
const unsigned int T_DGRAM_MAX=					
const unsigned int T_DGRAM_MIN=					
const unsigned int SLEEPTIME=					
const unsigned int SLEEPTIME_MSEC =				
const unsigned int ADRESS =						
const unsigned int MAX_FRAMES_PR_DATAGRAM =		
const unsigned int MAX_DATAGRAM_PR_MESSAGE =	
const unsigned int OUTPUT_BUFFER_SIZE =         
const unsigned int OUTPUT_SAMPLE_RATE =		    
const unsigned int INPUT_BUFFER_SIZE =			
const unsigned int INPUT_SAMPLE_RATE =			
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
	DtmfBuffer * dtmfBuffer_;
	boost::mutex ** callbackMainLoopMutex_;
	void main();
#ifdef DEBUG
public:
#endif
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
	DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferDown,DtmfMsgBuffer *& msgBufferUp,boost::mutex ** callbackMainLoopMutex,bool &hasToken);
	~DtmfBackbone();
	
};
#endif DTMFBACKBONE_H

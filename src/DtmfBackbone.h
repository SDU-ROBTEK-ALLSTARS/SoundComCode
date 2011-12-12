//Placeholders
class placeholder
{
public:
	void encode(char * inputUp,char * inputdown,char * outputUp,char * outputDown)
	{
	}
	void decode(char * inputUp,char * inputdown,char * outputUp,char * outputDown)
	{
	}
};

typedef placeholder DtmfApi; //To simulate, randomly deposit in msgIn.
typedef placeholder DtmfSession;
typedef placeholder DtmfDatalink;
typedef placeholder DtmfPhysical;

typedef placeholder PortaudioInterface; //Essentially two buffers.


//Everything above this line should be implemented by someone at some point.






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
# library to do work on the buffers.
*******************************************************************************/







//TODO: Arv fra rudis kode... han har  bedre thread handler!

#ifndef DTMFBACKBONE_H
#define DTMFBACKBONE_H


//Default buffer værdier, pt bare bogus.
#define T_TONE_MAX	(10)
#define T_TONE_MIN   (20)
#define T_FRAME_MAX	(5)
#define T_FRAME_MIN (10)
#define T_DGRAM_MAX (2)
#define T_DGRAM_MIN (5)
#define SLEEPTIME (10)
#define FRAME_BUFFER_IN_SIZE (1000)
#define FRAME_BUFFER_OUT_SIZE (1000)
#define DATAGRAM_BUFFER_IN_SIZE (200)
#define DATAGRAM_BUFFER_OUT_SIZE (200)
#define DATAGRAM_SIZE (400)
#define FRAME_SIZE (100)
#define SLEEPTIME_MSEC (10)











#include <boost/thread.hpp>
#include "DtmfBuffer.h"


class DtmfBackbone
{
private:
	int i;
	DtmfApi * dtmfapi_;
	DtmfSession * dtmfSession_;
	DtmfDatalink * dtmfDatalink_;
	DtmfPhysical * dtmfPhysical_;
	PortaudioInterface * portaudioInterface_;
	DtmfBuffer * dtmfBuffer_;
	boost::thread workerThread_;

public:
	void operator()();
	DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferIn,DtmfMsgBuffer *& msgBufferOut);
	~DtmfBackbone();
	//ASSUMEDIRECTCONTROL() //stop workerthread.
	//RELEASEDIRECTCONTROL() //start workerthread again.
	void moveFrameIn();
	void decodeFrame();
	void decodeDatagram();
	void encodeMessage();
	void encodeDatagram();
	void moveFrameOut();
};

#endif DTMFBACKBONE_H
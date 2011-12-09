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
# File:     DtmfBuffer.h
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
# A class designated to hold all the individual buffers of the backbone application.
#
*******************************************************************************/

#ifndef DTMFBUFFER_H
#define DTMFBUFFER_H
#include "CircularCharArrayBuffer.h"
#include "DtmfMsgBuffer.h"

class DtmfBuffer
{
private:
public:
	CircularCharArrayBuffer * datagramIn;
	CircularCharArrayBuffer * datagramOut;
	CircularCharArrayBuffer * frameIn;
	CircularCharArrayBuffer * frameOut;
	DtmfMsgBuffer * msgBufferIn;
	DtmfMsgBuffer * msgBufferOut;
	DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity,int datagram_size, int frame_size);
	~DtmfBuffer();
};
#endif DTMFBUFFER_H
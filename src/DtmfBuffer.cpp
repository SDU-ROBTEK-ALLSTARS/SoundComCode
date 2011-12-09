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
# File:     DtmfBuffer.cpp
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
# Implementation of the DtmfBuffer class.
#
*******************************************************************************/
#include "DtmfBuffer.h"
DtmfBuffer::DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity,int datagram_size, int frame_size)
{
	datagramIn = new CircularCharArrayBuffer(datagram_size,datagramInCapacity);
	datagramOut  = new CircularCharArrayBuffer(datagram_size,datagramOutCapacity);
	frameIn  = new CircularCharArrayBuffer(frame_size,frameInCapacity);
	frameOut  = new CircularCharArrayBuffer(frame_size,frameOutCapacity);
	msgBufferIn = new DtmfMsgBuffer();
	msgBufferOut = new DtmfMsgBuffer();
}

DtmfBuffer::~DtmfBuffer()
{
	delete datagramIn;
	delete datagramOut;
	delete frameIn;
	delete frameOut;
	delete msgBufferIn;
	delete msgBufferOut;
}
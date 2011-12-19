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
#include "packet.h"
DtmfBuffer::DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity)
{
	//datagramIn   = new boost::circular_buffer<Datagram>(datagramInCapacity);
	//datagramOut   = new boost::circular_buffer<Datagram>(datagramInCapacity);
	dllTransportUp   = new boost::circular_buffer<unsigned int>(datagramInCapacity);
	transportDllDown = new boost::circular_buffer<Packet>(datagramInCapacity);
	physicalDllUp    = new boost::circular_buffer<Frame>(frameInCapacity);
	dllPhysicalDown  = new boost::circular_buffer<Frame>(frameOutCapacity);

	apiTransportDown = new DtmfMsgBuffer();
	transportApiUp   = new DtmfMsgBuffer();
}

DtmfBuffer::~DtmfBuffer()
{
	delete physicalDllUp;
	delete dllPhysicalDown;
	delete transportDllDown;
	delete dllTransportUp;	
	delete apiTransportDown;
	delete transportApiUp;
}
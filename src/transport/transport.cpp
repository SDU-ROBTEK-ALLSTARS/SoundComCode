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
# File:     transport.cpp
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# 
********************************************************************************/

#include <boost/circular_buffer.hpp>
#include "transport.h"
#include "datagram.h"

bool Transport::sPortsInUse_[256] = {false};

Transport::Transport()
{
	port_ = 0;
}

Transport::~Transport()
{
	sPortsInUse_[port_] = false;
}

//Datagram Transport::makeDatagram(Data_ data, unsigned char flags, unsigned char destPort)
//{
//	
//}

void Transport::setPort(const unsigned char newPort)
{
	if ((newPort > 19) && (!sPortsInUse_[newPort])) { //Port 0-19 are reserved. This could have a nicer solution
		sPortsInUse_[newPort] = true;
		port_ = newPort;
	}
	//If a port was not supplied, or the port which was supplied is not available, look for one that is
	else {
		for (int i=20; i<256; i++) { //We don't assign port 0-19
			if (!sPortsInUse_[i]) {
				sPortsInUse_[i] = true;
				port_ = (unsigned char) i;
				break;
			}
		}
	}
	if (!port_) {
		throw "[Transport] No ports available.";
	}
}

unsigned char Transport::getPort()
{
	return port_;
}

bool Transport::isPortSet(const unsigned char enteredPort)
{
	if (enteredPort) {
		if (sPortsInUse_[enteredPort])
			return true;
		else
			return false;
	}
	else {
		if (sPortsInUse_[port_])
			return true;
		else
			return false;
	}
}

bool *Transport::getPortTable()
{
	bool *pointer = sPortsInUse_;
	return pointer;
}

void Transport::decode(boost::circular_buffer<unsigned char> *ApiTransportDown,
                       boost::circular_buffer<Datagram> *TransportDllDown,
					   boost::circular_buffer<Datagram> *DllTransportUp,
					   boost::circular_buffer<unsigned char> *TransportApiUp)
{
}
	            
void Transport::encode(boost::circular_buffer<unsigned char> *ApiTransportDown,
                       boost::circular_buffer<Datagram> *TransportDllDown,
					   boost::circular_buffer<Datagram> *DllTransportUp,
					   boost::circular_buffer<unsigned char> *TransportApiUp)
{
}
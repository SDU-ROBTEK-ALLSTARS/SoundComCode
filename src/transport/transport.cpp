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
# Version:  
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# 
********************************************************************************/

#include <boost/circular_buffer.hpp>
#include <bitset>
#include "transport.h"
#include "packet.h"

bool Transport::sPortsInUse_[256] = {false};

Transport::Transport()
{
	port_ = 0;
}

Transport::~Transport()
{
	sPortsInUse_[port_] = false;
}

//void connect();

Packet Transport::assemblePacketFromBuffer()
{
	if (!DllTransportUp_->empty()) {
		//Get header off of buffer (HLEN long as defined in packet.h)
		unsigned char header[HLEN];
		for (unsigned char i=0; i<HLEN; i++) {
			header[i] = DllTransportUp_->front();
			DllTransportUp_->pop_front();
		}

		//Examine the total packet length (4th header byte) to get any data
		unsigned char *data;
		if (header[3] > HLEN) {
			unsigned char dataLength = header[3]-HLEN;
			data = new unsigned char[dataLength];
			for (unsigned char i=0; i<dataLength; i++) {
				data[i] = DllTransportUp_->front();
				DllTransportUp_->pop_front();
			}
		}

		//Now put collected data in to the established packet structure
		Packet packet;
		packet.makeIn(header, data);

		return packet;
	}
}

void Transport::processInboundPacket(Packet packet) //TO DO TO DO
{
	if ((port_ == packet.destPort()) && (packet.checksumValid())) {
		//lastInSequence_ = packet.seqNumber();
		std::bitset<8> flags (packet.flags());
		if (flags.test(SYN)) {
			int i=0;
		}
	}
}

void Transport::setPort(const unsigned char newPort)
{
	if ((newPort > 19) && (!sPortsInUse_[newPort])) { //Port 0-19 are reserved. This could have a nicer solution
		sPortsInUse_[newPort] = true;
		port_ = newPort;
	}
	else {
		//If a port was not supplied, or the port which was supplied is not available, look for one that is
		for (int i=20; i<256; i++) { //We don't assign port 0-19
			if (!sPortsInUse_[i]) {
				sPortsInUse_[i] = true;
				port_ = (unsigned char) i;
				break;
			}
		}
	}
	if (!port_) {
		throw "Transport.setPort() No ports available.";
		return;
	}
}

unsigned char Transport::port() const
{
	return port_;
}

bool Transport::isPortSet(const unsigned char enteredPort) const
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

bool *Transport::getPortTable() const
{
	bool *pointer = sPortsInUse_;
	return pointer;
}

void Transport::decode(boost::circular_buffer<unsigned char> *ApiTransportDown,
                       boost::circular_buffer<Packet> *TransportDllDown,
                       boost::circular_buffer<unsigned char> *DllTransportUp,
                       boost::circular_buffer<unsigned char> *TransportApiUp)
{
	
}

void Transport::encode(boost::circular_buffer<unsigned char> *ApiTransportDown,
                       boost::circular_buffer<Packet> *TransportDllDown,
                       boost::circular_buffer<unsigned char> *DllTransportUp,
                       boost::circular_buffer<unsigned char> *TransportApiUp)
{
}
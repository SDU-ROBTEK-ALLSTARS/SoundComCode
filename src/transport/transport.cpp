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
#include "transport.h"
#include "packet.h"

// Table of reserved ports
bool Transport::sPortsInUse_[256] = {false};

// Default constructor
Transport::Transport()
{
	port_ = 0;
}

// Destructor
Transport::~Transport()
{
	sPortsInUse_[port_] = false;
}

//void connect();

// assemblePacketFromCharBuffer()
//
// Returns a Packet object from chars contained in a circular buffer. It is
// supposed, that the buffer is operated in a "FIFO"-like way, so that the data
// gotten from buffer.front() is the data first inserted (with push_back()).
// Data is not deleted from the buffer until 
Packet Transport::packetFromCharBuffer(boost::circular_buffer<unsigned char> *buffer)
{
	Packet packet;

	if (buffer->size() >= HLEN) {
		//Get header off of buffer (HLEN long as defined in packet.h)
		unsigned char header[HLEN];
		for (unsigned char i=0; i<HLEN; i++) {
			header[i] = buffer->at(i);
		}

		//Examine the total packet length (4th header byte) to get any data it might contain
		unsigned char totalLength = header[3];
		unsigned char *data;

		if ((totalLength > HLEN) && ((unsigned char)buffer->size() > (totalLength-HLEN))) {
			unsigned char dataLength = header[3]-HLEN;
			data = new unsigned char[dataLength];
			for (unsigned char i=0; i<dataLength; i++) {
				data[i] = buffer->at(i+HLEN); //Data is off-set by HLEN
			}
		}
		else {
			throw "assemblePacketFromCharBuffer() Buffer does not contain a complete data array";
			return packet; //empty packet
		}

		//Both header and data was succesfully collected
		packet.makeFromArrays(header, data);
		buffer->erase_begin(totalLength);
		return packet;
	}
	else {
		throw "assemblePacketFromCharBuffer() Buffer does not contain a complete header";
		return packet; //empty packet
	}
}

// packetToCharBuffer
// Places a Packet object as a char array in a boost circular buffer. If there
// is not enough space in the buffer an exception is thrown.
void Transport::packetToCharBuffer(boost::circular_buffer<unsigned char> *buffer, Packet packet)
{
	unsigned char *packetAsArray = packet.getAsArray();
	int availableSpace = buffer->capacity() - buffer->size();
	if (availableSpace >= packet.totalLength()) {
		for (unsigned char i=0; i<packet.totalLength(); i++) {
			buffer->push_back(packetAsArray[i]);
		}
	}
	else {
		throw "packetToCharBuffer() not enough available space in buffer";
	}
}

void Transport::processUpboundPacket(Packet packet) //TO DO TO DO
{
	if ((port_ == packet.destPort()) && (packet.checksumValid())) {
		//lastInSequence_ = packet.seqNumber();
		if (packet.flagSet(SYN)) {
			int i=0;
		}
	}
}

// setPort()
// Gives a certain instance of the class a port number and reserves
// it, making it unavailable to other class instances.
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

// port()
// Returns this instances port number
unsigned char Transport::port() const
{
	return port_;
}

// isPortSet()
// True if port number has been set correctly. Can be used to 
// self-test.
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

// getPortTable()
// Returns a pointer to the array that contains reserved ports.
bool *Transport::getPortTable() const
{
	bool *pointer = sPortsInUse_;
	return pointer;
}

// decode()
// Main public up-stream packet processing method. This is called by the
// backbone when there is incoming data available to Transport from 
// the underlying layer.
void Transport::decode(boost::circular_buffer<unsigned char> *DllTransportUp,
                       boost::circular_buffer<unsigned char> *TransportApiUp)
{
	//Make packet from char array in buffer
	Packet packet = packetFromCharBuffer(DllTransportUp);

	if (packet.checksumValid()) {
		packetToCharBuffer(TransportApiUp, packet);
	}
	else {
		throw "assemblePacketFromCharBuffer() assembled packet checksum invalid";
	}
}

// encode()
// Main public down-stream packet processing method. This is called by the
// backbone when there is incoming data available to Transport from 
// the above layer.
void Transport::encode(boost::circular_buffer<unsigned char> *ApiTransportDown,
                       boost::circular_buffer<Packet> *TransportDllDown)
{
}
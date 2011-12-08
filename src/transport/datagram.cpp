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
# File:     datagram.cpp
# Project:  DtmfProject
# Version:  
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-06-12
********************************************************************************
# Description
#
# 
********************************************************************************/

#include <bitset>
#include <boost/crc.hpp>
#include "datagram.h"

Datagram::Datagram()
{ }

Datagram::~Datagram()
{ }

void Datagram::make(unsigned char sourcePort,
                    unsigned char destPort,
                    unsigned char seqNumber,
                    unsigned char ackNumber,
                    unsigned char data[],
					unsigned char dataLength,
                    char type[],
                    bool addChecksum)
	                
{
	std::bitset<8> flags;
	
	//Decide which type of package to make
	if (type == "data") {
		if (dataLength > 248) {
			throw "Datagram.make() Invalid data length";
			return;
		}
		sourcePort_ = sourcePort;
		destPort_ = destPort;
		seqNumber_ = seqNumber;
		ackNumber_ = ackNumber;
		length_ = dataLength + HLEN; //Header is always 8 bytes long
		data_ = data; //pointer address
		flags.reset();
		flags.set(ACK);

		if (addChecksum) {
			flags.set(CHK);
			flags_ = (unsigned char) flags.to_ulong(); //Set flags before calculating CRC
			
			boost::crc_ccitt_type crc;
			crc.process_bytes(&sourcePort_, (HLEN-2)); //Header minus the checksum field
			crc.process_bytes(data_, dataLength);
			checksum_ = crc.checksum();
		}
		else {
			flags.reset(CHK);
			flags_ = (unsigned char) flags.to_ulong();
			checksum_ = 0;
		}
	}
	else if (type == "sync") {
		// todo
	}
	else {
		throw "Datagram.make() Invalid datagram type";
		return;
	}
}

unsigned char Datagram::totalLength()
{
	return length_;
}

unsigned char Datagram::sourcePort()
{
	return sourcePort_;
}

unsigned char Datagram::destPort()
{
	return destPort_;
}

unsigned char Datagram::seqNumber()
{
	return seqNumber_;
}

unsigned char Datagram::ackNumber()
{
	return ackNumber_;
}

unsigned char Datagram::flags()
{
	return flags_;
}

bool Datagram::checksumValid()
{
	std::bitset<8> flags (flags_);
	if (flags.test(CHK)) {
		boost::crc_ccitt_type crc;
		crc.process_bytes(&sourcePort_, (HLEN-2)); //Header minus the checksum field
		crc.process_bytes(data_, (length_ - HLEN));
		if (checksum_ == crc.checksum()) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return true; //checksum was not calculated
	}
}
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

#include <cstdlib>
#include <ctime>
#include <bitset>
#include <boost/crc.hpp>
#include "datagram.h"

Datagram::Datagram()
{}

Datagram::~Datagram()
{}

unsigned short int Datagram::calcChecksum()
{
	boost::crc_ccitt_type crc;
	crc.process_bytes(&sourcePort_, (HLEN-2)); //Header minus the checksum field
	crc.process_bytes(data_, (length_ - HLEN)); //Data
	return crc.checksum();
}

void Datagram::make(char type[],
                    unsigned char destPort,
                    unsigned char sourcePort,
                    unsigned char ackNumber,
                    unsigned char seqNumber,
                    unsigned char data[],
                    unsigned char dataLength,
                    bool addChecksum)
{
	std::bitset<8> flags;
	
	//Decide which type of package to make
	if (type == "ack") {
		if ((dataLength > (256-HLEN)) || (!destPort) || (!sourcePort) || (!ackNumber) || (!seqNumber)) {
			throw "Datagram.make() Invalid args for type \"data\"";
			return;
		}
		sourcePort_ = sourcePort;
		destPort_ = destPort;
		seqNumber_ = seqNumber;
		ackNumber_ = ackNumber;
		length_ = dataLength + HLEN;
		data_ = data;

		flags.reset();
		flags.set(ACK);

		if (addChecksum) {
			flags.set(CHK);
			flags_ = (unsigned char) flags.to_ulong(); //Set flags before calculating CRC
			
			checksum_ = calcChecksum();
		}
		else {
			flags.reset(CHK);
			flags_ = (unsigned char) flags.to_ulong();
			checksum_ = 0;
		}
	}
	else if (type == "syn") {
		if ((!destPort) || (!sourcePort)) {
			throw "Datagram.make() Invalid args for type \"syn\"";
			return;
		}
		//Seed a random sequence number
		std::srand((unsigned)time(0));
		unsigned char randSeq = std::rand()%255;

		sourcePort_ = sourcePort;
		destPort_ = destPort;
		seqNumber_ = randSeq;
		ackNumber_ = 0;
		length_ = HLEN;
		data_ = 0;
		flags.reset();
		flags.set(SYN);
		flags_ = (unsigned char) flags.to_ulong();
		checksum_ = calcChecksum();
	}
	else if (type == "rst") {
		//
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
	if (flags.test(CHK) && (checksum_)) {
		short unsigned int newChecksum = calcChecksum();
		if (checksum_ == newChecksum) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false; //checksum does not exist at all
	}
}
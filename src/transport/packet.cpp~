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
# File:     packet.cpp
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
#include "packet.h"

// Default constructor
Packet::Packet()
{
	length_ = 0; //length is used to test if the packet is "made"; make sure it's 0 initially
}

// Default destructor
Packet::~Packet()
{}

// calcChecksum()
// Calculates the checksum based on the current data
// contained in "this" Packet.
unsigned short int Packet::calcChecksum() const
{
	boost::crc_ccitt_type crc;

	crc.process_byte(sourcePort_);
	crc.process_byte(destPort_);
	crc.process_byte(flags_);
	crc.process_byte(length_);
	crc.process_byte(seqNumber_);
	crc.process_byte(ackNumber_);
	crc.process_bytes(data_, (length_-HLEN));

	return crc.checksum();
}

// make()
// Contructs an instance of Packet with the given specifications
// A newly instantiated Package object is of no practical use 
// until it has been "made()"
// The type argument specifies some of the parameters of the
// packet automatically - flags are such a parameter.
void Packet::make(const char type[],
                  const unsigned char destPort,
                  const unsigned char sourcePort,
                  const unsigned char ackNumber,
                  const unsigned char seqNumber,
                  unsigned char data[],
                  const unsigned char dataLength,
                  bool addChecksum)
{
	std::bitset<8> flags;

	//Decide which type of package to make
	if (type == "ack") {
		if ((dataLength > (256-HLEN)) || (!destPort) || (!sourcePort) || (!ackNumber) || (!seqNumber)) {
			throw "Packet.make() Invalid args for type \"data\"";
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
			throw "Packet.make() Invalid args for type \"syn\"";
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
		flags.set(CHK);
		flags_ = (unsigned char) flags.to_ulong();
		checksum_ = calcChecksum();
	}
	else if (type == "rst") {
		//
	}
	else {
		throw "Packet.make() Invalid packet type";
		return;
	}
}

// makeFromArrays()
// As make() this function will construct a use-able packet.
// Here, no parameters are specified automatically: They are 
// all passed as an array of chars.
void Packet::makeFromArrays(unsigned char header[HLEN], unsigned char data[])
{
	sourcePort_ = header[0];
	destPort_ = header[1];
	flags_ = header[2];
	length_ = header[3];
	seqNumber_ = header[4];
	ackNumber_ = header[5];
	checksum_ = (header[6]<<8)|(header[7]);
	data_ = data; //may be just 0
}

// getAsPointerArray()
// Untested
unsigned char **Packet::getAsPointerArray()
{
	if (length_) {
		unsigned char **output = new unsigned char*[length_];
		output[0] = &sourcePort_;
		output[1] = &destPort_;
		output[2] = &flags_;
		output[3] = &length_;
		output[4] = &seqNumber_;
		output[5] = &ackNumber_;
		unsigned char c1 = (unsigned char)(checksum_>>8);
		unsigned char c2 = (unsigned char)(checksum_);
		output[6] = &c1;
		output[7] = &c2;
		if (length_ > HLEN) {
			output[8] = data_;
		}
		return output;
	}
	else {
		return 0;
	}
}

// getAsArray()
// Outputs the instance of Packet as (pointer to) a char array.
unsigned char *Packet::getAsArray()
{
	if (length_) {
		unsigned char *output = new unsigned char[length_];
		output[0] = sourcePort_;
		output[1] = destPort_;
		output[2] = flags_;
		output[3] = length_;
		output[4] = seqNumber_;
		output[5] = ackNumber_;
		output[6] = (unsigned char)(checksum_>>8);
		output[7] = (unsigned char)(checksum_);
		if (length_ > HLEN) {
			for (int i=0; i<(length_-HLEN); i++) {
				output[i+HLEN] = data_[i];
			}
		}
		return output;
	}
	else {
		return 0;
	}
}

// isMade()
// Tests the packets total length to learn if it has
// been "made" properly. Return true if it has.
bool Packet::isMade() const
{
	if (length_)
		return true;
	else
		return false;
}

// totalLength()
// Return this packets total length
unsigned char Packet::totalLength() const
{
	return length_;
}

// sourcePort()
// Return this packets source port
unsigned char Packet::sourcePort() const
{
	return sourcePort_;
}

// destPort()
// Return this packets destination port
unsigned char Packet::destPort() const
{
	return destPort_;
}

// seqNumber()
// Return this packets sequence number
unsigned char Packet::seqNumber() const
{
	return seqNumber_;
}

// ackNumber()
// Return this packets acknowledgment number
unsigned char Packet::ackNumber() const
{
	return ackNumber_;
}

// flagSet()
// Tests if a specific flag is set. Returns true if
// it is set (the bit is 1)
bool Packet::flagSet(const int test) const
{
	std::bitset<8> flags (flags_);
	
	if (flags.test(test))
		return true;
	else
		return false;
}

// flags()
// Return this packets flags as a char (1 byte)
unsigned char Packet::flags() const
{
	return flags_;
}

// data()
// If this packet contains data, it is returned
// as (a pointer to) a char array
unsigned char *Packet::data() const
{
	if (length_ > HLEN)
		return data_;
	else 
		return 0;
}

// dataLength()
// Returns the length of the packets data length.
// Returns 0 if there is no data.
unsigned char Packet::dataLength() const
{
	if (length_ > HLEN)
		return (length_-HLEN);
	else 
		return 0;
}

// checksumValid()
// Returns true if this packets checksum
// is valid. False otherwise.
bool Packet::checksumValid()
{
	std::bitset<8> flags (flags_);
	if (flags.test(CHK) && (checksum_)) {
		short unsigned int newChecksum = calcChecksum();
		if (checksum_ == newChecksum)
			return true;
		else
			return false;
	}
	else {
		return false; //checksum does not exist at all
	}
}
/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011 Alexander Adelholm Brandbyge <abran09@student.sdu.dk>
# Frederik Hagelskjær <frhag10@student.sdu.dk>
# Kent Stark Olsen <keols09@student.sdu.dk>
# Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Leon Bonde Larsen <lelar09@student.sdu.dk>
# Rudi Hansen <rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File: packet.cpp
# Project: DtmfProject
# Version:
# Platform: PC/Mac/Linux
# Author: Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Created: 2011-06-12
********************************************************************************
# Description
#
#
********************************************************************************/

#include <cstdlib>
#include <ctime>
#include <bitset>
#include <boost/crc.hpp>
#include "../common/exception.h"
#include "packet.h"
#include "DtmfOutMessage.h"

// Default constructor
Packet::Packet()
{
length_ = 0; //length is used to test if the packet is "made"; make sure it's 0 initially
}

// Destructor
Packet::~Packet() {}

// calcChecksum()
// Calculates the 16-bit checksum based on the
// current data contained in this packet:
// If the CHK flag is set, data is also included
// in the checksum calculation. If CHK is NOT
// set, checksum is only calculated for the header.
unsigned short int Packet::calcChecksum() const
{
boost::crc_ccitt_type crc;
crc.process_byte(sourcePort_);
crc.process_byte(destPort_);
crc.process_byte(flags_);
crc.process_byte(length_);
crc.process_byte(seqNumber_);
crc.process_byte(ackNumber_);
if ((flagSet(PACKET_FLAG_CHK)) && (data_)) {
crc.process_bytes(data_, (length_-PACKET_HLEN));
}
return crc.checksum();
}

// make()
// Contructs an instance of Packet with the given specifications
// A newly instantiated Package object is of no practical use
// until it has been "made()"
void Packet::make(const unsigned char sourcePort,
                  const unsigned char destPort,
                  const bool flagArray[8],
                  const unsigned char seqNumber,
                  const unsigned char ackNumber,
                  const unsigned char dataLength,
                  unsigned char data[])
{
//Flag array to bitset for easier manipulation
std::bitset<8> flags;
for (int i=0; i<8; i++) {
flags[i] = flagArray[i];
}

//Source port
sourcePort_ = sourcePort;

//Destination port
destPort_ = destPort;

//Flags
flags_ = (unsigned char) flags.to_ulong();

//Length
if (data)
length_ = dataLength + PACKET_HLEN;
else
length_ = PACKET_HLEN;

//Sequence number
if (flags.test(PACKET_FLAG_SYN)) {
std::srand((unsigned)time(0));
unsigned char randSeq = std::rand()%255;
seqNumber_ = randSeq;
}
else {
seqNumber_ = seqNumber;
}

//Aknowledgment number
ackNumber_ = ackNumber;

//Data (defaults to 0 if there isn't any)
data_ = data;

//Checksum (always calculate last, after all other variables are set)
checksum_ = calcChecksum();
}

// setRecvAddr()
//
void Packet::setRecvAddr(const unsigned char recvAddr)
{
if (recvAddr) {
recvAddr_ = recvAddr;
checksum_ = calcChecksum();
}
}

// destPort()
//
void Packet::setDestPort(const unsigned char destPort)
{
if (destPort) {
destPort_ = destPort;
checksum_ = calcChecksum();
}
}

// insertData()
// This method will insert data into an already instantiated
// packet. It will not check if the rest of the packet is
// correctly set up.
// Argument is a pointer to the char array to be inserted,
// and the length of the data array.
// Packet checksum will be re-calculated
void Packet::insertData(unsigned char data[], const unsigned char dataLength, const bool checksum)
{
if (dataLength > (PACKET_TLEN-PACKET_HLEN)) {
throw Exception("insertData() Incorrect data array length");
return;
}
else {
data_ = data;
length_ = dataLength + PACKET_HLEN;
if (checksum) {
checksum_ = calcChecksum();
}
}
}

// setFlag()
// Sets the flag supplied through the argument. If, for example,
// we have 0b00001000 and setFlag(2) is called, the result will
// be 0b00001100. Checksum is re-calculated.
void Packet::setFlag(const int flagToSet)
{
if (flagToSet > 7) {
throw Exception("setFlag() Incorrect flag bit");
return;
}
else {
std::bitset<8> flags (flags_);
flags.set(flagToSet);
checksum_ = calcChecksum();
}
}

//
bool Packet::orderBySeq::operator()(const Packet& p1, const Packet& p2) const
{
return p1.seqNumber_ > p2.seqNumber_;
}

// makeFromArrays()
// As make() this function will construct a use-able packet.
// Here, no parameters are specified automatically: They are
// all passed as an array of chars.
void Packet::makeFromArrays(unsigned char header[PACKET_HLEN], unsigned char data[])
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

// getAsArray()
// Outputs the instance of Packet as (pointer to) a char array.
unsigned char *Packet::getAsArray() const
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
if (length_ > PACKET_HLEN) {
for (int i=0; i<(length_-PACKET_HLEN); i++) {
output[i+PACKET_HLEN] = data_[i];
}
}
return output;
}
else {
return 0;
}
}

// Packet[i] operator
// Returns the i'th byte as if Packet was a byte array
unsigned char Packet::operator[](const unsigned char i) const
{
if (i >= length_)
return 0;
if ((i >= 0) && (i < PACKET_HLEN)) {
//We want a header byte
switch (i) {
case 0:
return sourcePort_;
case 1:
return destPort_;
case 2:
return flags_;
case 3:
return length_;
case 4:
return seqNumber_;
case 5:
return ackNumber_;
case 6:
return (unsigned char)(checksum_>>8);
case 7:
return (unsigned char)(checksum_);
default:
return 0;
}
}
else if ((i >= PACKET_HLEN) && (i < (PACKET_TLEN - PACKET_HLEN))) {
return data_[i-PACKET_HLEN];
}
else {
return 0;
}
}

// recvAddr()
// Returns the receiver address for the data included
// in this packet
unsigned char Packet::recvAddr() const
{
return recvAddr_;
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
if (length_ > PACKET_HLEN)
return data_;
else
return 0;
}

// dataLength()
// Returns the length of the packets data length.
// Returns 0 if there is no data.
unsigned char Packet::dataLength() const
{
if (length_ > PACKET_HLEN)
return (length_-PACKET_HLEN);
else
return 0;
}

// checksumValid()
// Returns true if this packets checksum
// is valid. False otherwise.
bool Packet::checksumValid() const
{
if ((length_) && (checksum_ == calcChecksum()))
return true;
else
return false;
}


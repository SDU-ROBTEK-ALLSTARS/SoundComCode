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
# File: packet.h
# Project: DtmfProject
# Version:
# Platform: PC/Mac/Linux
# Author: Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Created: 2011-06-12
********************************************************************************
# Description
#
********************************************************************************/
#ifndef PACKET_H
#define PACKET_H

//Flag bit positions
#define PACKET_FLAG_SYN 0
#define PACKET_FLAG_ACK 1
#define PACKET_FLAG_NUL 2
#define PACKET_FLAG_RST 3
#define PACKET_FLAG_EAK 4
#define PACKET_FLAG_CHK 7

#define PACKET_HLEN 8 //Header length (in bytes)
#define PACKET_TLEN 256 //Packet total length
                        //Warning: Max=256 atm.

class Packet
{
private:
unsigned char recvAddr_; //Receiver address to pass to the data link layer
unsigned char sourcePort_;
unsigned char destPort_;
unsigned char flags_;
unsigned char length_;
unsigned char seqNumber_;
unsigned char ackNumber_;
unsigned short int checksum_;
unsigned char *data_;

unsigned short int calcChecksum() const;

public:
Packet();
~Packet();

//make() will be our "lazy constructor"
void make(const unsigned char sourcePort,
const unsigned char destPort,
const bool flags[8],
const unsigned char seqNumber,
const unsigned char ackNumber,
const unsigned char dataLength=0,
unsigned char data[]=0);

// Make from char arrays
void makeFromArrays(unsigned char header[PACKET_HLEN], unsigned char data[]);

// Single-element insertion
void setRecvAddr(const unsigned char recvAddr);
void setDestPort(const unsigned char destPort);
void insertData(unsigned char data[], const unsigned char dataLength, const bool checksum=true);
void setFlag(const int);

// Packet sorting by sequence number (intended for use with std::priority_queue)
struct orderBySeq
{
bool operator()(const Packet&, const Packet&) const;
};

// Get a packet as a (pointer to) char array
unsigned char *getAsArray() const; // somewhat memory-inefficient (and propably not needed for normal use)

//Get one byte from packet as if it was a char array
unsigned char operator[](const unsigned char) const;

// Accessor methods
unsigned char recvAddr() const;
unsigned char totalLength() const;
unsigned char sourcePort() const;
unsigned char destPort() const;
unsigned char seqNumber() const;
unsigned char ackNumber() const;
bool flagSet(const int) const;
unsigned char flags() const;
unsigned char *data() const;
unsigned char dataLength() const;
bool checksumValid() const;
};
#endif //PACKET_H


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
# File:     packet.h
# Project:  DtmfProject
# Version:  
# Platform:	PC/Mac/Linux
# Author:  	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-06-12
********************************************************************************
# Description
#
# 
# 256 byte max overall length of each packet
#
********************************************************************************/
#ifndef PACKET_H
#define PACKET_H

//Flag bit positions
#define SYN 0
#define ACK 1
#define CHK 2
#define RST 3
#define FIN 4

#define HLEN 8 //Header length (in bytes)

class Packet
{
private:
	unsigned char sourcePort_;
	unsigned char destPort_;
	unsigned char flags_;
	unsigned char length_;
	unsigned char seqNumber_;
	unsigned char ackNumber_;
	unsigned short int checksum_;
	unsigned char *data_;
	
	unsigned short int calcChecksum();

public:
	Packet();
	~Packet();

	//make() will be our "lazy" constructor
	void make(const char type[]="ack", //Options are: ack, syn, rst
	          const unsigned char destPort=0,
	          const unsigned char sourcePort=0,
	          const unsigned char ackNumber=0,
	          const unsigned char seqNumber=0,
	          unsigned char data[]=0,
	          const unsigned char dataLength=0,
	          bool addChecksum=true);

	unsigned char totalLength() const;
	unsigned char sourcePort() const;
	unsigned char destPort() const;
	unsigned char seqNumber() const;
	unsigned char ackNumber() const;
	unsigned char flags() const;
	bool checksumValid();
};
#endif //PACKET_H
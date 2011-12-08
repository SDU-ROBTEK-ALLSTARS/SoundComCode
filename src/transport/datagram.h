/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskj�r				<frhag10@student.sdu.dk>
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
# File:     datagram.h
# Project:  DtmfProject
# Version:  
# Platform:	PC/Mac/Linux
# Author:  	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-06-12
********************************************************************************
# Description
#
# 
# 256 byte max overall length of each package
#
********************************************************************************/
#ifndef DATAGRAM_H
#define DATAGRAM_H

//Flag bit positions
#define SYN 0
#define ACK 1
#define CHK 2
#define RST 3
#define FIN 4

#define HLEN 8 //Header length (in bytes)

class Datagram
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
	Datagram();
	~Datagram();

	//make() will be our "lazy" constructor
	void make(char type[]="ack", //Options are: ack, syn, rst
	          unsigned char destPort=0,
	          unsigned char sourcePort=0,
	          unsigned char ackNumber=0,
	          unsigned char seqNumber=0,
	          unsigned char data[]=0,
	          unsigned char dataLength=0,
	          bool addChecksum=true);

	unsigned char totalLength();
	unsigned char sourcePort();
	unsigned char destPort();
	unsigned char seqNumber();
	unsigned char ackNumber();
	unsigned char flags();
	bool checksumValid();
};
#endif //DATAGRAM_H
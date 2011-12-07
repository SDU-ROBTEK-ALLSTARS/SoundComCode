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
# File:     datagram.h
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Author:  	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-06-12
********************************************************************************
# Description
#
# 
********************************************************************************/
#ifndef DATAGRAM_H
#define DATAGRAM_H

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

public:
	Datagram();
	Datagram(unsigned char sourcePort,
	         unsigned char destPort,
			 unsigned char *data);

	unsigned char getLength();
};
#endif //DATAGRAM_H
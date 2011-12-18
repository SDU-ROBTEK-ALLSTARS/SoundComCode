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
# File:     frame.h
# Project:  DtmfProject
# Version:  2.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-10-30
********************************************************************************
# Description
#
# The purpose of this class is to implement the concept of a frame
#                           (Denne header er dikteret af den onde diktator Kent)
*******************************************************************************/

#ifndef FRAME_H_
#define FRAME_H_

//*****     DEFINES     *****
//debugging
//#define		DEBUG
#define 	DEBUG_OUT			std::cout

//frame definitions
#define		TYPE				6
#define 	ADDRESS				4
#define		SEQUENCE			1
#define 	EOT					0

//enum {NONE,HEADER};

//*****     INCLUDES     *****
#include 	<fstream>

//*****     CLASS DECLARATION     *****
class Frame
{
private:
public:
	unsigned char byte2;			//header byte
	unsigned char byte1;			//data byte
	unsigned char byte0;			//parity byte

	Frame(void);										//default constructor for memory allocation
	Frame(unsigned char,unsigned char);					//constructs frame from header and data bytes
	Frame(unsigned char,unsigned char,unsigned char);	//constructs frame from header, data and parity bytes
	Frame(unsigned char,unsigned char,unsigned char,
			unsigned char,unsigned char,unsigned char);	//constructs frame from six nibbles

	int type;						//range 0:3 shifted 6
	int receiver;					//range 0:3 shifted 4
	int sequence;					//range 0:7 shifted 1
	int end;						//range 0:1 shifted 0
	int data;						//byte1
	int parity;						//byte0

	unsigned int makeParity(unsigned int,unsigned int);		//returns parity byte from header and data bytes
	int checkParity();										//checks parity of current frame, returns 0 for OK
	unsigned int getNibble(unsigned int,unsigned int);		//returns nibble (high/low , byte)

	friend std::ostream& operator<<(std::ostream& out, Frame& frm);  //for file output

#ifdef DEBUG
	void coutHeader();				//outputs header data
	friend std::ostream& printHeader(std::ostream& out);
#endif
};
#endif /* FRAME_H_ */

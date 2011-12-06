//============================================================================
// Name        : dataLinkLayer.cpp
// Author      : Leon Bonde Larsen
// Version     : 1.0 (updated 27/11-2011 by Leon Larsen)
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
//=====     INCLUDES     =====
#include 	"frame.h"
#include 	<stdio.h>
#include 	<iostream>

//=====     CLASS IMPLEMENTATION     =====
Frame::Frame(unsigned char b2,unsigned char b1)
{
	byte2 = b2;
	byte1 = data = b1;

	//extract header data
	type = (byte2 >> TYPE);
	receiver = (byte2 >> ADDRESS)&3;
	sequence = (byte2 >>SEQUENCE)&7;
	end = (byte2)&1;

	//generate parity byte
	byte0 = makeParity(byte2,byte1);

	#ifdef DEBUG
	DEBUG_OUT << "constructing frame...";
	coutHeader();
	#endif
}
//=====
Frame::Frame(unsigned char b2,unsigned char b1,unsigned char b0)
{
	byte2 = b2;
	byte1 = data = b1;
	byte0 = parity = b0;

	//extract header data
	type = (byte2 >> 6);
	receiver = (byte2 >> 4)&3;
	sequence = (byte2>>1)&7;
	end = (byte2)&1;

	#ifdef DEBUG
	DEBUG_OUT << "constructing frame...";
	coutHeader();
	#endif
}
//=====
unsigned int Frame::makeParity(unsigned int head, unsigned int dat)
{
	unsigned int par = 0;
	bool nibbles[4][4];
	bool parityByte[8];

	//put in four by four matrix
	nibbles[0][0] = head & (1<<7);
	nibbles[0][1] = head & (1<<6);
	nibbles[0][2] = head & (1<<5);
	nibbles[0][3] = head & (1<<4);
	nibbles[1][0] = head & (1<<3);
	nibbles[1][1] = head & (1<<2);
	nibbles[1][2] = head & (1<<1);
	nibbles[1][3] = head & (1<<0);
	nibbles[2][0] = dat & (1<<7);
	nibbles[2][1] = dat & (1<<6);
	nibbles[2][2] = dat & (1<<5);
	nibbles[2][3] = dat & (1<<4);
	nibbles[3][0] = dat & (1<<3);
	nibbles[3][1] = dat & (1<<2);
	nibbles[3][2] = dat & (1<<1);
	nibbles[3][3] = dat & (1<<0);

	//calculate parity bits
	parityByte[0] = nibbles[0][0]^nibbles[0][1]^nibbles[0][2]^nibbles[0][3];
	parityByte[1] = nibbles[1][0]^nibbles[1][1]^nibbles[1][2]^nibbles[1][3];
	parityByte[2] = nibbles[2][0]^nibbles[2][1]^nibbles[2][2]^nibbles[2][3];
	parityByte[3] = nibbles[3][0]^nibbles[3][1]^nibbles[3][2]^nibbles[3][3];
	parityByte[4] = nibbles[0][0]^nibbles[1][0]^nibbles[2][0]^nibbles[3][0];
	parityByte[5] = nibbles[0][1]^nibbles[1][1]^nibbles[2][1]^nibbles[3][1];
	parityByte[6] = nibbles[0][2]^nibbles[1][2]^nibbles[2][2]^nibbles[3][2];
	parityByte[7] = nibbles[0][3]^nibbles[1][3]^nibbles[2][3]^nibbles[3][3];

	//assemble parity byte and store in byte 0
	par = (unsigned char)(parityByte[0]<<7)|(parityByte[1]<<6)|(parityByte[2]<<5)|(parityByte[3]<<4)|
			(parityByte[4]<<3)|(parityByte[5]<<2)|(parityByte[6]<<1)|(parityByte[7]);

	return par;
}
//=====
int Frame::checkParity()
{
	#ifdef DEBUG
		DEBUG_OUT << "Parity check: ";
		if(byte0==makeParity(byte2,byte1))
			DEBUG_OUT << " - OK" << " - ";
		else
			DEBUG_OUT << " - FAILED" << " - ";
	#endif

	//compare actual parity with calculated parity, return 0 for OK, 1 for not OK
	return byte0==makeParity(byte2,byte1)?0:1;
}
//=====
unsigned int Frame::getNibble(unsigned int nib,unsigned int byt)
{
	int byte = byt==0?byte0:byt==1?byte1:byt==2?byte2:0;
	if(nib == 1)
		return (byte>>4);
	else if(nib == 0)
		return byte & 15;
	else return -1;
}
//=====
std::ostream& operator << (std::ostream& out, Frame& frm)
{
	//output bytes
	out << (int)frm.byte2 << " ";
	out << (int)frm.byte1 << " ";
	out << (int)frm.byte0 << std::endl;
	return out;
}
//=====     DEBUG FUNCTIONS     =====
#ifdef DEBUG
void Frame::coutHeader()
{
	//output contents of the frame in easy to read style
	DEBUG_OUT << "Type: " << type << " ";
	DEBUG_OUT << "Receiver: " << receiver << " ";
	DEBUG_OUT << "Sequence: " << sequence << " ";
	if(end == 1)
		DEBUG_OUT << "- End of transmission" << std::endl;
	else
		DEBUG_OUT << std::endl;
	DEBUG_OUT << "Data contents: ";
	for(int i=7;i>=0;i--)
		DEBUG_OUT << (bool)(byte1 & (1<<i));
	DEBUG_OUT << "(" << (int)byte1 << ")" << " " << "parity: ";
	for(int i=7;i>=0;i--)
		DEBUG_OUT << (bool)(byte0 & (1<<i));
	DEBUG_OUT  << "(" << (int)byte0 << ")" << std::endl;
}
#endif


//============================================================================
// Name        : soundLibCode.cpp
// Author      : Leon Bonde Larsen
// Version     :
// Copyright   : Open Source
// Description : Implementing frame in C++, Ansi-style
//============================================================================
#ifndef FRAME_H_
#define FRAME_H_
//debugging
#define		DEBUG
#define 	DEBUG_OUT			std::cout

//frame definitions
#define		TYPE				6
#define 	ADDRESS				4
#define		SEQUENCE			1
#define 	EOT					0

enum {NONE,HEADER};

//=====     INCLUDES     =====
#include 	<fstream>

//=====     CLASS DECLARATION     =====
class Frame
{
private:
public:
	unsigned char byte2;			//header byte
	unsigned char byte1;			//data byte
	unsigned char byte0;			//parity byte

	Frame(unsigned char,unsigned char);
	Frame(unsigned char,unsigned char,unsigned char);

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

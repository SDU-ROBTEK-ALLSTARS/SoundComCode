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
//=====     INCLUDES     =====
#include 	<fstream>

//=====     CLASS DECLARATION     =====
class Frame
{
private:
	unsigned char byte2;			//header byte
	unsigned char byte1;			//data byte
	unsigned char byte0;			//parity byte
public:
	Frame(unsigned char,unsigned char);
	Frame(unsigned char,unsigned char,unsigned char);

	int type;						//range 0:3 shifted 6
	int receiver;					//range 0:3 shifted 4
	int sequence;					//range 0:7 shifted 1
	int end;						//range 0:1 shifted 0
	int data;						//byte1
	int parity;						//byte0

	void makeParity();				//generates parity byte from header and data bytes
	int checkParity();				//checks parity of current frame, returns 0 for OK
	friend std::ostream& operator<<(std::ostream& out, Frame& frm);  //for file output

#ifdef DEBUG
	void coutBytes();				//outputs byte0
	void coutHeader();				//outputs header data
#endif
};
#endif /* FRAME_H_ */

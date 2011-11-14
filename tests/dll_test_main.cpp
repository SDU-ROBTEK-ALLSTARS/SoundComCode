//============================================================================
// Name        : soundLibCode.cpp
// Author      : Leon Bonde Larsen
// Version     :
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
#define		DEBUG
#define 	DEBUG_OUT		std::cout
#define 	MY_ADDRESS		7
#include 	<iostream>
#include 	"dataLinkLayer.h"

//=====     MAIN ENTRY POINT     =====
int main()
{
	//Construct data link layer
	DataLinkLayer dll;

	//Construct frames
	Frame input1(248,200);
	Frame input2(249,128);
	Frame input3(250,75);
	Frame input4(251,255);

	//Decode four frames and check list
	dll.decode(input1);
	dll.decode(input2);
	dll.decode(input3);
	dll.decode(input4);
	dll.checkList();
}

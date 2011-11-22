//============================================================================
// Name        : soundLibCode.cpp
// Author      : Leon Bonde Larsen
// Version     :
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
#define		DEBUG
#define 	DEBUG_OUT			std::cout
#define 	MY_ADDRESS			7
//#define		STARTS_WITH_TOKEN
#define		MAX_TIME_WITH_TOKEN	5
#include 	<iostream>
#include 	"dataLinkLayer.h"

//=====     MAIN ENTRY POINT     =====
int main()
{
	//Construct data link layer
	DataLinkLayer dll;

	//Construct 8 frames
	Frame input1(120,0);
	Frame input2(184,0);

	//Decode four frames and check list
	dll.processFrame(input1);

	char input;
	while(1)
	{
	std::cin >> input;
	if(input=='a')
		dll.checkToken();
	if(input=='b')
		dll.processFrame(input2);
	}
}

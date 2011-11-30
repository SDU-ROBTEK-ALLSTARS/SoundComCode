//============================================================================
// Name        : soundLibCode.cpp
// Author      : Leon Bonde Larsen
// Version     :
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
#define		DEBUG
#define 	DEBUG_OUT			std::cout
#define 	MY_ADDRESS			0
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
	Frame input1(192,1);
	Frame input2(194,2);
	Frame input3(196,3);
	Frame input4(198,4);
	Frame input5(200,5);
	Frame input6(202,6);
	Frame input7(204,7);
	Frame input8(207,8);

	//Decode four frames and check list

//	dll.processFrame(input5);
//	dll.processFrame(input6);

//	char input;
//	while(1)
//	{
//	std::cin >> input;
//	if(input=='a')
//		dll.checkToken();
//	if(input=='b')
//		dll.processFrame(input2);
//	}
}

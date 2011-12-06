//============================================================================
// Name        : testmain.cpp
// Author      : Frederik Hagelsk¾r
// Version     : 1.1 (updated 27/11-2011 by Leon Larsen)
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
//=====     DEFINES     =====
//buffer i/o
#define 	BUFFERSIZE 		100
#define 	INPUTUP 		"inputRight.dat"
#define 	INPUTDOWN 		"inputLeft.dat"
#define 	OUTPUTUP 		"outputRight.dat"
#define 	OUTPUTDOWN 		"outputLeft.dat"

//=====     INCLUDES     =====
//general
#include 	<iostream>
#include 	<fstream>
#include 	<string>
#include 	<boost/circular_buffer.hpp>


//data link layer
#include 	"dataLinkLayer.h"
#include	"frame.h"

int main()
{
	//variables
	unsigned int byte1,byte2,byte3;
	char waste[256];

	//i/o streams
	std::ifstream inputright;
	std::ifstream inputleft;
	std::ofstream outputright;
	std::ofstream outputleft;

	//open files
	inputright.open(INPUTUP);
	inputleft.open(INPUTDOWN);
	if(inputright.fail() || inputleft.fail())
	{
		DEBUG_OUT << "Error opening input file";
		exit(1);
	}
	outputright.open(OUTPUTUP);
	outputleft.open(OUTPUTDOWN);
	
	//instantiate buffers
	boost::circular_buffer< unsigned int > inputRight(BUFFERSIZE);   //downwards input Right
	boost::circular_buffer< unsigned int > inputLeft(BUFFERSIZE);   //downwards input Left

	boost::circular_buffer< unsigned int > outputRight(BUFFERSIZE);   //upwards output Right
	boost::circular_buffer< unsigned int > outputLeft(BUFFERSIZE);   //upwards output Left


	boost::circular_buffer< Frame > movingRightDll(BUFFERSIZE);   //dll communication Right
	boost::circular_buffer< Frame > movingLeftDll(BUFFERSIZE);   //dll communication Left

#ifdef DEBUG
DEBUG_OUT << "----------   ### INITIALIZING BUFFERS FROM FILES ###   ----------" << std::endl;
#endif

	//fill input buffers
	while( !inputleft.eof() )
	{
		inputleft >> byte1;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering datagram byte from file...";
		for(int i=7;i>=0;i--)
			DEBUG_OUT << (bool)(byte1 & (1<<i));
		DEBUG_OUT << std::endl;
		#endif
		if(!(byte1 == 255))
		{inputRight.push_back(byte1);}
		else
		{inputright.getline(waste,300);}
	}

	while( !inputright.eof() )
	{
		inputright >> byte1;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering datagram byte from file...";
		for(int i=7;i>=0;i--)
			DEBUG_OUT << (bool)(byte1 & (1<<i));
		DEBUG_OUT << std::endl;
		#endif
		if(!(byte1 == 255))
		{inputLeft.push_back(byte1);}
		else
		{inputleft.getline(waste,300);}
	}

	//instantiate data link layer
	DataLinkLayer Leftdll;
	DataLinkLayer Rightdll;
	//call encode or decode with arguments &dscbi, &dscbo, &uscbi, &uscbo

	Leftdll.encode(&inputLeft, &movingRightDll, &movingLeftDll, &outputLeft);
	
	
	Rightdll.encode(&inputRight, &movingLeftDll, &movingRightDll, &outputRight);

	Leftdll.decode(&inputLeft, &movingRightDll, &movingLeftDll, &outputLeft);

	Rightdll.decode(&inputRight, &movingLeftDll, &movingRightDll, &outputRight);

#ifdef DEBUG
DEBUG_OUT << std::endl << "----------   ### WRITING BUFFERS TO FILES ###   ----------" << std::endl;
#endif

	//write output buffers to files. Results in "Assertion failed:" if buffer is empty, but executes anyway
	while(true)
		{
			if(outputLeft.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing datagram..." << outputLeft.front() << std::endl;
				#endif
				outputleft << (int)outputLeft.front() << std::endl;
				outputLeft.pop_front();
			}
		}

	while(true)
		{
			if(outputRight.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing datagram..." << outputRight.front() << std::endl;
				#endif
				outputright << (int)outputRight.front() << std::endl;
				outputRight.pop_front();
			}
		}
		system("pause");
	return 0;
}

//End Of File

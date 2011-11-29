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
#define 	INPUTUP 		"inputup.dat"
#define 	INPUTDOWN 		"inputdown.dat"
#define 	OUTPUTUP 		"outputup.dat"
#define 	OUTPUTDOWN 		"outputdown.dat"

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

	//i/o streams
	std::ifstream inputup;
	std::ifstream inputdown;
	std::ofstream outputup;
	std::ofstream outputdown;

	//open files
	inputup.open(INPUTUP);
	inputdown.open(INPUTDOWN);
	if(inputup.fail() || inputdown.fail())
	{
		DEBUG_OUT << "Error opening input file";
		exit(1);
	}
	outputup.open(OUTPUTUP);
	outputdown.open(OUTPUTDOWN);
	
	//instantiate buffers
	boost::circular_buffer< unsigned int > dscbi(BUFFERSIZE);   //downwards input
	boost::circular_buffer< Frame > dscbo(BUFFERSIZE);   //downwards output
	boost::circular_buffer< Frame > uscbi(BUFFERSIZE);   //upwards input
	boost::circular_buffer< unsigned int > uscbo(BUFFERSIZE);   //upwards output

	//fill input buffers
	while( !inputup.eof() )
	{
		inputup >> byte1 >> byte2 >> byte3;
		uscbi.push_back(Frame(byte1,byte2,byte3));
		#ifdef DEBUG
		DEBUG_OUT << "Buffering from file..." << byte1 << " " << byte2 << " " << byte3 << std::endl;
		#endif
	}	
	while( !inputdown.eof() )
	{
		inputdown >> byte1;
		dscbi.push_back(byte1);
		#ifdef DEBUG
		DEBUG_OUT << "Buffering from file..." << byte1 << std::endl;
		#endif
	}

	//instantiate data link layer
	DataLinkLayer dll;

	//call encode or decode with arguments &dscbi, &dscbo, &uscbi, &uscbo
	dll.decode(&dscbi, &dscbo, &uscbi, &uscbo);

	//write output buffers to files. Results in "Assertion failed:" if buffer is empty, but executes anyway
	while(true)
		{
			outputup << (int)uscbo.front() << std::endl;
			uscbo.pop_front();
			if(uscbo.empty())
			{break;}

		}

		while(true)
		{
			outputdown << dscbo.front();
			dscbo.pop_front();
			if(dscbo.empty())
			{break;}

		}
	return 0;
}

//End Of File

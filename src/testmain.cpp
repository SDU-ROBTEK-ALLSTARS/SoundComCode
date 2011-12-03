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

#ifdef DEBUG
DEBUG_OUT << "----------   ### INITIALIZING BUFFERS FROM FILES ###   ----------" << std::endl;
#endif

	//fill input buffers
	while( !inputup.eof() )
	{
		inputup >> byte1 >> byte2 >> byte3;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering frame from file..." << byte1 << " " << byte2 << " " << byte3 << std::endl;
		#endif
		uscbi.push_back(Frame(byte1,byte2,byte3));
	}	
	while( !inputdown.eof() )
	{
		inputdown >> byte1;
		#ifdef DEBUG
		DEBUG_OUT << "Buffering datagram byte from file...";
		for(int i=7;i>=0;i--)
			DEBUG_OUT << (bool)(byte1 & (1<<i));
		DEBUG_OUT << std::endl;
		#endif

		dscbi.push_back(byte1);
	}

	//instantiate data link layer
	DataLinkLayer dll;

	//call encode or decode with arguments &dscbi, &dscbo, &uscbi, &uscbo
	dll.encode(&dscbi, &dscbo, &uscbi, &uscbo);
	dll.decode(&dscbi, &dscbo, &uscbi, &uscbo);
//	dll.encode(&dscbi, &dscbo, &uscbi, &uscbo);
//	dll.decode(&dscbi, &dscbo, &uscbi, &uscbo);

#ifdef DEBUG
DEBUG_OUT << std::endl << "----------   ### WRITING BUFFERS TO FILES ###   ----------" << std::endl;
#endif

	//write output buffers to files. Results in "Assertion failed:" if buffer is empty, but executes anyway
	while(true)
		{
			if(uscbo.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing datagram..." << uscbo.front() << std::endl;
				#endif
				outputup << (int)uscbo.front() << std::endl;
				uscbo.pop_front();
			}
		}

		while(true)
		{
			if(dscbo.empty())
			{
				break;
			}
			else
			{
				#ifdef DEBUG
				DEBUG_OUT << "Writing frame..." << dscbo.front() << std::endl;
				#endif
				outputdown << dscbo.front();
				dscbo.pop_front();
			}
		}
	return 0;
}

//End Of File

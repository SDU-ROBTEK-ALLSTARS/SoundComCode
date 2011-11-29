//============================================================================
// Name        : dataLinkLayer.h
// Author      : Leon Bonde Larsen
// Version     : 1.0 (updated 27/11-2011 by Leon Larsen)
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
#ifndef DATALINKLAYER_H_
#define DATALINKLAYER_H_

//defines needed for data link layer
#define 	MY_ADDRESS			0
//#define		STARTS_WITH_TOKEN
#define		MAX_TIME_WITH_TOKEN	5
//debugging
#define		DEBUG								//out comment for no debug info
#define 	DEBUG_OUT			std::cout		//output for debug info

//=====     INCLUDES     =====
#include 	<stdio.h>
#include 	<time.h>
#include 	<boost/circular_buffer.hpp>
#include 	"frame.h"

//=====     DECLARATIONS     =====
struct datalist									//structure for unfinished transmissions
{
	unsigned char dataByte;
	bool receivedFlag;
};

//=====     CLASS DECLARATION     =====
class DataLinkLayer
{
private:
	struct datalist frameRecList[8];			//list of received data
	time_t tokenReceiveClock;					//system time when token was received
	bool hasToken;								//1 means have token, 0 means have no token
	int nextStation;							//next station to be offered the token

	boost::circular_buffer< unsigned int > *datagramDown;   //downwards input pointer
	boost::circular_buffer< unsigned int > *datagramUp;   	//upwards output pointer
	boost::circular_buffer< Frame > *frameDown;   			//downwards output pointer
	boost::circular_buffer< Frame > *frameUp;   			//upwards input pointer

	void processFrame(Frame);					//process data from frame buffer and put in datagram buffer
	void makeFrame(unsigned char,unsigned char);//process data from datagram buffer and put in frame buffer
	void acceptFrame(Frame);					//inserts frame in list
	void discardFrame();						//discards frame (by doing nothing)
	void fatalError();							//reports critical error

	void tokenOffered();						//receives token
	void checkToken();							//returns 0 if token is OK and 1 if time to pass token
	void offerToken();							//offer token to next station
	void passToken();							//passes token to accepting station

	void checkFrameRecList();					//check if list is complete
	void clearFrameRecList();					//clears list of received data

	void passDataUpwards();						//writes data into buffer
	void requestResend(int);					//requests resending of data missing in list

	void replyFromReceiver();					//checks reply from receiver and generates resend

public:
	DataLinkLayer();							//default constructor
	void decode(boost::circular_buffer< unsigned int >*,		//decode is called from backbone
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< unsigned int >*);

};
#endif /* DATALINKLAYER_H_ */

//End Of File

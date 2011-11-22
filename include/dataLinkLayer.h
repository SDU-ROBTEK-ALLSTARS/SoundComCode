//============================================================================
// Name        : dataLinkLayer.h
// Author      : Leon Bonde Larsen
// Version     :
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
#ifndef DATALINKLAYER_H_
#define DATALINKLAYER_H_
//=====     INCLUDES     =====
#include "frame.h"
#include <stdio.h>
#include <time.h>
//=====     DECLARATIONS     =====
struct datalist
{
	unsigned char dataByte;
	bool receivedFlag;
};

//=====     CLASS DECLARATION     =====
class DataLinkLayer
{
private:
public:
	struct datalist receivedList[8];			//list of received data
	time_t tokenReceiveClock;					//system time when token was received
	bool hasToken;								//1 means have token, 0 means have no token
	int nextStation;

	void makeFrame(unsigned char,unsigned char);//process data from datagram buffer and put in frame buffer
	void acceptFrame(Frame);					//inserts frame in list
	void discardFrame();						//discards frame (by doing nothing)
	void fatalError();							//reports critical error

	void tokenOffered();						//receives token
	void checkToken();							//returns 0 if token is OK and 1 if time to pass token
	void offerToken();							//offer token to next station
	void passToken();							//passes token to accepting station

	void checkReceivedList();					//check if list is complete
	void clearReceivedList();					//clears list of received data

	void passDataUpwards();						//writes data into buffer
	void requestResend(int);					//requests resending of data missing in list

//public:
	DataLinkLayer();							//constructor
	int processFrame(Frame);					//process data from frame buffer and put in datagram buffer

};

//=====     CLASS IMPLEMENTATION     =====
DataLinkLayer::DataLinkLayer()
{
	clearReceivedList();
	#ifdef STARTS_WITH_TOKEN
	tokenOffered();
	#endif
}
//=====
int DataLinkLayer::processFrame(Frame incoming)
{
	if( (incoming.receiver == MY_ADDRESS)  &&  (incoming.checkParity()==0)  )
		//if frame is for me and parity is ok
	{
		switch(incoming.type)
		{
		case 3:
			//if frame type is dataframe
		{
			acceptFrame(incoming);
			break;
		}
		case 2:
			//if frame is station accepting token
		{
			passToken();
			break;
		}
		case 1:
			//if frame is station offering token
		{
			tokenOffered();
			break;
		}
		case 0:
			//if frame is reply from receiver
		{

			break;
		}
		default:
			//impossible frame type
		{
			fatalError();
			break;
		}
		}
	}
	else
		//frame is not for me or parity was foobah
	{
		discardFrame();
		return 0;
	}
	return 0;
}
//=====
void DataLinkLayer::makeFrame(unsigned char header,unsigned  char data)
{
	Frame output(header,data);
	#ifdef DEBUG
	DEBUG_OUT << "PUT IN FRAME BUFFER: ";
	output.coutBytes();
	#endif

}
//=====
void DataLinkLayer::acceptFrame(Frame incoming)
{
	receivedList[incoming.sequence].dataByte = incoming.data;
	receivedList[incoming.sequence].receivedFlag = 1;

	#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
	#endif
}
//=====
void DataLinkLayer::discardFrame()
{
	#ifdef DEBUG
	DEBUG_OUT << "Frame discarded" << std::endl;
	#endif
}
//=====
void DataLinkLayer::fatalError()
{
	#ifdef DEBUG
	DEBUG_OUT << "A fatal error has occured!" << std::endl;
	#endif
}
void DataLinkLayer::tokenOffered()
{
	time ( &tokenReceiveClock );
	hasToken = 1;
	nextStation = MY_ADDRESS;
	#ifdef DEBUG
	DEBUG_OUT << "Token offered" << std::endl;
	std::cout << "Token received " << ctime (&tokenReceiveClock) << std::endl;
	#endif
}
//=====
void DataLinkLayer::checkToken()
{
	if(hasToken==1)
	{
		time_t nowClock;
		time ( &nowClock );
		if((nowClock-tokenReceiveClock)>MAX_TIME_WITH_TOKEN)
		{
			#ifdef DEBUG
			DEBUG_OUT << "Token expired" << std::endl;
			#endif
			offerToken();
		}
		else
		{
			#ifdef DEBUG
			std::cout << "Time left with token: " << (MAX_TIME_WITH_TOKEN-(nowClock-tokenReceiveClock))
				  << " seconds" << std::endl;
			#endif
		}
	}
	else
	{
		#ifdef DEBUG
		DEBUG_OUT << "checkToken called without token" << std::endl;
		#endif
	}
}
//=====
void DataLinkLayer::offerToken()
{
	nextStation++;
	if(nextStation>7)
		nextStation = 0;
	 makeFrame((1<<6)|(nextStation<<3),0);
}
//=====
void DataLinkLayer::passToken()
{
	//TODO: Her kunne inds¾ttes et tjek pŒ om den rette station har modtaget...
	hasToken = 0;
	#ifdef DEBUG
	DEBUG_OUT << "Token was passed" << std::endl;
	#endif
}
//=====
void DataLinkLayer::clearReceivedList()
{
	for(int i=0;i<8;i++)
	{
		receivedList[i].dataByte = 0;
		receivedList[i].receivedFlag = 0;
	}
}
//=====
void DataLinkLayer::checkReceivedList()
{
	#ifdef DEBUG
	DEBUG_OUT << "Current list entries: " << std::endl;
	for(int i=0;i<8;i++)
	{
	DEBUG_OUT << i << " - ";
	for(int j=7;j>=0;j--)
		DEBUG_OUT << (bool)(receivedList[i].dataByte&(1<<j));
	if(receivedList[i].receivedFlag==1)
		DEBUG_OUT << " - received" << std::endl;
	else
		DEBUG_OUT << " - missing" << std::endl;
	}
	#endif

	int request = 0;
	for(int i=0;i<8;i++)
	{
		request |= ((bool)receivedList[i].receivedFlag<<i);
	}
	if(request == 0xFF)
	{
		passDataUpwards();
	}
	else
	{
		requestResend(request);
	}
}
//=====
void DataLinkLayer::passDataUpwards()
{
	#ifdef DEBUG
	DEBUG_OUT << "PUT IN DATAGRAM BUFFER:";
	for(int i=0;i<8;i++)
	{
		for(int j=7;j>=0;j--)
			DEBUG_OUT << (bool)(receivedList[i].dataByte&(1<<j));
		DEBUG_OUT << " ";
	}
	DEBUG_OUT << std::endl;
	#endif
	makeFrame(MY_ADDRESS<<3,~0);
}
//=====
void DataLinkLayer::requestResend(int request)
{
	char data;
	for(int j=0;j<8;j++)
		if(!(bool)(request&(1<<j)))
		{
			data |= 1<<j;
			#ifdef DEBUG
			DEBUG_OUT << "Requesting resend frame " << j << std::endl;
			#endif
		}
	makeFrame(MY_ADDRESS<<3,~data);
}
#endif /* DATALINKLAYER_H_ */

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

//=====     CLASS DECLARATION     =====
class DataLinkLayer
{
private:
	struct datalist
	{
		unsigned char dataByte;
		bool receivedFlag;
	};
	struct datalist receivedList[4];

	void frameAccepted(Frame);
	void frameDiscarded();
	void tokenOffered();
	void fatalError();
	void clearReceivedList();
	void passDataUpwards();
	void requestResend(int);

public:
	DataLinkLayer();
	int decode(Frame);
	void encode(unsigned char,unsigned char);
	void checkList();
};

//=====     CLASS IMPLEMENTATION     =====
DataLinkLayer::DataLinkLayer()
{
	clearReceivedList();
}
//=====
int DataLinkLayer::decode(Frame incoming)
{
	if( (incoming.receiver == MY_ADDRESS)  &&  (incoming.checkParity()==0)  )
		//if frame is for me and parity is ok
	{
		if(incoming.type == 3)
			//if frame type is dataframe
		{
			frameAccepted(incoming);
			return 0;
		}
		else if(incoming.type == 2)
			//if frame is station offering token
		{
			tokenOffered();
			return 0;
		}
		else
			//impossible frame type
		{
			fatalError();
		}
	}
	else
		//frame is not for me or parity was foobah
	{
		frameDiscarded();
		return 0;
	}
	return 0;
}
//=====
void DataLinkLayer::encode(unsigned char header,unsigned  char data)
{
	Frame output(header,data);
	#ifdef DEBUG
	DEBUG_OUT << "PUT IN FRAME BUFFER: ";
	output.coutBytes();
	#endif

}
//=====
void DataLinkLayer::frameAccepted(Frame incoming)
{
	receivedList[incoming.sequence].dataByte = incoming.data;
	receivedList[incoming.sequence].receivedFlag = 1;

	#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
	#endif
}
//=====
void DataLinkLayer::frameDiscarded()
{
	#ifdef DEBUG
	DEBUG_OUT << "Frame discarded" << std::endl;
	#endif
}
//=====
void DataLinkLayer::tokenOffered()
{
	#ifdef DEBUG
	DEBUG_OUT << "Token offered" << std::endl;
	#endif
}
//=====
void DataLinkLayer::fatalError()
{
	#ifdef DEBUG
	DEBUG_OUT << "Fatal error - ohh shit!" << std::endl;
	#endif
}
//=====
void DataLinkLayer::clearReceivedList()
{
	for(int i=0;i<4;i++)
	{
		receivedList[i].dataByte = 0;
		receivedList[i].receivedFlag = 0;
	}
}
//=====
void DataLinkLayer::checkList()
{
	#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
	DEBUG_OUT << "Current list entries: " << std::endl;
	for(int i=0;i<4;i++)
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
	for(int i=0;i<4;i++)
	{
		request |= ((bool)receivedList[i].receivedFlag<<i);
	}
	if(request == 0x0F)
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
	for(int i=0;i<4;i++)
	{
		for(int j=7;j>=0;j--)
			DEBUG_OUT << (bool)(receivedList[i].dataByte&(1<<j));
		DEBUG_OUT << " ";
	}
	DEBUG_OUT << std::endl;
	#endif
	encode(MY_ADDRESS<<4,~0);
}
//=====
void DataLinkLayer::requestResend(int request)
{
	char data;
	for(int j=0;j<4;j++)
		if(!(bool)(request&(1<<j)))
		{
			data |= 3<<(j*2);
			#ifdef DEBUG
			DEBUG_OUT << "Requesting resend frame " << j << std::endl;
			#endif
		}
	encode(MY_ADDRESS<<4,~data);
}
#endif /* DATALINKLAYER_H_ */

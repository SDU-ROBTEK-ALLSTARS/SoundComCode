/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011 Alexander Adelholm Brandbyge <abran09@student.sdu.dk>
# Frederik Hagelskjær <frhag10@student.sdu.dk>
# Kent Stark Olsen <keols09@student.sdu.dk>
# Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Leon Bonde Larsen <lelar09@student.sdu.dk>
# Rudi Hansen <rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
 ********************************************************************************
# File: DtmfDataLinkLayer.cpp
# Project: DtmfProject
# Version: 2.0
# Platform: PC/Mac/Linux
# Authors: Alexander Adelholm Brandbyge <abran09@student.sdu.dk>
# Frederik Hagelskjær <frhag10@student.sdu.dk>
# Kent Stark Olsen <keols09@student.sdu.dk>
# Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Leon Bonde Larsen <lelar09@student.sdu.dk>
# Rudi Hansen <rudha10@student.sdu.dk>
# Created: 2011-10-30
 ********************************************************************************
# Description
#
# The purpose of this class is to implement the data link layer of the OSI model.
# Processing frames into datagrams and datagrams into frames
# (Denne header er dikteret af den onde diktator Kent)
 *******************************************************************************/
//***** INCLUDES *****
#include <cstdlib>
#include <math.h>
#include "DtmfDatalinkLayer.h"


#undef DEBUG
//***** CLASS IMPLEMENTATION *****
DtmfDataLinkLayer::DtmfDataLinkLayer(int addr,bool tok)
{
	if(tok == 1)
	{
		hasToken = 1;
		time ( &timestampToken );
	}
	else
		hasToken = 0;

	MY_ADDRESS = nextStation = addr;

#ifdef DEBUG
	DEBUG_OUT << std::endl << "---------- ### INSTANTIATING DATA LINK LAYER ### ----------" << std::endl;
	if(hasToken)
		DEBUG_OUT << "Has token" << std::endl;
	DEBUG_OUT << "Address: " << MY_ADDRESS << std::endl;
#endif

	clearFrameReceiveList();
	clearFrameSendList();
	receiverNeedsUpdate = 1;
	nextInSendSequence = datagramIterator = lostFrameCount = successFrameCount = awaitsReply = generatedError = 0;
}
//*****
void DtmfDataLinkLayer::encode(  boost::circular_buffer< Packet > *downIn,
		boost::circular_buffer< Frame > *downOut,
		boost::circular_buffer< Frame > *upIn,
		boost::circular_buffer< unsigned int > *upOut)
{
#ifdef DEBUG
	DEBUG_OUT << std::endl << "---------- ### ENCODE ### ----------" << std::endl;
#endif

	//update pointers
	datagramDown = downIn;
	frameDown = downOut;
	frameUp = upIn;
	datagramUp = upOut;



	//do I have token?
	if(hasToken == 1)
	{
		//is the token expired?
		if(checkToken() == 1)
			return;
	}
	else
	{
#ifdef DEBUG
		DEBUG_OUT << "has no token" << std::endl;
#endif
		return;
	}

	//check if ready to process
	if(awaitsReply == 1)
	{
		//check current time
		time_t nowClock;
		time ( &nowClock );

		//compare with saved timestamp from when transmission ended
		if((nowClock-timestampAwaitsReply)>MAX_TIME_TO_REPLY)
		{
			//if time expired
#ifdef DEBUG
			DEBUG_OUT << "Time for receiver to reply is up" << std::endl;
#endif
			//resend all
			resendData(0);
		}
		else
		{
			//if there is time left to reply
#ifdef DEBUG
			std::cout << "Time left for receiver to reply: " <<
					(MAX_TIME_TO_REPLY-(nowClock-timestampAwaitsReply)) << " seconds" << std::endl;
#endif
		}
	}

	//start processing if there is data in the buffer
	if(!(datagramDown->empty()))
	{
		while(true)
		{
			//check if list is full
			if(nextInSendSequence >= 8)
			{
#ifdef DEBUG
				DEBUG_OUT << "List is full...ending transmission" << std::endl;
#endif
				endTransmission();
				break;
			}
			//check if end of datagram is reached
			if(datagramIterator >= datagramLength)
			{
				//previous datagram has ended and all data has been sent and confirmed
				if(dataInSendList == 0 && receiverNeedsUpdate == 0)
				{
					datagramDown->pop_front();
					datagramIterator = 0;
					receiverNeedsUpdate = 1;
					if(datagramDown->empty())
					{
#ifdef DEBUG
					DEBUG_OUT << "Last element popped" << std::endl;
#endif
					return;
					}
				}
				if(receiverNeedsUpdate == 1)
				{
					//get address of receiver
					currentReceiver = datagramDown->front().recvAddr();
					datagramLength = datagramDown->front().totalLength();
					receiverNeedsUpdate = 0;

#ifdef DEBUG
					DEBUG_OUT << "Setting receiver address to " << currentReceiver << std::endl;
#endif
				}
				else
				{
					//datagram has ended but sent data has not yet been confirmed
#ifdef DEBUG
					DEBUG_OUT << "Buffer is empty...ending transmission" << std::endl;
#endif
					//Set EOT bit in last frame
					endTransmission();
					break;
				}
			}
			else
			{
				//still data to process in this datagram
#ifdef DEBUG
				DEBUG_OUT << "Popping from datagrambuffer..." << (int)datagramDown->front()[datagramIterator] << std::endl;
#endif
				processDatagram((unsigned int)datagramDown->front()[datagramIterator]);
				datagramIterator++;
			}
		}
	}
	else
	{
		//buffer is empty
#ifdef DEBUG
		DEBUG_OUT << "Buffer is empty...nothing to process" << std::endl;
#endif

		if(hasToken == 1 && tokenAlreadyOffered == 0 && dataInSendList == 0)
		{
			// nothing to process. Might as well pass token
#ifdef DEBUG
			DEBUG_OUT << "No need for token...passing" << std::endl;
#endif
			offerToken();
		}
	}
}
//*****
void DtmfDataLinkLayer::decode(
		boost::circular_buffer< Packet > *downIn,
		boost::circular_buffer< Frame > *downOut,
		boost::circular_buffer< Frame > *upIn,
		boost::circular_buffer< unsigned int > *upOut)
{
#ifdef DEBUG
	DEBUG_OUT << std::endl << "---------- ### DECODE ### ----------" << std::endl;
#endif
	//update pointers
	datagramDown = downIn;
	frameDown = downOut;
	frameUp = upIn;
	datagramUp = upOut;

	//reset flagbits
	replyGenerated = 0;
	noReply = 1;

#ifdef DEBUG
	DEBUG_OUT << "Popping from framebuffer..." << std::endl;
#endif

	//nothing to process
	if(frameUp->empty())
		return;

	//process frames
	while(!frameUp->empty())
	{
		processFrame(frameUp->front());
		if(frameUp->front().end == 1)
			eotReceived = 1;
		frameUp->pop_front();
	}

	//check list if EOT was received
	if(replyGenerated == 0 && hasToken == 0 && eotReceived == 1)
		checkFrameReceiveList();
}
//*****
bool DtmfDataLinkLayer::needsAttention()
{
	time_t nowClock;
	time (&nowClock);

	//check timestamps
	if(tokenAlreadyOffered == 1 && (nowClock-timestampTokenOffered)>MAX_TIME_OFFERING_TOKEN)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to re-offer token..." << std::endl;
#endif
		return 1;
	}
	if(awaitsReply == 1 && (nowClock-timestampAwaitsReply)>MAX_TIME_TO_REPLY)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to resend data..." << std::endl;
#endif
		return 1;
	}
	if(hasToken == 1 && (nowClock-timestampToken)>MAX_TIME_WITH_TOKEN)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to pass token..." << std::endl;
#endif
		return 1;
	}
	return 0;

}
//*****
bool DtmfDataLinkLayer::canTransmit()
{
	return hasToken || eotReceived;
}
//*****
void DtmfDataLinkLayer::processFrame(Frame incoming)
{
	if(incoming.checkParity()==0)
	{
		if(incoming.receiver == MY_ADDRESS)
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
				if(hasToken == 1)
					replyFromReceiver(incoming);
				else
				{
#ifdef DEBUG
					DEBUG_OUT << " reply to tokenholder " ;
#endif
					discardFrame();
				}
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
			//frame is not for me
		{
			if(incoming.type == 0 && hasToken == 1)
				//if frame is reply from receiver and tokenholder is me
				replyFromReceiver(incoming);
			else if(incoming.type == 2 && hasToken == 1)
				//if frame is accept token and I have token
				passToken();
			else
			{
#ifdef DEBUG
				DEBUG_OUT << " wrong receiver " ;
#endif
				discardFrame();
			}
		}
	}
	else
	{
#ifdef DEBUG
		DEBUG_OUT << " bad parity " ;
#endif
		discardFrame();
	}
}
//*****
bool DtmfDataLinkLayer::awaitsReplys()
{
	return awaitsReply;
}

//*****
void DtmfDataLinkLayer::makeFrame(unsigned char header,unsigned char data)
{
	//push frame to buffer
	frameDown->push_back(Frame(header,data));

#ifdef DEBUG
	DEBUG_OUT << "Pushing to framebuffer: " << frameDown->back();
#endif

#ifdef GENERATE_ERRORS
	simulateError();
#endif
}
//*****
void DtmfDataLinkLayer::endTransmission()
{
#ifdef DEBUG
	DEBUG_OUT << "Setting EOT-bit..." << std::endl;
#endif

	//can never try to access an empty buffer
	if(frameDown->empty())
		return;

	//pop last frame put in buffer
	Frame temp = frameDown->back();
	frameDown->pop_back();

	//extract bytes and set EOT bit
	unsigned int tempHeader,tempData;
	tempData = temp.byte1;
	tempHeader = temp.byte2;
	tempHeader |= 1;

	//re-push altered frame
	frameDown->push_back(Frame(tempHeader,tempData));

	//timestamp
	time ( &timestampAwaitsReply );
	awaitsReply = 1;
}
//*****
void DtmfDataLinkLayer::acceptFrame(Frame incoming)
{
	noReply = 0;
	successFrameCount++;

	//put frame in list
	frameRecList[incoming.sequence].dataByte = incoming.data;
	frameRecList[incoming.sequence].flag = 1;

	//mark if end of transmission is set
	if(incoming.end == 1)
		frameRecList[incoming.sequence].eot = 1;

#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
#endif

	//check end of transmission bit and process list if set TODO: might no longer be necessary
	//	if(incoming.end == 1)
	//		checkFrameReceiveList();
}
//*****
void DtmfDataLinkLayer::discardFrame()
{
	//discard frame
	lostFrameCount++;
#ifdef DEBUG
	DEBUG_OUT << "Frame discarded" << std::endl;
#endif
}
//*****
void DtmfDataLinkLayer::processDatagram(unsigned int incoming)
{
	dataInSendList = 1;

	//generate frame
	makeFrame(
			(3<<TYPE)| //type 3
			(currentReceiver<<ADDRESS)| //receiver address
			(nextInSendSequence<<SEQUENCE), //sequence number
			incoming); //data

	//put data in list
	frameSendList[nextInSendSequence].dataByte = incoming; //save data in list
	frameSendList[nextInSendSequence].flag = 1; //set flag

	//update sequence number
	nextInSendSequence++;
}
//*****
void DtmfDataLinkLayer::fatalError()
{
	//something is very bad...
#ifdef DEBUG
	DEBUG_OUT << "A fatal error has occured!" << std::endl;
#endif
}
void DtmfDataLinkLayer::tokenOffered()
{
	//set time
	time ( &timestampToken );

	//acquire token
	hasToken = 1;

	//set next station to me (increased when offered)
	nextStation = MY_ADDRESS;

#ifdef DEBUG
	DEBUG_OUT << "Token offered" << std::endl;
	std::cout << "Token received " << ctime (&timestampToken) << std::endl;
#endif

	//make type 2 frame for station before me
	makeFrame((2<<TYPE)|(MY_ADDRESS<<ADDRESS),~0);
}
//*****
bool DtmfDataLinkLayer::checkToken()
{
	//check current time
	time_t nowClock;
	time ( &nowClock );

	if(tokenAlreadyOffered == 1)
	{
#ifdef DEBUG
		DEBUG_OUT << "Token has been offered to " << nextStation;
#endif
		//check timestamp
		if((nowClock-timestampTokenOffered)>MAX_TIME_OFFERING_TOKEN)
		{
			//if expired
#ifdef DEBUG
			DEBUG_OUT << "...time to reply is up" << std::endl;
#endif
			offerToken();
			return 1;
		}
		else
		{
			//if not expired
#ifdef DEBUG
			DEBUG_OUT << "...still time left to reply" << std::endl;
#endif
			return 1;
		}
	}
	else
	{
		//compare with saved timestamp from when token was passed
		if((nowClock-timestampToken)>MAX_TIME_WITH_TOKEN)
		{
			//if token expired
#ifdef DEBUG
			DEBUG_OUT << "Token expired" << std::endl;
#endif
			offerToken();
			return 1;
		}
		else
		{
			//if token has time left
#ifdef DEBUG
			DEBUG_OUT << "Time left with token: " << (MAX_TIME_WITH_TOKEN-(nowClock-timestampToken))
				<< " seconds" << std::endl;
#endif
			return 0;
		}
	}
	return 0;
}
//*****
void DtmfDataLinkLayer::offerToken()
{
	//increase token receiver
	nextStation++;

	//flip over if higher than number of addresses
	if(nextStation>3)
		nextStation = 0;

	//if next receiver has been increased four times
	if(nextStation==MY_ADDRESS)
	{
#ifdef DEBUG
		DEBUG_OUT << "Offering token to self" << std::endl;
#endif
		//offer token to self
		tokenOffered();
		tokenAlreadyOffered = 0;
		noReply = 1;
	}
	else
	{
#ifdef DEBUG
		DEBUG_OUT << "Offering token to " << nextStation << std::endl;
#endif
		tokenAlreadyOffered = 1;
		time( &timestampTokenOffered );
		//generate frame to offer token to next station
		makeFrame((1<<TYPE)|(nextStation<<ADDRESS),0);
	}
}
//*****
void DtmfDataLinkLayer::passToken()
{
	//release token
	hasToken = 0;
	tokenAlreadyOffered = 0;
	noReply = 1;

#ifdef DEBUG
	DEBUG_OUT << "Token was passed" << std::endl;
#endif
}
//*****
void DtmfDataLinkLayer::clearFrameReceiveList()
{
#ifdef DEBUG
	DEBUG_OUT << "clearing receive list" << std::endl;
#endif

	//clear each entry in list
	for(int i=0;i<8;i++)
	{
		frameRecList[i].dataByte = 0;
		frameRecList[i].flag = 0;
		frameRecList[i].eot = 0;
	}
}
//*****
void DtmfDataLinkLayer::clearFrameSendList()
{
#ifdef DEBUG
	DEBUG_OUT << "clearing send list" << std::endl;
#endif

	//clear each entry in list
	for(int i=0;i<8;i++)
	{
		frameSendList[i].dataByte = 0;
		frameSendList[i].flag = 0;
	}

	nextInSendSequence = 0;
	dataInSendList = 0;
}

void DtmfDataLinkLayer::checkFrameReceiveList()
{
	//initialize variable to hold EOT, goes and nogoes
	int request = 0;
	int listLength = 0;

	//iterate through list to find list length
	for(int i=0;i<8;i++)
		if(frameRecList[i].eot == 1)
			listLength = i+1;

#ifdef DEBUG
	DEBUG_OUT << "Checking list of " << listLength << " entries..." << std::endl;
	for(int i=0;i<listLength;i++)
	{
		DEBUG_OUT << i << " - ";
		for(int j=7;j>=0;j--)
			DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<j));
		if(frameRecList[i].flag==1)
			DEBUG_OUT << " - received";
		else
			DEBUG_OUT << " - missing";
		if(frameRecList[i].eot == 1)
			DEBUG_OUT << " - End of transmission";
		DEBUG_OUT << std::endl;
	}
#endif

	//iterate through list and and set request variable according to flagbits
	for(int j=0;j<listLength;j++)
	{
		for(int i=0;i<j+1;i++)
			request |= ((bool)(frameRecList[i].flag)<<i);
	}

	//pass upwards if list is complete
	if(request == (pow(2.,listLength)-1) && listLength > 0)
		passDataUpwards(listLength);
	else
		//request resends
		requestResend(request);
}

//*****
void DtmfDataLinkLayer::passDataUpwards(int listLength)
{
#ifdef DEBUG
	DEBUG_OUT << "Pushing to datagram buffer:";
#endif

	//iterate through list and push to datagram buffer
	for(int i=0;i<listLength;i++)
	{
#ifdef DEBUG
		for(int k=7;k>=0;k--)
			DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<k));
		DEBUG_OUT << " ";
#endif
		datagramUp->push_back(frameRecList[i].dataByte);
	}
	std::cout << std::endl;

#ifdef DEBUG
	DEBUG_OUT << std::endl << "Gerating OK reply to sender" << std::endl;
#endif

	//generate ok for sender
	makeFrame((MY_ADDRESS<<ADDRESS)|(1<<EOT),~0);
	replyGenerated = 1;

	//clear list
	clearFrameReceiveList();
}
//*****
void DtmfDataLinkLayer::requestResend(int request)
{
#ifdef DEBUG
	for(int j=0;j<8;j++)
		if(!(bool)(request&(1<<j)))
			DEBUG_OUT << "Requesting resend frame " << j << std::endl;
#endif

	//make type 0 frame with data byte holding ones for received an zeroes for resend
	makeFrame((MY_ADDRESS<<ADDRESS)|(1<<EOT),request);
	replyGenerated = 1;
}
//*****
void DtmfDataLinkLayer::replyFromReceiver(Frame incoming)
{
	awaitsReply = 0;
	if(incoming.data == 255)
	{
#ifdef DEBUG
		DEBUG_OUT << "OK from receiver...";
#endif
		clearFrameSendList();
	}
	else
	{
#ifdef DEBUG
		DEBUG_OUT << "Receiver requesting frames...";
		for(int i=0;i<8;i++)
		{
			if((bool)(incoming.data & (1<<i)) == 0)
				DEBUG_OUT << " " << i;
		}
		DEBUG_OUT << std::endl;
#endif
		resendData(incoming.data);
	}
}
//*****
void DtmfDataLinkLayer::resendData(unsigned int request)
{
	//iterate through databyte
	for(int i=0;i<8;i++)
	{
		//resend requested frames
		if((bool)(request & (1<<i)) == 0)
		{
			makeFrame(
					(3<<TYPE)| //type 3
					(currentReceiver<<ADDRESS)| //receiver address
					(i<<SEQUENCE), //sequence number
					frameSendList[i].dataByte); //data
		}
	}
	//set EOT bit in last frame
	endTransmission();
}
//*****
#ifdef GENERATE_ERRORS
void DtmfDataLinkLayer::simulateError()
{
	//generate random number
	int random = rand()&0xFF;

	if((random*100/255)<ERROR_PERCENTAGE)
	{
#ifdef DEBUG
		DEBUG_OUT << "<<<<< SIMULATING ERROR >>>>>" << std::endl;
#endif

		//pop last frame put in buffer
		Frame temp = frameDown->back();
		frameDown->pop_back();

		generatedError++;
		//re-push altered frame
		frameDown->push_back(Frame(temp.byte1,temp.byte2,temp.byte0 + random));
	}
}
#endif
//End Of File

#define DEBUG

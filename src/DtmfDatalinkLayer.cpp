/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#						Frederik Hagelskjær				<frhag10@student.sdu.dk>
#						Kent Stark Olsen				<keols09@student.sdu.dk>
#						Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#						Leon Bonde Larsen				<lelar09@student.sdu.dk>
#						Rudi Hansen						<rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File:     DtmfDatalinkLayer.cpp
# Project:  DtmfProject
# Version:  2.0
# Platform:	PC/Mac/Linux
# Authors:  Alexander Adelholm Brandbyge	<abran09@student.sdu.dk>
#			Frederik Hagelskjær				<frhag10@student.sdu.dk>
#			Kent Stark Olsen				<keols09@student.sdu.dk>
#			Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
#			Leon Bonde Larsen				<lelar09@student.sdu.dk>
#			Rudi Hansen						<rudha10@student.sdu.dk>
# Created:  2011-10-30
********************************************************************************
# Description
#
# The purpose of this class is to implement the data link layer of the OSI model.
# Processing frames into datagrams and datagrams into frames
#                           (Denne header er dikteret af den onde diktator Kent)
*******************************************************************************/

//*****     INCLUDES     *****
#include 	"DtmfDatalinkLayer.h"

//*****     CLASS IMPLEMENTATION     *****
DtmfDatalinkLayer::DtmfDatalinkLayer(int addr,bool tok)
{
	if(tok == 1)
	{
	hasToken = 1;
	time ( &timestampToken );
	currentReceiver = 1; //TODO: temporary implementation
	}
	else
	{
	hasToken = 0;
	currentReceiver = 0; //TODO: temporary implementation
	}
	MY_ADDRESS = addr;
	#ifdef DEBUG
	DEBUG_OUT << std::endl << "----------   ###   INSTANTIATING DATA LINK LAYER   ###   ----------" << std::endl;
	if(hasToken)
		DEBUG_OUT << "Has token" << std::endl;
	DEBUG_OUT << "Address: " << MY_ADDRESS << std::endl;
	#endif
	clearFrameReceiveList();
	clearFrameSendList();
	awaitsReply = 0;
	receiverNeedsUpdate = 0;
	nextInSendSequence = 0;

}
//*****
void DtmfDatalinkLayer::encode(
		boost::circular_buffer< unsigned int > *downIn,
		boost::circular_buffer< Frame > *downOut,
		boost::circular_buffer< Frame > *upIn,
		boost::circular_buffer< unsigned int > *upOut)
{
	#ifdef DEBUG
	DEBUG_OUT << std::endl << "----------   ###   ENCODE   ###   ----------" << std::endl;
	#endif

	//update pointers
	datagramDown = downIn;
	datagramUp = upOut;
	frameDown = downOut;
	frameUp = upIn;

	if(hasToken == 1)
	{
		//if token expired
		if(checkToken() == 1)
		{
			return;
		}
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
			resendData(~0);
		}
		else
		{
			//if there is time left
			#ifdef DEBUG
			std::cout << "Time left for receiver to reply: " << (MAX_TIME_TO_REPLY-(nowClock-timestampAwaitsReply))
				  << " seconds" << std::endl;
			#endif
		}
	}

	#ifdef DEBUG
	DEBUG_OUT << "Popping from datagrambuffer..." << std::endl;
	#endif

	//process datagrams
	if(!datagramDown->empty())
	{
		while(1)
		{
//TODO: implement two-dimensional buffer, zero next in send sequence
			if(datagramDown->empty())
			{
				//Set EOT bit in last frame
				#ifdef DEBUG
				DEBUG_OUT << "Buffer is empty...ending transmission" << std::endl;
				#endif
				endDatagram();
				receiverNeedsUpdate = 1;
				awaitsReply = 1;
				time ( &timestampAwaitsReply );
				break;
			}
			if(nextInSendSequence >= 8)
			{
				#ifdef DEBUG
				DEBUG_OUT << "List is full...ending transmission" << std::endl;
				#endif
				break;
			}
			processDatagram(datagramDown->front());
			datagramDown->pop_front();
		}
	}
	else
	{
		#ifdef DEBUG
		DEBUG_OUT << "Buffer is empty...nothing to process" << std::endl;
		#endif
		if(hasToken == 1 && tokenAlreadyOffered == 0)
		{
			//Might as well pass token
			#ifdef DEBUG
			DEBUG_OUT << "No need for token...passing" << std::endl;
			#endif
			offerToken();
		}
	}


}
//*****
void DtmfDatalinkLayer::decode(
		boost::circular_buffer< unsigned int > *downIn,
		boost::circular_buffer< Frame > *downOut,
		boost::circular_buffer< Frame > *upIn,
		boost::circular_buffer< unsigned int > *upOut)
{
	#ifdef DEBUG
	DEBUG_OUT << std::endl << "----------   ###   DECODE   ###   ----------" << std::endl;
	#endif
	//update pointers
	datagramDown = downIn;
	datagramUp = upOut;
	frameDown = downOut;
	frameUp = upIn;

	#ifdef DEBUG
	DEBUG_OUT << "Popping from framebuffer..." << std::endl;
	#endif

	//process frames
	while(!frameUp->empty())
	{
	processFrame(frameUp->front());
	frameUp->pop_front();
	}
}
//*****
void DtmfDatalinkLayer::processFrame(Frame incoming)
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
void DtmfDatalinkLayer::makeFrame(unsigned char header,unsigned  char data)
{
	//push frame to buffer
	frameDown->push_back(Frame(header,data));

	#ifdef DEBUG
	DEBUG_OUT << "Pushing to framebuffer: " << frameDown->back();
	#endif
}
//*****
void DtmfDatalinkLayer::endDatagram()
{
	#ifdef DEBUG
	DEBUG_OUT << "Setting EOT-bit..." << std::endl;
	#endif

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
}
//*****
void DtmfDatalinkLayer::acceptFrame(Frame incoming)
{
	//put frame in list
	frameRecList[incoming.sequence].dataByte = incoming.data;
	frameRecList[incoming.sequence].flag = 1;

	//mark if end of transmission is set
	if(incoming.end == 1)
		frameRecList[incoming.sequence].eot = 1;

	#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
	#endif

	//check end of transmission bit and process list if set
	if(incoming.end == 1)
		checkFrameReceiveList();
}
//*****
void DtmfDatalinkLayer::discardFrame()
{
	//discard frame
	#ifdef DEBUG
	DEBUG_OUT << "Frame discarded" << std::endl;
	#endif
}
//*****
void DtmfDatalinkLayer::processDatagram(unsigned int incoming)
{
	//update receiver if incoming is first byte of new datagram
	if(receiverNeedsUpdate == 1)
	{
		currentReceiver = (incoming >> 6);
		receiverNeedsUpdate = 0;
	}

	//generate frame
	makeFrame(
			(3<<TYPE)|   									//type 3
			(currentReceiver<<ADDRESS)| 					//receiver address
			(nextInSendSequence<<SEQUENCE)|   				//sequence number
			(nextInSendSequence==7?1:0),					//set end of transmission if seq.no is 7
			incoming);  									//data

	//put data in list
	frameSendList[nextInSendSequence].dataByte = incoming; 	//save data in list
	frameSendList[nextInSendSequence].flag = 1;  		   	//set flag

	//update sequence number
	nextInSendSequence++;
}
//*****
void DtmfDatalinkLayer::fatalError()
{
	//something is very bad...
	#ifdef DEBUG
	DEBUG_OUT << "A fatal error has occured!" << std::endl;
	#endif
}
void DtmfDatalinkLayer::tokenOffered()
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
bool DtmfDatalinkLayer::checkToken()
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
void DtmfDatalinkLayer::offerToken()
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
void DtmfDatalinkLayer::passToken()
{
	//release token
	hasToken = 0;
	tokenAlreadyOffered = 0;

	#ifdef DEBUG
	DEBUG_OUT << "Token was passed" << std::endl;
	#endif
}
//*****
void DtmfDatalinkLayer::clearFrameReceiveList()
{
	#ifdef DEBUG
		DEBUG_OUT << "clearing receive list" << std::endl;
	#endif

	//clear each entry in list
	for(int i=0;i<8;i++)
	{
		frameRecList[i].dataByte = 0;
		frameRecList[i].flag = 0;
	}
}
//*****
void DtmfDatalinkLayer::clearFrameSendList()
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
}
//*****
void DtmfDatalinkLayer::checkFrameReceiveList()
{
	//initialize variable to hold EOT, goes and nogoes
	bool transmissionStop = 0;
	int request = 0;

	#ifdef DEBUG
	DEBUG_OUT << "Checking list...entries: " << std::endl;
	for(int i=0;i<8;i++)
	{
	if(transmissionStop==1)
		break;
	DEBUG_OUT << i << " - ";
	for(int j=7;j>=0;j--)
		DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<j));
	if(frameRecList[i].flag==1)
		DEBUG_OUT << " - received";
	else
		DEBUG_OUT << " - missing";
	if(frameRecList[i].eot == 1)
	{
		DEBUG_OUT << " - End of transmission";
		transmissionStop = 1;
	}
	DEBUG_OUT  << std::endl;
	}
	#endif

	//reset EOT
	transmissionStop = 0;

	//iterate through list
	for(int i=0;i<8;i++)
	{
		//set corresponding bit if seq. no is received or transmission stopped
		request |= ((bool)(frameRecList[i].flag|transmissionStop)<<i);

		//update if eot bit is set
		if(frameRecList[i].eot == 1)
			transmissionStop = 1;
	}

	//pass upwards if list is complete
	if(request == 0xFF)
		passDataUpwards();
	else
		//request resends
		requestResend(request);
}
//*****
void DtmfDatalinkLayer::passDataUpwards()
{
	bool transmissionStop = 0;

	//push to datagram buffer
	for(int i=0;i<8;i++)
	{
		//break at the end of transmission
		if(transmissionStop==1)
			break;

		//push to buffer
		datagramUp->push_back(frameRecList[i].dataByte);

		//update if EOT is set
		if(frameRecList[i].eot == 1)
			transmissionStop = 1;
	}

	#ifdef DEBUG
	transmissionStop = 0;
	DEBUG_OUT << "Pushing to datagram buffer:";
	for(int i=0;i<8;i++)
	{
		if(transmissionStop==1)
			break;
		for(int j=7;j>=0;j--)
			DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<j));
		DEBUG_OUT << " ";
		if(frameRecList[i].eot == 1)
				transmissionStop = 1;
	}
	DEBUG_OUT << std::endl << "Gerating OK reply to sender" << std::endl;
	#endif

	//generate ok for sender
	makeFrame((MY_ADDRESS<<ADDRESS)|(1<<EOT),~0);

	//clear list
	clearFrameReceiveList();
}
//*****
void DtmfDatalinkLayer::requestResend(int request)
{
	#ifdef DEBUG
	for(int j=0;j<8;j++)
		if(!(bool)(request&(1<<j)))
			DEBUG_OUT << "Requesting resend frame " << j << std::endl;
	#endif

	//make type 0 frame with data byte holding ones for received an zeroes for resend
	makeFrame((MY_ADDRESS<<ADDRESS)|(1<<EOT),request);
}
//*****
void DtmfDatalinkLayer::replyFromReceiver(Frame incoming)
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
				if((bool)(incoming.byte1 & (1<<i)) == 0)
					DEBUG_OUT << " " << i;
			}
			DEBUG_OUT << std::endl;
		#endif
			resendData(incoming.byte1);
	}
}
//*****
void DtmfDatalinkLayer::resendData(unsigned int request)
{
	//iterate through databyte
	for(int i=0;i<8;i++)
	{
		//resend requested frames
		if((bool)(request & (1<<i)) == 0)
		{
			makeFrame(
					(3<<TYPE)|   									//type 3
					(currentReceiver<<ADDRESS)| 					//receiver address
					(i<<SEQUENCE),   								//sequence number
					frameSendList[i].dataByte);  					//data
		}
	}
	//set EOT bit in last frame
	endDatagram();
}

//*****
bool DtmfDatalinkLayer::needsAttention()
{
	time_t nowClock;
	time (&nowClock);

	//check timestamps
	if((nowClock-timestampTokenOffered)>MAX_TIME_OFFERING_TOKEN)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to re-offer token..." << std::endl;
#endif
		return 1;
	}
	if((nowClock-timestampAwaitsReply)>MAX_TIME_TO_REPLY)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to resend data..." << std::endl;
#endif
		return 1;
	}
	if((nowClock-timestampToken)>MAX_TIME_WITH_TOKEN)
	{
#ifdef DEBUG
		DEBUG_OUT << "Time to pass token..." << std::endl;
#endif
		return 1;
	}
	return 0;

}

//End Of File

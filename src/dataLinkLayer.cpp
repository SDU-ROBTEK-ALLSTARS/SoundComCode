//============================================================================
// Name        : dataLinkLayer.cpp
// Author      : Leon Bonde Larsen
// Version     : 1.0 (updated 27/11-2011 by Leon Larsen)
// Copyright   : Open Source
// Description : Testing data link layer in C++, Ansi-style
//============================================================================
//=====     INCLUDES     =====
#include 	"dataLinkLayer.h"

//=====     CLASS IMPLEMENTATION     =====
DataLinkLayer::DataLinkLayer()
{
	#ifdef STARTS_WITH_TOKEN
	hasToken = 1;
	#else
	hasToken = 0;
	#endif
	#ifdef DEBUG
	DEBUG_OUT << std::endl << "----------   ###   INSTANTIATING DATA LINK LAYER   ###   ----------" << std::endl;
	if(hasToken)
		DEBUG_OUT << "Has token" << std::endl;
	#endif
	clearFrameReceiveList();
	clearFrameSendList();

}
//=====
void DataLinkLayer::encode(
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

	#ifdef DEBUG
	DEBUG_OUT << "Popping from datagrambuffer..." << std::endl;
	#endif

	currentReceiver = MY_ADDRESS;
	nextInSendSequence = 0;
//TODO: timestampReply
	//process datagrams
	if(!datagramDown->empty())
	{
		while(1)
		{
			if(datagramDown->empty())
			{
				//Set EOT bit in last frame
				#ifdef DEBUG
				DEBUG_OUT << "Buffer is empty...ending transmission" << std::endl;
				#endif
				endDatagram();
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
	}


}
//=====
void DataLinkLayer::decode(
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

	//TODO: ved tom buffer, tjek timestampOK

	//process frames
	while(!frameUp->empty())
	{
	processFrame(frameUp->front());
	frameUp->pop_front();
	}
}
//=====
void DataLinkLayer::processFrame(Frame incoming)
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
			if(hasToken == 1)
				replyFromReceiver(incoming);
			else
				discardFrame();
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
		if(incoming.type == 0 && hasToken == 1)
			//if frame is reply from receiver and tokenholder is me
			replyFromReceiver(incoming);
		else
			discardFrame();
	}
}
//=====
void DataLinkLayer::makeFrame(unsigned char header,unsigned  char data)
{
	//push frame to buffer
	frameDown->push_back(Frame(header,data));

	#ifdef DEBUG
	DEBUG_OUT << "Pushing to framebuffer: ";
	frameDown->back().coutBytes();
	#endif

}
//=====
void DataLinkLayer::endDatagram()
{
	#ifdef DEBUG
	DEBUG_OUT << "Setting EOT-bit..." << std::endl;
	#endif

	//pop last frame put in buffer
	Frame temp = frameDown->back();
	frameDown->pop_back();

	//set EOT bit
	unsigned int tempHeader,tempData;
	tempData = temp.byte1;
	tempHeader = temp.byte2;
	tempHeader |= 1;

	//re-push altered frame
	frameDown->push_back(Frame(tempHeader,tempData));
}
//=====
void DataLinkLayer::acceptFrame(Frame incoming)
{
	//put frame in list
	frameRecList[incoming.sequence].dataByte = incoming.data;
	frameRecList[incoming.sequence].flag = 1;

	#ifdef DEBUG
	DEBUG_OUT << "Frame accepted" << std::endl;
	#endif

	//check end of transmission bit and process list if set
	if(incoming.end == 1)
		checkFrameReceiveList();
}
//=====
void DataLinkLayer::discardFrame()
{
	//discard frame
	#ifdef DEBUG
	DEBUG_OUT << "Frame discarded" << std::endl;
	#endif
}
//=====
void DataLinkLayer::processDatagram(unsigned int incoming)
{
	//TODO: decide if currentReceiver needs update
	makeFrame(
			(3<<6)|   										//type 3
			(currentReceiver<<4)| 							//receiver address
			(nextInSendSequence<<1)|   						//sequence number
			(nextInSendSequence==7?1:0),					//set end of transmission if seq.no is 7
			incoming);  									//data
	frameSendList[nextInSendSequence].dataByte = incoming; 	//save data in list
	frameSendList[nextInSendSequence].flag = 1;  		   	//set flag
	nextInSendSequence++;
}
//=====
void DataLinkLayer::fatalError()
{
	//something is very bad...
	#ifdef DEBUG
	DEBUG_OUT << "A fatal error has occured!" << std::endl;
	#endif
}
void DataLinkLayer::tokenOffered()
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
	makeFrame((2<<6)|MY_ADDRESS?3:MY_ADDRESS-1<<4,~0);
}
//=====
void DataLinkLayer::checkToken()
{
	if(hasToken==1)
	{
		//check current time
		time_t nowClock;
		time ( &nowClock );

		//compare with saved timestamp from when token was passed
		if((nowClock-timestampToken)>MAX_TIME_WITH_TOKEN)
		{
			//if token expired
			#ifdef DEBUG
			DEBUG_OUT << "Token expired" << std::endl;
			#endif
			offerToken();
		}
		else
		{
			//if token has time left
			#ifdef DEBUG
			std::cout << "Time left with token: " << (MAX_TIME_WITH_TOKEN-(nowClock-timestampToken))
				  << " seconds" << std::endl;
			#endif
			//TODO: do what?
		}
	}
	else
	{
		//if no token
		#ifdef DEBUG
		DEBUG_OUT << "checkToken called without token" << std::endl;
		#endif
	}
}
//=====
void DataLinkLayer::offerToken()
{
	//increase next receiver of token
	nextStation++;

	//flip over if higher than no of addresses
	if(nextStation>3)
		nextStation = 0;

	//if next receiver has been increased four times
	if(nextStation==MY_ADDRESS)
		//offer token to self
		tokenOffered();
	else
		//offer frame to next station
		makeFrame((1<<6)|(nextStation<<3),0);
}
//=====
void DataLinkLayer::passToken()
{
	//release token
	hasToken = 0;

	#ifdef DEBUG
	DEBUG_OUT << "Token was passed" << std::endl;
	#endif
}
//=====
void DataLinkLayer::clearFrameReceiveList()
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
//=====
void DataLinkLayer::clearFrameSendList()
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
//=====
void DataLinkLayer::checkFrameReceiveList()
{
	#ifdef DEBUG
	DEBUG_OUT << "Checking list...entries: " << std::endl;
	for(int i=0;i<8;i++)
	{
	DEBUG_OUT << i << " - ";
	for(int j=7;j>=0;j--)
		DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<j));
	if(frameRecList[i].flag==1)
		DEBUG_OUT << " - received" << std::endl;
	else
		DEBUG_OUT << " - missing" << std::endl;
	}
	#endif

	//initialize variable to hold goes and nogoes
	int request = 0;

	//iterate through list
	for(int i=0;i<8;i++)
		//set corresponding bit if seq. no is received
		request |= ((bool)frameRecList[i].flag<<i);

	//if list is complete
	if(request == 0xFF)
		//finish list
		passDataUpwards();
	else
		//request resends
		requestResend(request);
}
//=====
void DataLinkLayer::passDataUpwards()
{
	//push to datagram buffer
	for(int i=0;i<8;i++)
		datagramUp->push_back(frameRecList[i].dataByte);

	#ifdef DEBUG
	DEBUG_OUT << "Pushing to datagram buffer:";
	for(int i=0;i<8;i++)
	{
		for(int j=7;j>=0;j--)
			DEBUG_OUT << (bool)(frameRecList[i].dataByte&(1<<j));
		DEBUG_OUT << " ";
	}
	DEBUG_OUT << std::endl << "Gerating OK reply to sender" << std::endl;
	#endif

	//generate ok for sender
	makeFrame(MY_ADDRESS<<4,~0);

	//TODO: What happens if this frame is lost...

	//clear list
	clearFrameReceiveList();
}
//=====
void DataLinkLayer::requestResend(int request)
{
	//TODO: This function could be spared. Some functionalities has been moved
	#ifdef DEBUG
	for(int j=0;j<8;j++)
		if(!(bool)(request&(1<<j)))
			DEBUG_OUT << "Requesting resend frame " << j << std::endl;
	#endif

	//make type 0 frame with data byte holding ones for received an zeroes for resend
	makeFrame(MY_ADDRESS<<4,request);
}
//=====
void DataLinkLayer::replyFromReceiver(Frame incoming)
{
	//TODO: resend missing frames

	if(incoming.data == 0)
	{
		#ifdef DEBUG
			DEBUG_OUT << "OK from receiver...";
		#endif
		clearFrameSendList();
	}
	else
	{
		#ifdef DEBUG
			DEBUG_OUT << "Receiver requesting resend..." << std::endl;
		#endif
	}
}

//End Of File

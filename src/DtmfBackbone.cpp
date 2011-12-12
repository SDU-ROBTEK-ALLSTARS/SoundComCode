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
# File:     Dtmfbackbone.cpp
# Project:  DtmfBackbone
# Version:  1.0
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
# Implementation of the class DtmfBackbone
#
*******************************************************************************/

#include "DtmfBackbone.h"

//The backbone instantiates the buffers and individual layers, and ends off with launching its own operator thread. 
//Since the api constructs the backbone, a pointer to it, and its message buffers is given as well.
DtmfBackbone::DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferDown,DtmfMsgBuffer *& msgBufferUp)
{
	//Instantiate the layers, and threads.
	this->dtmfapi_ =  dtmfApi;
	this->dtmfBuffer_ = new DtmfBuffer(DATAGRAM_BUFFER_IN_SIZE,DATAGRAM_BUFFER_OUT_SIZE,FRAME_BUFFER_IN_SIZE,FRAME_BUFFER_OUT_SIZE);
	this->dtmfDatalink_ = new DtmfDatalinkLayer(ADRESS,TOKEN_START);
	this->dtmfPhysical_ = new DtmfPhysical();
	this->dtmfSession_ = new DtmfSession();
	this->i = 0;
	msgBufferDown = this->dtmfBuffer_->apiTransportDown;
	msgBufferUp = this->dtmfBuffer_->transportApiUp;
	//Spawn the backbone thread, this must be the last thing to do. Object is running
	workerThread_ = boost::thread(boost::ref(*this));
}
DtmfBackbone::~DtmfBackbone()
{
	delete dtmfBuffer_;
	delete dtmfDatalink_;
	delete dtmfPhysical_;
	delete dtmfSession_;
	delete portaudioInterface_;
}

//Main dispatching loop
void DtmfBackbone::operator()()
{
	while(true)
	{	
		//----------Primary thresholds-----------
		
		//----------  Physical layer section ----
		//The goal here is to ensure that the physical layer
		//has enough samples, so it never stops playing before all data is "out".
		//At the same time, the frames decoded must be moved away fast enough, 
		//so the audio buffer never overruns.
		
		if((UP_PFRAME_AMOUNT > T_PFRAME_MAX)&&
		   (!this->dtmfBuffer_->physicalDllUp->full()))
		{
			moveFrameIn();
		}
		else if((PFRAME_DOWN_AMOUNT < T_PFRAME_MIN)&&
				(!this->dtmfBuffer_->dllPhysicalDown->empty()))
		{
			moveFrameOut();
		}


		//--------- Data link section --------
		//The datalink layer decodes frames and encodes datagrams. Since the datalink layer has some token handling
		//it must always call decode then encode, in that order. This means that 
		else if(this->dtmfDatalink_->needsAttention()&&this->hasRoomForDatalinkAction())
		{
			dataLinkAction();
		}



		else if((this->dtmfBuffer_->physicalDllUp->size() > T_FRAME_MAX)&& //Too many input frames waiting.
			   (this->hasRoomForDatalinkAction()))
		{
			dataLinkAction();
		}

		else if((this->dtmfBuffer_->dllPhysicalDown->size() < T_FRAME_MIN)&&  //The physical layer will soon need more output frames.
				(!this->dtmfBuffer_->transportDllDown->empty()) &&			  //and there is atleast 1 datagram ready to be encoded
				(this->hasRoomForDatalinkAction())
				)

		{
			dataLinkAction();
		}


		//------------ Network layer section ------------
		else if(this->dtmfBuffer_->dllTransportUp->size() > T_DGRAM_MAX) //Too many datagrams waiting (the dll is about to clog itself)
		{
			decodeDatagram();
		}

		//Messages are waiting to be sent, and the dll is about to run out of work.
		else if((this->dtmfBuffer_->transportDllDown->size() < T_DGRAM_MIN)&&(!this->dtmfBuffer_->apiTransportDown->empty()))
		{
			encodeMessage();
		}
		
		
		//-------------General work ------------
		//If no buffer is above a max threshold, or belov a min threshold, the system is temporarily stable.
		//Therefore messages will be moved through the buffers in any way possible.

		
		
		else
		{
			i++;
			i %= 6;
			int j = 0;
			bool workDone = false;
			while(j<7  && !workDone)
			{
				j++;
				switch((i+j)%6)
				{
				case 0:
					if(!this->dtmfBuffer_->apiTransportDown->empty()
						&& hasRoomForMessageEncode())
					{
						workDone = true;
						encodeMessage();
					}
					break;
				case 1:
					if(!this->dtmfBuffer_->dllPhysicalDown->empty() && ROOM_FOR_PFRAME_DOWN)
					{
						workDone = true;
						moveFrameOut();
					}
					break;
				case 2:
					if(!this->dtmfBuffer_->physicalDllUp->empty() && hasRoomForDatalinkAction())
					{
						workDone = true;
						dataLinkAction();
					}
					break;
				case 3:
					if(UP_PFRAME_AVAILABLE && !this->dtmfBuffer_->physicalDllUp->full())
					{
						workDone = true;
						moveFrameIn();
					}
					break;
				case 4:
					if(!this->dtmfBuffer_->dllTransportUp->empty())
					{
						workDone = true;
						decodeDatagram();
					}
					break;
				default: //case 5:
					if(!this->dtmfBuffer_->transportDllDown->empty() && hasRoomForDatalinkAction())
					{
						dataLinkAction();
					}
					break;
				}
				//else sleep
				if(!workDone)
				{
					boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEPTIME_MSEC));
				}
			}
		}
	}
}



//TODO: add actual encode/decode calls, these will be available when people turn in their code >_>

void DtmfBackbone::dataLinkAction()
{
	this->dtmfDatalink_->decode(
		this->dtmfBuffer_->transportDllDown,
		this->dtmfBuffer_->dllPhysicalDown,
		this->dtmfBuffer_->physicalDllUp, 
		this->dtmfBuffer_->dllTransportUp);
	this->dtmfDatalink_->encode(
		this->dtmfBuffer_->transportDllDown,
		this->dtmfBuffer_->dllPhysicalDown,
		this->dtmfBuffer_->physicalDllUp, 
		this->dtmfBuffer_->dllTransportUp);
}

#ifdef DEBUG
void DtmfBackbone::moveFrameIn()
{
}
void DtmfBackbone::decodeDatagram()
{
}
void DtmfBackbone::encodeMessage()
{
}

void DtmfBackbone::moveFrameOut()
{
}

bool DtmfBackbone::hasRoomForDatalinkAction()
{
	return true;
}
bool DtmfBackbone::hasRoomForMessageEncode()
{
	return true;
}
#endif DEBUG
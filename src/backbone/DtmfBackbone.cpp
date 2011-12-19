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
#ifdef DEBUG
void debugBackboneBuffers(DtmfBuffer * buffer,DtmfPhysical * phys);
#endif DEBUG


//The backbone instantiates the buffers and individual layers, and ends off with launching its own operator thread. 
//Since the api constructs the backbone, a pointer to it, and its message buffers is given as well.
DtmfBackbone::DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferDown,DtmfMsgBuffer *& msgBufferUp, boost::mutex ** callbackMainLoopMutex,bool &hasToken,unsigned char thisAddress)
{
	//Instantiate the layers, and threads.
	callbackMainLoopMutex_ = callbackMainLoopMutex;
	this->dtmfapi_ =  dtmfApi;
	this->dtmfBuffer_ = new DtmfBuffer(DATAGRAM_BUFFER_IN_SIZE,DATAGRAM_BUFFER_OUT_SIZE,FRAME_BUFFER_IN_SIZE,FRAME_BUFFER_OUT_SIZE);
	this->dtmfDatalink_ = new DtmfDataLinkLayer(thisAddress,hasToken);
	this->dtmfPhysical_ = new DtmfPhysical(paFloat32,2,OUTPUT_BUFFER_SIZE,OUTPUT_SAMPLE_RATE,paFloat32,2,INPUT_BUFFER_SIZE,INPUT_SAMPLE_RATE);
	this->dtmfTransport_ = new DtmfTransport();
	this->i = 0;
	msgBufferDown = this->dtmfBuffer_->apiTransportDown;
	msgBufferUp = this->dtmfBuffer_->transportApiUp;
	//Spawn the backbone thread, this must be the last thing to do. Object is running
}
DtmfBackbone::~DtmfBackbone()
{
	delete dtmfBuffer_;
	delete dtmfDatalink_;
	delete dtmfPhysical_;
	delete dtmfTransport_;
}

//Main dispatching loop
void DtmfBackbone::main()
{
	while(continueRunning_)
	{	

		//----------Primary thresholds-----------
		#ifdef DEBUG
		
			debugBackboneBuffers(this->dtmfBuffer_,this->dtmfPhysical_);
			std::cout << "Running" << std::endl;
			std::cout << "Can send ?: " << this->dtmfDatalink_->canTransmit() << std::endl;
			std::cout << "Has token?: " << this->dtmfDatalink_->hasToken << std::endl;
			std::cout << std::endl;
		#endif DEBUG
		//----------  Physical layer section ----
		//The goal here is to ensure that the physical layer
		//has enough samples, so it never stops playing before all data is "out".
		//At the same time, the frames decoded must be moved away fast enough, 
		//so the audio buffer never overruns.
		
		if((this->dtmfPhysical_->receiveBufferSize()> T_PFRAME_MAX)&&
		   (!this->dtmfBuffer_->physicalDllUp->full()))
		{
			
			#ifdef DEBUG
			std::cout << "moving frame in                                   " << std::endl;
			#endif DEBUG
			moveFrameIn();
		}
		else if((this->dtmfPhysical_->sendBufferSize() < T_PFRAME_MIN)&&
				(!this->dtmfBuffer_->dllPhysicalDown->empty())&&
				this->dtmfDatalink_->canTransmit())
		{
			#ifdef DEBUG
			std::cout << "moving frame out                                  " << std::endl << std::endl;
			#endif DEBUG
			moveFrameOut();
			
		}


		//--------- Data link section --------
		//The datalink layer decodes frames and encodes datagrams. Since the datalink layer has some token handling
		//it must always call decode then encode, in that order. This means that 
		else if(this->dtmfDatalink_->needsAttention()&&this->hasRoomForDatalinkAction())
		{
			#ifdef DEBUG
			std::cout << "Datalink action, because needs attention" << std::endl;
			#endif DEBUG
			dataLinkAction();
		}



		else if((this->dtmfBuffer_->physicalDllUp->size() > T_FRAME_MAX)&& //Too many input frames waiting.
			   (this->hasRoomForDatalinkAction()))
		{
			#ifdef DEBUG
			std::cout << "Datalink action, because too many input frames" << std::endl;
			#endif DEBUG
			dataLinkAction();
		}

		else if((this->dtmfBuffer_->dllPhysicalDown->size() < T_FRAME_MIN)&&  //The physical layer will soon need more output frames.
				(!this->dtmfBuffer_->transportDllDown->empty()) &&			  //and there is atleast 1 datagram ready to be encoded
				(this->hasRoomForDatalinkAction())
				)

		{
			
			#ifdef DEBUG
			std::cout << "Datalink action, because too few output frames" << std::endl;
			#endif DEBUG
			dataLinkAction();
		}


		//------------ Network layer section ------------
		else if(this->dtmfBuffer_->dllTransportUp->size() > T_DGRAM_MAX) //Too many datagrams waiting (the dll is about to clog itself)
		{
			
			#ifdef DEBUG
			std::cout << "decoding datagram, because Too many datagrams waiting" << std::endl;
			#endif DEBUG
			decodeDatagram();
		}

		//Messages are waiting to be sent, and the dll is about to run out of work.
		else if((this->dtmfBuffer_->transportDllDown->size() < T_DGRAM_MIN)&&(!this->dtmfBuffer_->apiTransportDown->empty()))
		{
			#ifdef DEBUG
			std::cout << "encoding message, because too few packets" << std::endl;
			#endif DEBUG
			

			encodeMessage();
		}
		
		
		//-------------General work ------------
		//If no buffer is above a max threshold, or belov a min threshold, the system is temporarily stable.
		//Therefore messages will be moved through the buffers in any way possible.

		
		
		else
		{
			#ifdef DEBUG
			std::cout << "Went to general work" << std::endl;
			#endif DEBUG
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
						#ifdef DEBUG
						std::cout << "case 0" << std::endl;
						#endif DEBUG
						workDone = true;
						encodeMessage();
					}
					break;
				case 1:
					if(!this->dtmfBuffer_->dllPhysicalDown->empty() && this->dtmfPhysical_->receiveBufferSize() == 0&&this->dtmfDatalink_->canTransmit())
					{
						#ifdef DEBUG
						std::cout << "case 1" << std::endl;
						#endif DEBUG
						workDone = true;
						moveFrameOut();
					}
					break;
				case 2:
					if(!this->dtmfBuffer_->physicalDllUp->empty() && hasRoomForDatalinkAction())
					{
						#ifdef DEBUG
						std::cout << "case 2" << std::endl;
						#endif DEBUG	
					workDone = true;
						dataLinkAction();
					}
					break;
				case 3:
					if(this->dtmfPhysical_->receiveBufferSize() > 0 && !this->dtmfBuffer_->physicalDllUp->full())
					{
						#ifdef DEBUG
						std::cout << "case 3" << std::endl;
						#endif DEBUG
						workDone = true;
						moveFrameIn();
					}
					break;
				case 4:
					if(!this->dtmfBuffer_->dllTransportUp->empty())
					{
						#ifdef DEBUG
						std::cout << "case 4" << std::endl;
						#endif DEBUG
						workDone = true;
						decodeDatagram();
					}
					break;
				default: //case 5:
					if(!this->dtmfBuffer_->transportDllDown->empty() && hasRoomForDatalinkAction())
					{
						#ifdef DEBUG
						std::cout << "case 5" << std::endl;
						#endif DEBUG
						dataLinkAction();
					}
					break;
				}
				//else sleep
				if(!workDone)
				{
					#ifdef DEBUG
					std::cout << "sleeping" << std::endl;
					#endif DEBUG
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


void DtmfBackbone::moveFrameIn()
{
	this->dtmfPhysical_->receive(this->dtmfBuffer_->physicalDllUp);
}



void DtmfBackbone::decodeDatagram()
{

	this->dtmfTransport_->decode(this->dtmfBuffer_->dllTransportUp,this->dtmfBuffer_->transportApiUp);
	//this->dtmfBuffer_->

	(*(this->callbackMainLoopMutex_))->unlock();
	
}
void DtmfBackbone::encodeMessage()
{
	this->dtmfTransport_->encode(this->dtmfBuffer_->apiTransportDown,this->dtmfBuffer_->transportDllDown);
	//pull message
}

#ifdef DEBUG
#include "../buffers/frame.h"
#endif DEBUG

void DtmfBackbone::moveFrameOut()
{
	//boost::circular_buffer<Frame> skank(*(this->dtmfBuffer_->dllPhysicalDown));

	//this->dtmfPhysical_->send(&skank);
	this->dtmfPhysical_->send(this->dtmfBuffer_->dllPhysicalDown);
}

bool DtmfBackbone::hasRoomForDatalinkAction()
{
	return true;
}
bool DtmfBackbone::hasRoomForMessageEncode()
{
	return true;
}



#ifdef DEBUG
#include <windows.h>
using namespace std;
void gotoxy(short,short);
void debugBackboneBuffers(DtmfBuffer * buffer,DtmfPhysical * phys)
{
	//Message buffers
	gotoxy(0,0);
	cout << "Top layer buffers, Api <-> Transport" << endl;
	cout << "\tapiTransportDown.queueSize: " << buffer->apiTransportDown->queueSize() << " messages" << endl;
	cout << "\ttransportApiUp.queueSize:   " << buffer->transportApiUp->queueSize() << " messages" << endl<<endl;
	cout << "Medium layer buffers, Transport <-> Datalink " << endl;
	cout << "\tdllTransportUp.size:        "   << buffer->dllTransportUp->size() << " char32_t's" << endl;
	cout <<	"\ttransportDllDown.size:      " << buffer->transportDllDown->size() << " packets " << endl<<endl;
	cout << "Low layer buffers, datalink <-> physical " << endl;
	cout << "\tphysicalDllUp.size:         " << buffer->physicalDllUp->size() << " frames" <<endl;
	cout << "\tdllPhysicalDown.size:       " << buffer->dllPhysicalDown->size() << " frames" << endl<<endl;
	cout << "Physical buffers" << endl;
	cout << "\tDown:                       " << phys->sendBufferSize() << endl;
	cout << "\tUp:                         " << phys->receiveBufferSize() << endl << endl;
}
void gotoxy( short x, short y )
{
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { x, y }; 
	SetConsoleCursorPosition( hStdout, position );
}
#endif DEBUG
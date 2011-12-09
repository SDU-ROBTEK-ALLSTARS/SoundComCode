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
//#include "DtmfBackbone.h"
//DtmfBackbone::DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& msgBufferIn,DtmfMsgBuffer *& msgBufferOut )
//{
//	//Instantiate the layers, and threads.
//	this->dtmfapi_ =  dtmfApi;
//	this->dtmfBuffer_ = new DtmfBuffer(DATAGRAM_BUFFER_IN_SIZE,DATAGRAM_BUFFER_OUT_SIZE,FRAME_BUFFER_IN_SIZE,FRAME_BUFFER_OUT_SIZE,DATAGRAM_SIZE,FRAME_SIZE);
//	this->dtmfDatalink_ = new DtmfDatalink();
//	this->dtmfPhysical_ = new DtmfPhysical();
//	this->dtmfSession_ = new DtmfSession();
//	this->portaudioInterface_ = new PortaudioInterface(); //Is this something the physical layers does for me ?
//	this->i = 0;
//
//
//	//Spawn the backbone thread, this must be the last thing to do. Object is running
//	workerThread_ = boost::thread(boost::ref(*this));
//
//
//}
//DtmfBackbone::~DtmfBackbone()
//{
//	delete dtmfBuffer_;
//	delete dtmfDatalink_;
//	delete dtmfPhysical_;
//	delete dtmfSession_;
//	delete portaudioInterface_;
//}
//
////Main dispatching loop
//void DtmfBackbone::operator()()
//{
//	//TODO: follow diagram
//	while(true)
//	{
//
//
//
//
//
//
//		std::cout << "test";
//		//Primary thresholds
//		if((PFRAME_IN > T_PFRAME_MAX)&&(ROOM_FOR_FRAME))
//		{
//			moveFrameIn();
//		}
//		else if((PFRAME_OUT < T_PFRAME_MIN)&&(FRAME_AVAILABLE))
//		{
//			moveFrameOut();
//		}
//		else if((FRAME_IN > T_FRAME_MAX)&&(ROOM_FOR_DATAGRAM))
//		{
//			decodeFrame();
//		}
//		else if((FRAME_OUT < T_FRAME_MIN)&&(DATAGRAM_AVAILABLE))
//		{
//			encodeDatagram();
//		}
//		else if(DGRAM_IN > T_DGRAM_MAX)
//		{
//			decodeDatagram();
//		}
//		else if((DGRAM_OUT < T_DGRAM_MIN)&&(MESSAGE_AVAILABLE))
//		{
//			encodeMessage();
//		}
//		//General work
//		else
//		{
//			i++;
//			i %= 6;
//			int j = 0;
//			bool workDone = false;
//			while(j<7)
//			{
//				j++;
//				switch((i+j)%6 && !workDone)
//				{
//				case 0:
//					if(OUT_MSG_AVAILABLE && ROOM_FOR_RESULT)
//					{
//						workDone = true;
//						encodeMessage();
//					}
//					break;
//				case 1:
//					if(OUT_FRAME_AVAILABLE && ROOM_FOR_RESULT)
//					{
//						workDone = true;
//						moveFrameOut();
//					}
//					break;
//				case 2:
//					if(IN_FRAME_AVAILABLE && ROOM_FOR_RESULT)
//					{
//						workDone = true;
//						decodeFrame();
//					}
//					break;
//				case 3:
//					if(IN_PFRAME_AVAILABLE && ROOM_FOR_IN_FRAME)
//					{
//						workDone = true;
//						moveFrameIn();
//					}
//					break;
//				case 4:
//					if(IN_DGRAM_AVAILABLE && ROOM_FOR_RESULT)
//					{
//						workDone = true;
//						decodeDatagram();
//					}
//					break;
//				default: //case 5:
//					if(OUT_DGRAM_AVAILABLE && ROOM_FOR_RESULT)
//					{
//						encodeDatagram();
//					}
//					break;
//				}
//			//else sleep
//			boost::this_thread::sleep(boost::posix_time::milliseconds(SLEEPTIME_MSEC));
//			}
//
//
//
//
//
//		}
//
//
//	}
//}
//
//
//
////TODO: add actual encode/decode calls
//void DtmfBackbone::moveFrameIn()
//{
//}
//void DtmfBackbone::decodeFrame()
//{
//}
//void DtmfBackbone::decodeDatagram()
//{
//}
//void DtmfBackbone::encodeMessage()
//{
//}
//void DtmfBackbone::encodeDatagram()
//{
//}
//void DtmfBackbone::moveFrameOut()
//{
//}

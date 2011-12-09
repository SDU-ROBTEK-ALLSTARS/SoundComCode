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
# File:     datalinklayer.h
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
#ifndef DATALINKLAYER_H_
#define DATALINKLAYER_H_

//defines needed for data link layer
//#define 	MY_ADDRESS			1
#define		STARTS_WITH_TOKEN
#define     MAX_TIME_WITH_TOKEN	1
#define 	MAX_TIME_OFFERING_TOKEN 1
#define		MAX_TIME_TO_REPLY	1

//debugging
#define		DEBUG								//out comment for no debug info
#define 	DEBUG_OUT			std::cout		//output for debug info

//*****     INCLUDES     *****
#include 	<stdio.h>
#include 	<time.h>
#include 	<boost/circular_buffer.hpp>
#include 	"frame.h"

//*****     DECLARATIONS     *****
struct datalist									//structure for unfinished transmissions
{
	unsigned char dataByte;
	bool flag;
	bool eot;
};

//*****     CLASS DECLARATION     *****
class DataLinkLayer
{
private:
	int MY_ADDRESS;

	datalist frameRecList[8];					//list of received data
	datalist frameSendList[8];					//list of send data
	int currentReceiver;						//datagrams are processed for this receiver (0:3)
	int nextInSendSequence;						//points to the next usable sequence number.
	bool receiverNeedsUpdate;					//set on end of datagram, cleared on receiver update
	bool awaitsReply;							//set on end of transmission, clear when reply is received
	time_t timestampAwaitsReply;				//system time @ end of transmission
	bool tokenAlreadyOffered;					//set on offerToken, clear on passToken
	time_t timestampTokenOffered;				//system time when token was offered
	time_t timestampToken;						//system time when token was received

	bool hasToken;								//1 means have token, 0 means have no token
	int nextStation;							//next station to be offered the token

	boost::circular_buffer< unsigned int > *datagramDown;   //downwards input pointer
	boost::circular_buffer< unsigned int > *datagramUp;   	//upwards output pointer
	boost::circular_buffer< Frame > *frameDown;   			//downwards output pointer
	boost::circular_buffer< Frame > *frameUp;   			//upwards input pointer

	void processFrame(Frame);					//main engine for processing frames into datagrams
	void acceptFrame(Frame);					//inserts frame in list
	void discardFrame();						//discards frame (by doing nothing)

	void processDatagram(unsigned int);			//main engine for processing datagrams into frames
	void makeFrame(unsigned char,unsigned char);//generate frame and put in frame buffer
	void endDatagram();							//sets end of transmission bit in last frame

	void tokenOffered();						//receives token
	bool checkToken();							//returns 0 if token is OK and 1 if time to pass token
	void offerToken();							//offer token to next station
	void passToken();							//passes token to accepting station

	void checkFrameReceiveList();				//check if list is complete
	void clearFrameReceiveList();				//clears list of received data

	void checkFrameSendList();					//check if list is complete
	void clearFrameSendList();					//clears list of send data

	void passDataUpwards();						//writes data into buffer
	void requestResend(int);					//requests resending of data missing in list

	void replyFromReceiver(Frame);				//checks reply from receiver and generates resend
	void resendData(unsigned int);				//resends requested frames

	void fatalError();							//reports critical error

public:
	DataLinkLayer(int,bool);							//default constructor
	void decode(boost::circular_buffer< unsigned int >*,		//decode is called from backbone
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< unsigned int >*);
	void encode(boost::circular_buffer< unsigned int >*,		//encode is called from backbone
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< Frame >*,
				boost::circular_buffer< unsigned int >*);

};
#endif /* DATALINKLAYER_H_ */

//End Of File

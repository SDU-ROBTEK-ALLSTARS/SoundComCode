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
# File: datalinklayer.h
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
#ifndef DATALINKLAYER_H_
#define DATALINKLAYER_H_

//defines needed for data link layer
//#define MY_ADDRESS 1
#define STARTS_WITH_TOKEN
#define MAX_TIME_WITH_TOKEN 20
#define MAX_TIME_OFFERING_TOKEN 20
#define MAX_TIME_TO_REPLY 20

//testing and debugging
#define GENERATE_ERRORS
//#define ERROR_PERCENTAGE 5
#define	DEBUG
#define DEBUG_OUT std::cout //output for debug info

//***** INCLUDES *****
#include <stdio.h>
#include <time.h>
#include <boost/circular_buffer.hpp>
#include "../buffers/frame.h"
#include "../buffers/packet.h"

//***** DECLARATIONS *****
struct datalist //structure for unfinished transmissions
{
unsigned char dataByte;
bool flag;
bool eot;
};

//***** CLASS DECLARATION *****
class DtmfDataLinkLayer
{
private:
//defining this data link layer
int MY_ADDRESS;

//send and receive lists
datalist frameRecList[8]; //list of received data
datalist frameSendList[8]; //list of send data
void checkFrameReceiveList(); //check if list is complete
void clearFrameReceiveList(); //clears list of received data
void checkFrameSendList(); //check if list is complete
void clearFrameSendList(); //clears list of send data

//frame generation
void processDatagram(unsigned int); //main engine for processing datagrams into frames
void makeFrame(unsigned char,unsigned char);//generate frame and put in frame buffer
void endTransmission(); //sets end of transmission bit in last frame
int currentReceiver; //datagrams are processed for this receiver (0:3)
unsigned char datagramIterator; //used to keep track of next datagram byte to pop
int datagramLength; //holds length of current datagram
int nextInSendSequence; //points to the next usable sequence number.

//frame processing
void processFrame(Frame); //main engine for processing frames into datagrams
void acceptFrame(Frame); //inserts frame in list
void discardFrame(); //discards frame (by doing nothing)
void passDataUpwards(int); //writes data into buffer

//flags controlling program flow
bool receiverNeedsUpdate; //set on end of datagram, cleared on receiver update
bool awaitsReply; //set on end of transmission, clear when reply is received
bool dataInSendList; //flag set if there are frames in send list
bool tokenAlreadyOffered; //set on offerToken, clear on passToken
bool replyGenerated; //flag sets when reply is generated and clears on decode
bool noReply; //sets in situations where reply is prohibited
bool resendRequested; //sets on request resend, clear on pass upwards

//timestamps
time_t timestampAwaitsReply; //system time @ end of transmission
time_t timestampTokenOffered; //system time when token was offered
time_t timestampToken; //system time when token was received

//network
int nextStation; //next station to be offered the token
bool eotReceived;
void tokenOffered(); //receives token
bool checkToken(); //returns 0 if token is OK and 1 if time to pass token
void offerToken(); //offer token to next station
void passToken(); //passes token to accepting station
void requestResend(int); //requests resending of data missing in list
void replyFromReceiver(Frame); //checks reply from receiver and generates resend
void resendData(unsigned int); //resends requested frames

//pointers to buffers
boost::circular_buffer< Packet > *datagramDown; //downwards input pointer
boost::circular_buffer< Frame > *frameDown; //downwards output pointer
boost::circular_buffer< Frame > *frameUp; //upwards input pointer
boost::circular_buffer< unsigned int > *datagramUp; //upwards output pointer

//reportd
void fatalError(); //reports critical error

#ifdef GENERATE_ERRORS
void simulateError();
#endif

public:
DtmfDataLinkLayer(int,bool); //construct dll with arguments address and starts with token
void decode(boost::circular_buffer< Packet >*, //decode is called from backbone
boost::circular_buffer< Frame >*,
boost::circular_buffer< Frame >*,
boost::circular_buffer< unsigned int >*);

void encode(boost::circular_buffer< Packet >*, //encode is called from backbone
boost::circular_buffer< Frame >*,
boost::circular_buffer< Frame >*,
boost::circular_buffer< unsigned int >*);

bool needsAttention(); //returns 1 if a timer is due and 0 if not
bool canTransmit(); //returns 1 if hasToken = 1 and 0 if hasToken = 0
bool awaitsReplys();
double lostFrameCount; //increase on discard frame
double successFrameCount; //increase on accept frame


bool hasToken; //1 means have token, 0 means have no token
double generatedError;
int ERROR_PERCENTAGE;
};
#endif /* DATALINKLAYER_H_ */




//End Of File


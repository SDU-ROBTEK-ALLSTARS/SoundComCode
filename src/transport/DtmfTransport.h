/*******************************************************************************
# DtmfProject - 3rd term Robot Systems Engineering, Fall 2011, SDU
#
# Copyright (c) 2011    Alexander Adelholm Brandbyge    <abran09@student.sdu.dk>
#                       Frederik Hagelskjær             <frhag10@student.sdu.dk>
#                       Kent Stark Olsen                <keols09@student.sdu.dk>
#                       Kim Lindberg Schwaner           <kschw10@student.sdu.dk>
#                       Leon Bonde Larsen               <lelar09@student.sdu.dk>
#                       Rudi Hansen                     <rudha10@student.sdu.dk>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
********************************************************************************
# File:       DtmfTransport.h
# Project:    DtmfProject
# Version:
# Platform:   PC/Mac/Linux
# Author:     Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Created:    2011-06-12
********************************************************************************
# Description
#
# The goal of the Transport class is to implement a simple protocol handling
# tasks most often assigned to the transport layer protocols (OSI model).
# That is, process-to-process communication.
#
********************************************************************************/

#ifndef DTMFTRANSPORT_H
#define DTMFTRANSPORT_H

#include <ctime>
#include <queue>
#include <map>
#include <boost/circular_buffer.hpp>
#include "packet.h"
#include "DtmfOutMessage.h"
#include "DtmfInMessage.h"
#include "DtmfMsgBuffer.h"

// Connection status aliases
#define TRANSPORT_STATUS_SYNC_MASTER     1
#define TRANSPORT_STATUS_SYNC_MASTER_S2  6
#define TRANSPORT_STATUS_SYNC_SLAVE      2
#define TRANSPORT_STATUS_CONNECTED       3
#define TRANSPORT_STATUS_DISCONNECTED    4
#define TRANSPORT_STATUS_RESET           5

// Max number of ports. Warning: Packet object not compatible with more than 255.
#define TRANSPORT_NUM_PORTS            255

// Number of reserved ports ranging from 0 to TRANSPORT_NUM_RES_PORTS-1.
#define TRANSPORT_NUM_RES_PORTS        20

class DtmfTransport
{
private:
    static bool sPortsInUse_[TRANSPORT_NUM_PORTS];
    unsigned char port_;
    unsigned int connStatus_;

    // Maximum number of unacknowledged packets that
    // will be accumulated before an EAK packet is sent
    unsigned char maxNumUnack_;

    // The sequence number last used
    unsigned char lastSeqNumber_;

    // Re-send time out settings
    double retransTimeout;
    unsigned int maxRetrans;
    struct timedPacket {
        Packet* packet__;
        std::time_t timedPacketStamp__;
    };

    // For compability with the API layer (DtmfOutMessage
    // are friends with us)
    void toPacketsFromApi(DtmfOutMessage *, std::deque<Packet *> *);

    // To/from char-array buffer and Packet object
    Packet packetFromCharBuffer(boost::circular_buffer<unsigned int> *);
    void packetToCharBuffer(boost::circular_buffer<unsigned char> *, Packet);


public:
    DtmfTransport();
    ~DtmfTransport();

    // To use the transport connection, it
    // must be configured with:
    void setPort(const unsigned int newPort=0);

    // Accessors
    unsigned char port() const;
    bool isPortSet(const unsigned int enteredPort=0) const;
    bool *getPortTable() const;
    int status() const;

    // Public data I/O


    std::map<unsigned char,Packet *> packsToApi_;
    // Map containers for unacknowledged sent- or received
    // packets. They are mapped as a pair of sequence
    // number and packet object pointer address.
    std::map<unsigned char,Packet *> sentUnAckPackets_;
    std::map<unsigned char,Packet *> recvUnAckPackets_;

    // TODO make private
    void processOutgoingPacket(std::deque<Packet *> *inQueue,
                               std::deque<Packet *> *outQueue);
    void processReceivedPacket(std::deque<Packet *> *inQueue,
                               std::deque<Packet *> *outQueue);


};
#endif //DTMFTRANSPORT_H

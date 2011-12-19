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
# File: packet.h
# Project: DtmfProject
# Version:
# Platform: PC/Mac/Linux
# Author: Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Created: 2011-06-12
********************************************************************************
# Description
#
# The goal of the Transport class is to implement a simple protocol handling
# tasks most often assigned to the transport layer protocols (OSI model).
# That is, process-to-process communication.
#
********************************************************************************/
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <ctime>
#include <queue>
#include <boost/circular_buffer.hpp>
#include "../buffers/packet.h"
#include "../buffers/DtmfOutMessage.h"
#include "../buffers/DtmfInMessage.h"
#include "../DtmfMsgBuffer.h"

// Connection status aliases
#define TRANSPORT_STATUS_SYNC_WAIT 1
#define TRANSPORT_STATUS_CONNECTED 2
#define TRANSPORT_STATUS_DISCONNECTED 3
#define TRANSPORT_STATUS_RESET 4

#define TRANSPORT_NUM_PORTS 256 // Max number of ports. Warning:
// Packet object not compatible with more than 256.
#define TRANSPORT_NUM_RES_PORTS 20 // Number of reserved ports ranging
                                            // from 0 to TRANSPORT_NUM_RES_PORTS-1.

class DtmfTransport {
 private:
    static bool sPortsInUse_[TRANSPORT_NUM_PORTS];
    unsigned char port_;
    unsigned int connStatus_; // Connection status I.D.

    unsigned char lastInSequence_; // Last received packet that was in sequence.
    unsigned int maxNumOutstanding; // Max number of packet which will be sent
                                     // without getting an acknowledgment.
    unsigned int retransmissionTimeout;
    unsigned int nullTimeout; //Timeout value (in millisecs) for sending a
                              // NUL packet (data-less) if there is nothing else to send

    //For compability with the API layer (DtmfOutMessage are friends with us)
    void toPacketFromApi(DtmfOutMessage *);

    // Retransmission
    double retransTimeout; //Timeout value (in millisecs) before retransmission of an un-acknowledged packet
    unsigned int maxRetrans; //Max number of consecutive retransmissions that will be attempted
    unsigned char timedPacketSeq_; //The sequence number of a timed packet
    std::time_t timedPacketStamp_; //Timestamp of timed packet

    // Internal sender queues
    std::deque<Packet*> outQueue_;
    std::deque<Packet*> sentUnAckPackets_; //Packets must be kept until they are acknowledged

    // Internal receiver queues
    std::deque<Packet*> inQueue_; //To store on API call
    std::priority_queue<Packet *, std::vector<Packet *>, Packet::orderBySeq> recvOutOfSeqPackets_; //Packets received out-of-sequence

    // To/from char-array buffer and Packet object
    Packet packetFromCharBuffer(boost::circular_buffer<unsigned int> *);
    void packetToCharBuffer(boost::circular_buffer<unsigned char> *, Packet);

    void init(); // TODO initializes certain vars

    Packet processIncPacket(Packet);

 public:
    DtmfTransport();
    ~DtmfTransport();

    // To use the transport connection, it
    // must be configured with:
    void setPort(const unsigned int newPort=0);

    //To open a connection one must use
    void connect(const unsigned char destPort);

    //keep-alive function?

    //Closes all connections
    void closeAll();

    // Accessors
    unsigned char port() const;
    bool isPortSet(const unsigned int enteredPort=0) const;
    bool *getPortTable() const;
    int connStatus() const;

    // Public data I/O
    void decode(boost::circular_buffer<unsigned int> *DllTransportUp,
                DtmfMsgBuffer *TransportApiUp);

    void encode(DtmfMsgBuffer *ApiTransportDown,
                boost::circular_buffer<Packet> *TransportDllDown);
};
#endif //TRANSPORT_H


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
# File:       DtmfTransport.cpp
# Project:    DtmfProject
# Version:
# Platform:   PC/Mac/Linux
# Author:     Kim Lindberg Schwaner <kschw10@student.sdu.dk>
# Created:    2011-06-12
********************************************************************************
# Description
#
#
#
# General shortcomings/Todo
#
# - One DtmfTransport will only correctly function while communication
#   with ONE DtmfTransport (meaning throwing it packets to different
#   ports will screw things up. A solution would be to outsource i/o
#   packet handling/decision making to another class, and then instantiate
#   one per target being sent to.
#
********************************************************************************/

#include <ctime>
#include <queue>
#include <map>
#include <boost/circular_buffer.hpp>
#include "exception.h"
#include "DtmfTransport.h"
#include "packet.h"
#include "DtmfOutMessage.h"
#include "DtmfInMessage.h"
#include "DtmfMsgBuffer.h"

/****************************************
#                                       #
#          Core functionality           #
#                                       #
*****************************************/

// Table of reserved ports.
bool DtmfTransport::sPortsInUse_[TRANSPORT_NUM_PORTS] = {false};

// Default constructor does not init more than necessary; see make().
DtmfTransport::DtmfTransport()
{
    port_ = 0;
    connStatus_ = TRANSPORT_STATUS_DISCONNECTED;
    maxNumUnack_ = 5;
}

DtmfTransport::~DtmfTransport()
{
    sPortsInUse_[port_] = false;
}
// Gives a certain instance of the class a port number and reserves
// it, making it unavailable to other class instances. If an
// invalid port number is supplied through the methods argument
// automatic port assignment will be attempted. The user should
// check to see if the port actually assigned was the one wished for.
void DtmfTransport::setPort(const unsigned int newPort)
{
    if ((newPort >= TRANSPORT_NUM_RES_PORTS) && (newPort < TRANSPORT_NUM_PORTS) && (!sPortsInUse_[newPort])) {
        sPortsInUse_[newPort] = true;
        port_ = newPort;
    } else {
        // If a port was not supplied, or the supplied port
        // is not available, look for one that is.
        for (int i=TRANSPORT_NUM_RES_PORTS; i<TRANSPORT_NUM_PORTS; i++) {
            if (!sPortsInUse_[i]) {
                sPortsInUse_[i] = true;
                port_ = (unsigned char) i;
                break;
            }
        }
    }
    if (!port_) {
        throw Exception("Transport.setPort() No ports available.");
        return;
    }
}


// Returns this instances port number.
unsigned char DtmfTransport::port() const
{
    return port_;
}

// True if port number has been set correctly. Can be used to
// self-test. If a port number is supplied through the argument,
// that number will be checked. If no argument is supplied,
// the instance's own port number is checked.
bool DtmfTransport::isPortSet(const unsigned int enteredPort) const
{
    if ((enteredPort) && (enteredPort < TRANSPORT_NUM_PORTS)) {
        if (sPortsInUse_[enteredPort]) {
            return true;
        } else {
            return false;
        }
    } else {
        if ((port_) && (sPortsInUse_[port_])) {
            return true;
        } else {
            return false;
        }
    }
}

// Returns a pointer to the array that contains reserved ports.
bool *DtmfTransport::getPortTable() const
{
    bool *pointer = sPortsInUse_;
    return pointer;
}

// Returns current connection status.
int DtmfTransport::status() const
{
    return connStatus_;
}

//
//
//
//
//
void DtmfTransport::processOutgoingPacket(std::deque<Packet *> *inQueue,
                                          std::deque<Packet *> *outQueue)
{
    // NOTE we may want to call this so sync or whatnot without
    // actually sending data....
    /*if (inQueue->empty()) {
    #ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" <<  (int)port_ << ")  ERROR: down() called with no data." << std::endl;
    #endif
        return;
    }*/
    if (connStatus_ == TRANSPORT_STATUS_DISCONNECTED) {
        // We're disconnected and must open a connection
        // by sending a SYN packet (1st step of handshake).
        unsigned char destPort = inQueue->front()->destPort();
        Packet *synPacket = new Packet; // TODO de-alloc
        bool flags[8] = {false};  // IMPORTANT: Init to false
        flags[PACKET_FLAG_SYN] = true;
        synPacket->make(port_,
                        destPort,
                        flags,
                        0,
                        0); // seq#, ack# and data are not supplied for SYN
        outQueue->push_front(synPacket);
        connStatus_ = TRANSPORT_STATUS_SYNC_MASTER;
        lastSeqNumber_ = synPacket->seqNumber();
        sentUnAckPackets_.clear();
        sentUnAckPackets_[synPacket->seqNumber()] = synPacket;
        //sentUnAckPackets_.insert(std::pair<unsigned char,Packet *>(synPacket->seqNumber(),synPacket));
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Initiating connection with "
                  << (int)synPacket->destPort()
                  << " (seq: " << (int)synPacket->seqNumber() << ")..." << std::endl;
#endif
        return;
    } else if (connStatus_ == TRANSPORT_STATUS_SYNC_MASTER) {
        // We are waiting for an acknowledgment of a recently sent SYN
        // packet and do not do anything.
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Cannot process out-packet (waiting for sync)..." << std::endl;
#endif
        return;
    } else if (connStatus_ == TRANSPORT_STATUS_SYNC_SLAVE) {
        // A SYN packet has been received and we must acknowledge
        // it. Note no data gets attached in order to establish
        // connections quickly.
        if (recvUnAckPackets_.size() == 1) {
            Packet *ackSynPacket = new Packet;  // TODO de-alloc
            bool flags[8] = {false};
            flags[PACKET_FLAG_SYN] = true;
            flags[PACKET_FLAG_ACK] = true;
            ackSynPacket->make(port_,
                               (*recvUnAckPackets_.begin()).second->sourcePort(),
                               flags,
                               0,
                               (*recvUnAckPackets_.begin()).first);
            lastSeqNumber_ = ackSynPacket->seqNumber();
            outQueue->push_front(ackSynPacket);
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Acknowledging connection with "
                      << (int)(*recvUnAckPackets_.begin()).second->sourcePort()
                      << " (seq: " << (int)ackSynPacket->seqNumber() << ")..." << std::endl;
#endif
            sentUnAckPackets_[ackSynPacket->seqNumber()] = ackSynPacket;
            recvUnAckPackets_.erase(recvUnAckPackets_.begin());
            return;
        } else {
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ")  ERROR: hurr" << std::endl;
#endif
            return;
        }
    } else if (connStatus_ == TRANSPORT_STATUS_CONNECTED) {
        // TODO
        if (recvUnAckPackets_.size() >= maxNumUnack_) {
            // We have too many unacknowledged packets and
            // must send an EAK packet.
            Packet *eakPacket = new Packet; // TODO de-alloc mem
            bool flags[8] = {false};
            flags[PACKET_FLAG_EAK] = true;
            flags[PACKET_FLAG_CHK] = true;
            unsigned char dataLength = (unsigned char)recvUnAckPackets_.size();
            unsigned char *data = new unsigned char[dataLength]; // TODO de-alloc memory
            std::map<unsigned char,Packet *>::iterator it = recvUnAckPackets_.begin();
            int i = 0;
            while(it != recvUnAckPackets_.end()) {
                data[i] = (*it).first;
                i++;
                it++;
            }
            eakPacket->make(port_,
                            (*recvUnAckPackets_.begin()).second->sourcePort(),
                            flags,
                            (lastSeqNumber_ + 1),
                            0,
                            dataLength,
                            data);
            lastSeqNumber_ = lastSeqNumber_+1;
            outQueue->push_back(eakPacket);
            sentUnAckPackets_[eakPacket->seqNumber()] = eakPacket;
            recvUnAckPackets_.clear();
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Sending EAK packet acknowledging ";
            for (int j=0; j<dataLength; j++) {
                DEBUG_OUT << (int)data[i] << ", ";
            }
            DEBUG_OUT << std::endl;
#endif
            return;
        } else if (!inQueue->empty()) {
            // Send a completely normal ACK+data
            Packet *packetOut = inQueue->front();
            inQueue->pop_front();
            packetOut->setFlag(PACKET_FLAG_CHK);
            packetOut->setSourcePort(port_);
            if (!recvUnAckPackets_.empty()) {
                packetOut->setFlag(PACKET_FLAG_ACK);
                packetOut->setAckNumber((*recvUnAckPackets_.begin()).first);
                recvUnAckPackets_.erase(recvUnAckPackets_.begin());
            }
            packetOut->setSeqNumber(lastSeqNumber_+1);
            lastSeqNumber_ = lastSeqNumber_+1;
            sentUnAckPackets_[packetOut->seqNumber()] = packetOut;
            outQueue->push_back(packetOut);
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" <<  (int)port_ << ") Sending data packet "
                      << "(seq: " << (int)packetOut->seqNumber() << ", ack: " << (int)packetOut->ackNumber() << ")..." << std::endl;
#endif
            return;
        } else {
            //
            return;
        }
    } else {
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ")  ERROR: Status unknown!" << std::endl;
#endif
    }
}

//
//
//
//
//
void DtmfTransport::processReceivedPacket(std::deque<Packet *> *inQueue,
                                          std::deque<Packet *> *outQueue)
{
    if (inQueue->empty()) {
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ")  ERROR: up() called with no data." << std::endl;
#endif
        return;
    }
    Packet *packIn = inQueue->front();
    inQueue->pop_front();  // TODO place elsewhere?
/*
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" <<  (int)port_ << ") Received packet being processed:" << std::endl;
        DEBUG_OUT << "Source" << (char)9 << (int)packIn->sourcePort()    << (char)9 << (char)9 << "SYN" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_SYN) << std::endl;
        DEBUG_OUT << "Dest" << (char)9 << (int)packIn->destPort()        << (char)9 << (char)9 << "ACK" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_ACK) << std::endl;
        DEBUG_OUT << "Seq#" << (char)9 << (int)packIn->seqNumber()       << (char)9 << (char)9 << "NUL" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_NUL) << std::endl;
        DEBUG_OUT << "Ack#" << (char)9 << (int)packIn->ackNumber()       << (char)9 << (char)9 << "RST" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_RST) << std::endl;
        DEBUG_OUT << "Length" << (char)9 << (int)packIn->totalLength()   << (char)9 << (char)9 << "EAK" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_EAK) << std::endl;
        DEBUG_OUT << "CRC ok" << (char)9 << (int)packIn->checksumValid() << (char)9 << (char)9 << "CHK" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_CHK) << std::endl;
#endif
*/
    if ((!packIn->checksumValid()) || (packIn->destPort() != port_)) {
        // Discard silently
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Packet discarded." << std::endl;
#endif
        return;
    }
    if (connStatus_ == TRANSPORT_STATUS_DISCONNECTED) {
        if (packIn->flagSet(PACKET_FLAG_SYN)) {
            // Someone else is initiating a connection with us (2nd step
            // of handshake).
            connStatus_ = TRANSPORT_STATUS_SYNC_SLAVE;
            recvUnAckPackets_.clear();
            recvUnAckPackets_[packIn->seqNumber()] = packIn;
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") " << (int)packIn->sourcePort() << " is initiating a connection with me..." << std::endl;
#endif
            return;
        } else {
            // If we do not receive a SYN packet first, to open
            // a connection, we ignore whatéver comes our way.
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Packet from " << (int)packIn->sourcePort() << " ignored (connection not sync'd)." << std::endl;
#endif
            return;
        }
    } else if (connStatus_ == TRANSPORT_STATUS_SYNC_MASTER) {
        // This packet should contain the acknowledgment number
        // of the SYN packet initially sent. Additionally it
        // should contain the other node's randomly chosen sequence nr.
        if (packIn->flagSet(PACKET_FLAG_SYN)) {
            // 3rd step of handshake.
            connStatus_ = TRANSPORT_STATUS_CONNECTED;
            recvUnAckPackets_[packIn->seqNumber()] = packIn;
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Connection established with " << (int)packIn->sourcePort() << "." << std::endl;
#endif
            return;
        } else {
            // If we do not receive a SYN packet first, to open
            // a connection, we ignore whatéver comes our way.
#ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Packet from " << (int)packIn->sourcePort() << " ignored (connection not sync'd)." << std::endl;
#endif
            return;
        }
        return;
    } else if (connStatus_ == TRANSPORT_STATUS_SYNC_SLAVE) {
        // This packet should acknowledge the step 2 SYN packet
        // sent by SLAVE. Other than that it's a normal ACK packet.
        // TODO
        recvUnAckPackets_[packIn->seqNumber()] = packIn;

        connStatus_ = TRANSPORT_STATUS_CONNECTED;
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ") Connection established with " << (int)packIn->sourcePort() << std::endl;
#endif
        return;
    } else if (connStatus_ == TRANSPORT_STATUS_CONNECTED) {
        //TODO
        return;
    } else {
#ifdef DEBUG
        DEBUG_OUT << "***TRANSPORT (" << (int)port_ << ")  ERROR: Status unknown!" << std::endl;
#endif
    }
}


/****************************************
#                                       #
#             Compatibility             #
#                                       #
*****************************************/

// Converts a DtmfOutMessage object (from API) to Packet
// objects, and places them in a queue.
// When receiving a message object from the API, there exists
// two scenarios: The data may just fit into one Packet, or
// it will have to be split up between several packets.
// We create as many packets as needed to move the data.
void DtmfTransport::toPacketsFromApi(DtmfOutMessage *msg, std::deque<Packet *> *outQueue)
{
    if ((msg->dataLength_ > 0) && (msg->dataLength_ <= (PACKET_TLEN - PACKET_HLEN))) {
        Packet *packetOut = new Packet;
        packetOut->setDestPort(msg->rcvPort_);
        packetOut->setRecvAddr(msg->rcvAddress_);
        packetOut->insertData(msg->data_, (unsigned char)msg->dataLength_, false);  // no need to checksum it _yet_
        outQueue->push_back(packetOut);
    } else if (msg->dataLength_ > (PACKET_TLEN - PACKET_HLEN)) {

        unsigned int numPacketsNeeded = msg->dataLength_ / (PACKET_TLEN - PACKET_HLEN) + 1;
        unsigned int lengthLast = msg->dataLength_ % (PACKET_TLEN - PACKET_HLEN);

        for (unsigned int i=0; i<numPacketsNeeded; i++) {

            Packet *packetOut = new Packet;
            packetOut->setDestPort(msg->rcvPort_);
            packetOut->setRecvAddr(msg->rcvAddress_);

            int dataOffset = i*(PACKET_TLEN - PACKET_HLEN);

            if (i == (numPacketsNeeded-1)) {  // Last packet of the set
                packetOut->insertData(&msg->data_[dataOffset], lengthLast, false);
                outQueue->push_back(packetOut);
            } else {
                packetOut->insertData(&msg->data_[dataOffset], (PACKET_TLEN - PACKET_HLEN), false);  // no need to checksum it _yet_
                outQueue->push_back(packetOut);
            }
        }
    } else {
        return;  // There was no data in message.
    }
}

// For compability with data link layer
// Returns a Packet object from chars contained in a circular buffer. It is
// supposed, that the buffer is operated in a "FIFO"-like way, so that the data
// gotten from buffer.front() is the data first inserted (with push_back()).
// Data is not deleted from the buffer until packet has been made in case
// there was an error.
Packet DtmfTransport::packetFromCharBuffer(boost::circular_buffer<unsigned int> *buffer)
{
    Packet packet;

    if (buffer->size() >= PACKET_HLEN) {
        // Get header off of buffer (PACKET_HLEN long as defined in packet.h).
        unsigned char header[PACKET_HLEN];
        for (unsigned char i=0; i<PACKET_HLEN; i++) {
            header[i] = (unsigned char)buffer->at(i);
        }

        // Examine the total packet length (4th header byte) to get any data it might contain.
        unsigned char totalLength = header[3];
        unsigned char *data;

        if ((totalLength > PACKET_HLEN) && ((unsigned char)buffer->size() > (totalLength-PACKET_HLEN))) {
            unsigned char dataLength = header[3]-PACKET_HLEN;
            data = new unsigned char[dataLength];
            for (unsigned char i=0; i<dataLength; i++) {
                data[i] = (unsigned char)buffer->at(i+PACKET_HLEN);  // Data is off-set by PACKET_HLEN.
            }
        } else {
            throw Exception("assemblePacketFromCharBuffer() Buffer does not contain a complete data array");
            return packet; //empty packet
        }

        // Both header and data was succesfully collected.
        packet.makeFromArrays(header, data);
        buffer->erase_begin(totalLength);
        delete data;
        return packet;
    } else {
        throw Exception("assemblePacketFromCharBuffer() Buffer does not contain a complete header");
        return packet;  // Empty packet.
    }
}

// Places a Packet object as a char array in a boost circular buffer
// if there is room for it.
void DtmfTransport::packetToCharBuffer(boost::circular_buffer<unsigned char> *buffer, Packet packet)
{
    unsigned char *packetAsArray = packet.getAsArray();
    int availableBufferSpace = buffer->capacity() - buffer->size();
    if (availableBufferSpace >= packet.totalLength()) {
        for (int i=0; i<packet.totalLength(); i++) {
            buffer->push_back(packetAsArray[i]);
        }
    } else {
        throw Exception("packetToCharBuffer() not enough available space in buffer");
    }
}




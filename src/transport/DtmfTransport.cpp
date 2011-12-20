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
# File:       packet.h
# Project:    DtmfProject
# Version:
# Platform:   PC/Mac/Linux
# Author:     Kim Lindberg Schwaner            <kschw10@student.sdu.dk>
# Created:    2011-06-12
********************************************************************************
# Description
#
#
********************************************************************************/

#include <ctime>
#include <queue>
#include <map>
#include <algorithm>
#include <iostream>
#include <boost/circular_buffer.hpp>
#include "exception.h"
#include "DtmfTransport.h"
#include "packet.h"
#include "DtmfOutMessage.h"
#include "DtmfInMessage.h"
#include "DtmfMsgBuffer.h"

// Table of reserved ports.
bool DtmfTransport::sPortsInUse_[TRANSPORT_NUM_PORTS] = {false};

// Default constructor does not init more than necessary; see make().
DtmfTransport::DtmfTransport() {
    port_ = 0;
    connStatus_ = TRANSPORT_STATUS_DISCONNECTED;
}

DtmfTransport::~DtmfTransport() {
    sPortsInUse_[port_] = false;
}

// Converts a DtmfOutMessage object (from API) to Packet
// objects, and places them in the outQueue_.
// When receiving a message object from the API, there exists
// two scenarios: The data may just fit into one Packet, or
// it will have to be split up between several packets.
// We create as many packets as needed to move the data.
void DtmfTransport::toPacketsFromApi(DtmfOutMessage *msg) {

    if ((msg->dataLength_ > 0) && (msg->dataLength_ <= (PACKET_TLEN - PACKET_HLEN))) {
        Packet packetOut;
        packetOut.setDestPort(msg->rcvPort_);
        packetOut.setRecvAddr(msg->rcvAddress_);
        packetOut.insertData(msg->data_, (unsigned char)msg->dataLength_, false);  // no need to checksum it _yet_
        outQueue_.push_back(&packetOut);
    }
    else if (msg->dataLength_ > (PACKET_TLEN - PACKET_HLEN)) {

        unsigned int numPacketsNeeded = msg->dataLength_ / (PACKET_TLEN - PACKET_HLEN) + 1;
        unsigned int lengthLast = msg->dataLength_ % (PACKET_TLEN - PACKET_HLEN);

        for (unsigned int i=0; i<numPacketsNeeded; i++) {

            Packet *packetOut = new Packet;
            packetOut->setDestPort(msg->rcvPort_);
            packetOut->setRecvAddr(msg->rcvAddress_);

            int dataOffset = i*(PACKET_TLEN - PACKET_HLEN);

            if (i == (numPacketsNeeded-1)) {  // Last packet of the set
                packetOut->insertData(&msg->data_[dataOffset], lengthLast, false);
                outQueue_.push_back(packetOut);
            }
            else {
                packetOut->insertData(&msg->data_[dataOffset], (PACKET_TLEN - PACKET_HLEN), false);  // no need to checksum it _yet_
                outQueue_.push_back(packetOut);
            }
        }
    }
    else {
        return;  // There was no data in message.
    }
}

// For compability with data link layer
// Returns a Packet object from chars contained in a circular buffer. It is
// supposed, that the buffer is operated in a "FIFO"-like way, so that the data
// gotten from buffer.front() is the data first inserted (with push_back()).
// Data is not deleted from the buffer until packet has been made in case
// there was an error.
Packet DtmfTransport::packetFromCharBuffer(boost::circular_buffer<unsigned int> *buffer) {
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
        }
        else {
            throw Exception("assemblePacketFromCharBuffer() Buffer does not contain a complete data array");
            return packet; //empty packet
        }

        // Both header and data was succesfully collected.
        packet.makeFromArrays(header, data);
        buffer->erase_begin(totalLength);
        delete data;
        return packet;
    }
    else {
        throw Exception("assemblePacketFromCharBuffer() Buffer does not contain a complete header");
        return packet;  // Empty packet.
    }
}

// Places a Packet object as a char array in a boost circular buffer
// if there is room for it.
void DtmfTransport::packetToCharBuffer(boost::circular_buffer<unsigned char> *buffer, Packet packet) {
    unsigned char *packetAsArray = packet.getAsArray();
    int availableBufferSpace = buffer->capacity() - buffer->size();
    if (availableBufferSpace >= packet.totalLength()) {
        for (int i=0; i<packet.totalLength(); i++) {
            buffer->push_back(packetAsArray[i]);
        }
    }
    else {
        throw Exception("packetToCharBuffer() not enough available space in buffer");
    }
}

// Gives a certain instance of the class a port number and reserves
// it, making it unavailable to other class instances. If an
// invalid port number is supplied through the methods argument
// automatic port assignment will be attempted. The user should
// check to see if the port actually assigned was the one wished for.
void DtmfTransport::setPort(const unsigned int newPort) {
    if ((newPort >= TRANSPORT_NUM_RES_PORTS) && (newPort < TRANSPORT_NUM_PORTS) && (!sPortsInUse_[newPort])) {
        sPortsInUse_[newPort] = true;
        port_ = newPort;
    }
    else {
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
    init();  // Initializes certain vars.
}

// Opens a connection by sending a SYN packet.
void DtmfTransport::connect(const unsigned char destPort) {
    if (destPort && port_) {
        Packet *synPacket = new Packet;
        bool flags[8] = {false};  // IMPORTANT: Init to false
        flags[PACKET_FLAG_SYN] = true;
        flags[PACKET_FLAG_CHK] = true;
        synPacket->make(port_,     // sourceport (send from self)
                        destPort,  // destport
                        flags,
                        0,         // seq nr not filled for SYN
                        0);        // ack nr not filled for SYN

        outQueue_.push_front(synPacket);  // We always want it in front of queue
    }
}

// UNDONE Initializes variable parameters.
void DtmfTransport::init() {
    retransTimeout = 5;  // Seconds.
    maxRetrans = 3;      // Max number of times.
}

// Returns this instances port number.
unsigned char DtmfTransport::port() const {
    return port_;
}

// True if port number has been set correctly. Can be used to
// self-test. If a port number is supplied through the argument,
// that number will be checked. If no argument is supplied,
// the instance's own port number is checked.
bool DtmfTransport::isPortSet(const unsigned int enteredPort) const {
    if ((enteredPort) && (enteredPort < TRANSPORT_NUM_PORTS)) {
        if (sPortsInUse_[enteredPort]) {
            return true;
        }
        else {
            return false;
        }
    }
    else {
        if ((port_) && (sPortsInUse_[port_])) {
            return true;
        }
        else {
            return false;
        }
    }
}

// Returns a pointer to the array that contains reserved ports.
bool *DtmfTransport::getPortTable() const {
    bool *pointer = sPortsInUse_;
    return pointer;
}

// Returns current connection status.
int DtmfTransport::connStatus() const {
    return connStatus_;
}

// Main public up-stream packet processing interface. This is called by the
// backbone when there is incoming data available to Transport from
// the underlying layer.
//
// Every time decode is called it gets data out of any unacknowledged packets
// stored in a map container by sequence number key. This way we can get it in
// the correct order.
// DLL -> Transport -> API
void DtmfTransport::decode(boost::circular_buffer<unsigned int> *DllTransportUp,
                           DtmfMsgBuffer *TransportApiUp) {
    if (DllTransportUp->empty()) {
        // If the buffer is empty, why are we here? :o
        return;
    }
    Packet packIn = packetFromCharBuffer(DllTransportUp); // THIS MAY HAVE TO FILTER OUT STUFFING IF THATS NEEDED
    if ((port_ == packIn.destPort()) && (packIn.checksumValid()) && (packIn.totalLength())) {  // check validity
        processReceivedPacket(&packIn);
        if (!recvUnAckPackets_.empty()) {
            std::map<unsigned char,Packet *>::iterator it;
            if ((*recvUnAckPackets_.begin()).first == (lastSeqToApi_+1)) {
                for (it = recvUnAckPackets_.begin(); it != recvUnAckPackets_.end(); it++) {
                    // HACK ? we make one DtmfInMessage per Packet, altough making just
                    // one big message would perhaps be smarter.
                    DtmfInMessage *msgToApi = new DtmfInMessage(0, (*it).second->sourcePort(), (*it).second->dataLength(), (*it).second->data()); // I don't know the sender address?
                    TransportApiUp->pushMsg((char *) msgToApi);  // TODO is this correct? ask rudi
                    lastSeqToApi_ = (*it).second->seqNumber();  // Set the last sequence nr that was passed to API
                }
            }
            // Some packets must be out of sequence. Wait for the rest to fill the hole(s)
        }
        // There are no packets mapped to recvUnAckPackets_
    }
    #ifdef DEBUG
    else {
        DEBUG_OUT << "***TRANSPORT: Invalid packet discarded";
    }
    #endif
}

// Main public down-stream processing interface. This is called by the
// backbone when there is incoming data available to Transport from
// the above layer.
// API -> Transport -> DLL
void DtmfTransport::encode(DtmfMsgBuffer *ApiTransportDown, //change buffer type
                           boost::circular_buffer<Packet> *TransportDllDown) {
 
    while (ApiTransportDown->queueSize() > 0) {
        DtmfOutMessage *msg = (DtmfOutMessage*) ApiTransportDown->pullMsg();
        toPacketsFromApi(msg);  // Packets made are accessable via outQueue_<Packet*>
    }

    if (connStatus_ == TRANSPORT_STATUS_CONNECTED) {



    }
    else if (connStatus_ == TRANSPORT_STATUS_DISCONNECTED) {
        // Connect to port as specified in info from API
        connect(outQueue_.front()->destPort());
        sentUnAckPackets_.insert(std::pair<unsigned char,Packet *>(outQueue_.front()->seqNumber(),outQueue_.front()));
        #ifdef DEBUG
            DEBUG_OUT << "***TRANSPORT: Attempting connection with " << (int)outQueue_.front()->destPort() << std::endl;
        #endif
        TransportDllDown->push_back(*outQueue_.front());
        outQueue_.pop_front();
        // TODO free mem
    }
}

// Called when an incoming packet object is ready for
// processing.
// This function will check against maps
// for unacknowledged sequence numbers and delete and
// create new entries as needed.
// DOES NOT CHECK PACKET VALIDITY
void DtmfTransport::processReceivedPacket(Packet *packIn) {
    #ifdef DEBUG
        DEBUG_OUT << std::endl << "***TRANSPORT: Received packet being processed" << std::endl;
        DEBUG_OUT << "Source:" << (char)9 << (int)packIn->sourcePort() << std::endl;
        DEBUG_OUT << "Dest:" << (char)9 << (int)packIn->destPort() << std::endl;
        DEBUG_OUT << "SYN:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_SYN) << std::endl;
        DEBUG_OUT << "ACK:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_ACK) << std::endl;
        DEBUG_OUT << "NUL:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_NUL) << std::endl;
        DEBUG_OUT << "RST:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_RST) << std::endl;
        DEBUG_OUT << "EAK:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_EAK) << std::endl;
        DEBUG_OUT << "CHK:" << (char)9 << (int)packIn->flagSet(PACKET_FLAG_CHK) << std::endl;
        DEBUG_OUT << "Seq#:" << (char)9 << (int)packIn->seqNumber() << std::endl;
        DEBUG_OUT << "Ack#:" << (char)9 << (int)packIn->sourcePort() << std::endl;
        DEBUG_OUT << "Length:" << (char)9 << (int)packIn->totalLength() << std::endl;
        DEBUG_OUT << "CRC ok:" << (char)9 << (int)packIn->checksumValid() << std::endl;
    #endif
    //
    // SYN packet
    //
    if (packIn->flagSet(PACKET_FLAG_SYN)) {
        if (connStatus_ == TRANSPORT_STATUS_DISCONNECTED) {
            // Someone else is initiating a connection with us.
            recvUnAckPackets_.clear();
            recvUnAckPackets_.insert(std::pair<unsigned char,Packet *>(packIn->seqNumber(),packIn));
            connStatus_ = TRANSPORT_STATUS_SYNC_WAIT;
            //remoteSeq_ = packIn->seqNumber();  // TODO ?
            //lastInSequence_ = packIn->seqNumber();
            //sentUnAckPackets_.insert(std::pair<unsigned char,Packet *>(packIn->seqNumber(),packIn));//d
            //std::map<unsigned char,Packet *>::iterator it = sentUnAckPackets_.find(packIn->seqNumber());//d
            //DEBUG_OUT << (int) (*it).second->destPort();//d
            #ifdef DEBUG
                DEBUG_OUT << "***TRANSPORT: Incoming connection from " << (int)packIn->sourcePort() << std::endl;
            #endif
            return;
        }
        else if (connStatus_ == TRANSPORT_STATUS_SYNC_WAIT) {
            // We have recently sent a SYN packet ourselves
            // and this is the response.
            if (packIn->flagSet(PACKET_FLAG_ACK)) {
                std::map<unsigned char,Packet *>::iterator it = sentUnAckPackets_.find(packIn->ackNumber());
                if (it != sentUnAckPackets_.end()) {
                    sentUnAckPackets_.erase(it);
                    recvUnAckPackets_.insert(std::pair<unsigned char,Packet *>(packIn->seqNumber(),packIn));
                    //connStatus_ = TRANSPORT_STATUS_CONNECTED;
                    //remoteSeq_ = packIn->seqNumber();  // TODO ?
                    #ifdef DEBUG
                        DEBUG_OUT << "***TRANSPORT: Handshaking (1) with " << (int)packIn->sourcePort() << std::endl;
                    #endif
                }
                // ACK number sent with packet is not a key in the un-ack map
            }
            // ACK invalid or incoming ack-number does not fit with any in our map.
        }
        // SYN flag set, but packet not valid in some way.
    }
    //
    // ACK packet
    //
    else if (packIn->flagSet(PACKET_FLAG_ACK)) {
        if (connStatus_ == TRANSPORT_STATUS_CONNECTED) {
            // A "normal" ACK packet has been received.
            std::map<unsigned char,Packet *>::iterator it = sentUnAckPackets_.find(packIn->ackNumber());
            if (it != sentUnAckPackets_.end()) {
                sentUnAckPackets_.erase(it);
                recvUnAckPackets_.insert(std::pair<unsigned char,Packet *>(packIn->seqNumber(),packIn));
                #ifdef DEBUG
                    DEBUG_OUT << "***TRANSPORT: ACK packet (Seq: " << (int)packIn->seqNumber() << ", Ack: " << (int)packIn->ackNumber() << ") from " << (int)packIn->sourcePort() << std::endl;
                #endif
                return;
            }
            // ACK number sent with packet is not a key in the un-ack map
        }
        // status is not CONNECTED :(
    }
    //
    // - packet
    //

}

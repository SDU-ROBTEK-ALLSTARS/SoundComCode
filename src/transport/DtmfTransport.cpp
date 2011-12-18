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
#include <boost/circular_buffer.hpp>
#include "exception.h"
#include "DtmfTransport.h"
#include "packet.h"
#include "DtmfOutMessage.h"
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
void DtmfTransport::toPacketFromApi(DtmfOutMessage *msg) {

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

            Packet packetOut;
            packetOut.setDestPort(msg->rcvPort_);
            packetOut.setRecvAddr(msg->rcvAddress_);

            int dataOffset = i*(PACKET_TLEN - PACKET_HLEN);

            if (i == (numPacketsNeeded-1)) {  // Last packet of the set
                packetOut.insertData(&msg->data_[dataOffset], lengthLast, false);
                outQueue_.push_back(&packetOut);
            }
            else {
                packetOut.insertData(&msg->data_[dataOffset], (PACKET_TLEN - PACKET_HLEN), false);  // no need to checksum it _yet_
                outQueue_.push_back(&packetOut);
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

// UNDONE
// This method is called upon to decide an appropiate answer
// to a received packet. It returns the answer as a packet
// object. If the answer is "silence", the packet returned
// will have length zero.
Packet DtmfTransport::processIncPacket(Packet incPacket) {
    Packet packet;
    return packet;
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
        Packet synPacket;
        bool flags[8] = {false};  // IMPORTANT: Init to false
        flags[PACKET_FLAG_SYN] = true;
        flags[PACKET_FLAG_CHK] = true;
        synPacket.make(port_,     // sourceport (send from self)
                       destPort,  // destport
                       flags,
                       0,         // seq nr not filled for SYN
                       0);        // ack nr not filled for SYN

        outQueue_.push_back(&synPacket);
    }
    return;
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
        if (sPortsInUse_[enteredPort])
            return true;
        else
            return false;
    }
    else {
        if ((port_) && (sPortsInUse_[port_]))
            return true;
        else
            return false;
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
// TODO
// * Negotiable parameters sent with the SYN package in it's otherwise empty data field
// * Time-out
// * Connection reset
// * Out-of-sequence handling: Discard? Queue for later?
// * Max un-ack packets counter before reset
void DtmfTransport::decode(boost::circular_buffer<unsigned int> *DllTransportUp,
                       boost::circular_buffer<unsigned char> *TransportApiUp) {
    if (DllTransportUp->empty()) {
        // If the buffer is empty, why are we here? :o
        return;
    }

    Packet packIn = packetFromCharBuffer(DllTransportUp); // THIS MAY HAVE TO FILTER OUT STUFFING IF THATS NEEDED


    if ((port_ == packIn.destPort()) && (packIn.checksumValid()) && (!packIn.totalLength())) {  // note dest port check

        if (packIn.flagSet(PACKET_FLAG_SYN)) {
            // A SYN packet has been received: Someone is
            // establishing a connection with us.
            lastInSequence_ = packIn.seqNumber();  // tjek?
            connStatus_ = TRANSPORT_STATUS_CONNECTED;

            // The answer is an ACK packet; lets make that. Currently we don't know if
            // there is data to put in it, so we fill in the rest and queue it for sending.
            Packet packOut;
            bool flags[8];
            flags[PACKET_FLAG_ACK] = true;
            flags[PACKET_FLAG_CHK] = true;
            packOut.make(port_, packIn.sourcePort(), flags, lastInSequence_++, lastInSequence_);

            outQueue_.push_back(&packOut);
        }

        if (packIn.flagSet(PACKET_FLAG_ACK)) {

            // TODO
        }

        // We have prepared an answer for next out-going packet, but we
        // still need to pass data (if any) on to the upper layer
        if (packIn.data()) {
            unsigned char *data = packIn.data();
            for (int i=0; i<packIn.dataLength(); i++ ) {
                TransportApiUp->push_back(data[i]);  // THIS HAS NO ADDRESS IN IT
            }
        }
    }
    //else package is not for us, checksum wrong or length=0 (invalid packet)
}

// Main public down-stream processing interface. This is called by the
// backbone when there is incoming data available to Transport from
// the above layer.
void DtmfTransport::encode(DtmfMsgBuffer *ApiTransportDown,
                       boost::circular_buffer<Packet> *TransportDllDown) {
    if (connStatus_ == TRANSPORT_STATUS_CONNECTED) {


        while (ApiTransportDown->queueSize() > 0) {
            DtmfOutMessage *msg = (DtmfOutMessage*) ApiTransportDown->pullMsg();
            toPacketFromApi(msg);
        }  // while (ApiTransportDown->queueSize() > 0)

    }  // connStatus_ == TRANSPORT_STATUS_CONNECTED
    else if (connStatus_ == TRANSPORT_STATUS_SYNC_WAIT) {

    }
    else if (connStatus_ == TRANSPORT_STATUS_DISCONNECTED) {
        // If we're disconnected, see if a SYN packet is queued for sending
        if (!outQueue_.size() > 1) {
            outQueue_.front()->flagSet(PACKET_FLAG_SYN);

        }
    }
    else {
        return;
    }
}


// API -> DtmfMsgBuffer -> TRANSPORT -> outQueue_(Packet) -> DLL
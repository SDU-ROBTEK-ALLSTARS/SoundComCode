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
********************************************************************************/
#ifndef PACKET_H
#define PACKET_H

// Flag bit positions.
#define PACKET_FLAG_SYN 0
#define PACKET_FLAG_ACK 1
#define PACKET_FLAG_NUL 2
#define PACKET_FLAG_RST 3
#define PACKET_FLAG_EAK 4
#define PACKET_FLAG_CHK 7

#define PACKET_HLEN 8 - 1    // Header length (in bytes).
// UNDONE length defs
#define PACKET_DLEN 248 - 1  // Data length (in bytes).
#if (PACKET_DLEN + PACKET_HLEN) > 255
 #error "Packet lengths in-correctly set."
#endif
#define PACKET_TLEN = PACKET_DLEN + PACKET_HLEN

class Packet {
 private:
    // Receiver address to pass to the data link layer.
    unsigned char recvAddr_;

    // These are the data members of the
    // packet as defined in the documentation.
    unsigned char sourcePort_;
    unsigned char destPort_;
    unsigned char flags_;
    unsigned char length_;
    unsigned char seqNumber_;
    unsigned char ackNumber_;
    unsigned short int checksum_;
    unsigned char *data_;

    // Calculates and returns checksum value based on the current data members.
    unsigned short int calcChecksum() const;

 public:
    Packet();
    ~Packet();

    // This is a "lazy constructor".
    void make(const unsigned char sourcePort,
              const unsigned char destPort,
              const bool          flagArray[8],  // Bool array of 8, each representing a flag
              const unsigned char seqNumber,
              const unsigned char ackNumber,
              const unsigned char dataLength=0,  // Max 
                    unsigned char data[]=0);

    // DEPRECATED Make Packet from char arrays.
    void makeFromArrays(unsigned char header[PACKET_HLEN],
                        unsigned char data[]);

    // Single-element insertions.
    void setRecvAddr(const unsigned char recvAddr);
    void setDestPort(const unsigned char destPort);
    void insertData(       unsigned char data[],
                     const unsigned char dataLength,
                     const bool          checksum=true);
    void setFlag(const int);  // Sets one flag

    // Packet sorting by sequence number (intended for use with priority_queue).
    // UNDONE
    struct orderBySeq {
        bool operator()(const Packet&, const Packet&) const;
    };

    // DEPRECATED Get a packet as a (pointer to) char array.
    unsigned char *getAsArray() const;

    // Accessor methods:
    unsigned char operator[](const unsigned char) const;  // Get one byte from packet
                                                          // as if it was a char array.
    unsigned char recvAddr() const;
    unsigned char totalLength() const;
    unsigned char sourcePort() const;
    unsigned char destPort() const;
    unsigned char seqNumber() const;
    unsigned char ackNumber() const;
    bool flagSet(const int) const;
    unsigned char flags() const;
    unsigned char *data() const;
    unsigned char dataLength() const;
    bool checksumValid() const;
};
#endif //PACKET_H

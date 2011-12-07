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
# File:     transport.h
# Project:  DtmfProject
# Version:  1.0
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# The goal of the Transport class is to implement a simple protocol handling
# tasks most often assigned to the transport layer protocols (OSI model).
# That is, process-to-process communication.
#
# Max ports available: 256
#
********************************************************************************/
#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <boost/circular_buffer.hpp>
#include "datagram.h"

class Transport
{
private:
	static bool sPortList_[256] = {false};
	unsigned char port_;

public:
	Transport();

	Transport(unsigned char);

	~Transport();

	void setPort(unsigned char);

	unsigned char getPort();

	void decode(boost::circular_buffer<unsigned char> *ApiTransportDown,
	            boost::circular_buffer<Datagram> *TransportDllDown,
				boost::circular_buffer<unsigned char> *DllTransportUp,
				boost::circular_buffer<unsigned char> *TransportApiUp);
	            
	void encode(boost::circular_buffer<unsigned char> *ApiTransportDown,
	            boost::circular_buffer<Datagram> *TransportDllDown,
				boost::circular_buffer<unsigned char> *DllTransportUp,
				boost::circular_buffer<unsigned char> *TransportApiUp);
};
#endif //TRANSPORT_H
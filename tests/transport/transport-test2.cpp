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
# File:     transport-test.cpp
# Project:  DtmfProject
# Version:  
# Platform:	PC/Mac/Linux
# Author:	Kim Lindberg Schwaner			<kschw10@student.sdu.dk>
# Created:  2011-29-11
********************************************************************************
# Description
#
# 
********************************************************************************/

#include <iostream>
#include <iomanip>
#include <boost/circular_buffer.hpp>
#include "exception.h"
#include "packet.h"



int main()
{
	try
	{
		/**************/


		Packet packet;
		
		unsigned char data[248] = {1,2,5};
		data[246] = 9;
		data[247] = 11;

		bool flags[8] = {false}; //IMPORTANT: Init to false
		flags[PACKET_FLAG_SYN] = true;
		flags[PACKET_FLAG_CHK] = true;
		packet.make(15,//sourceport (send from self)
		            16,//destport
		            flags,
		            0,//seq nr not filled for SYN
		            0,//ack nr not filled for SYN
		            248,//datalength
		            data);//no data
		

		std::cout << "Total length: " << (int)packet.totalLength() << "\n";
		std::cout << "Data length: " << (int)packet.dataLength() << "\n";

		std::cout << "packet[255]: " << (int)packet[255] << "\n";


		for (unsigned char i=0; i<packet.totalLength(); i++) {
			std::cout << (int)i << "-" << (int)packet[i] << "; ";
		}

		std::cout << "\n\n";


		unsigned char *packetAsArray = packet.getAsArray();
		for (unsigned char i=0; i<packet.totalLength(); i++) {

			std::cout << (int)packetAsArray[i] << " ";
		}




		/**************/
	}
	catch (std::exception const &e)
	{
		std::cerr << "EXCEPTION: " << e.what() << std::endl;
	}

	return 0;
}
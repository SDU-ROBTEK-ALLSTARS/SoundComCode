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
# File:     CircularCharArrayBuffer.h
# Project:  DtmfBackbone
# Version:  1.0
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
# A buffer class, designed to contain an array of char arrays, together with 
# pointers allowing it to mimic the functionality of a circular buffer.
*******************************************************************************/

//Remove later
#include <iostream>
#include <string>
#include <sstream>



#ifndef CircularCharArrayBuffer_H
#define CircularCharArrayBuffer_H

class CircularCharArrayBuffer
{
private:
	char ** data_;
	int ** size_;
	int slots_;
	int slotSize_;
	int count_;
	int it_back_;

public:
	CircularCharArrayBuffer(int slotSize, int slots);
	~CircularCharArrayBuffer();
	int Slots();
	int SlotSize();
	int count();
	int free();
	
	void back(char *& data, int *& size); //updates size, and data to point to the char array.
	void front(char *& data, int *& size);
	void advanceBack();
	void advanceFront();
	std::string statusstring();

};
#endif CircularCharArrayBuffer_H
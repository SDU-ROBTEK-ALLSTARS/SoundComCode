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
# File:     CircularCharArrayBuffer.cpp
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
# This is an implementation of the class CircularCharArrayBuffer
#
*******************************************************************************/

#include "CircularCharArrayBuffer.h"

CircularCharArrayBuffer::CircularCharArrayBuffer(int slotSize, int slots)
{
	this->slots_ = slots;
	this->slotSize_ = slotSize;
	this->count_ = 0;
	this->it_back_ =0;
		
	this->data_ = new char* [slots];
	this->size_ = new int * [slots];
	
	for(int i = 0; i < slots_; i++)
	{
		this->data_[i] = new char[slotSize_];
		this->size_[i] = new int();
	}
}

CircularCharArrayBuffer::~CircularCharArrayBuffer()
{
	for(int i = 0; i < slots_; i++)
	{
		delete[] this->data_[i];
		delete this->size_[i];
	}
	delete[] this->data_;
	delete[] this->size_;
}
int CircularCharArrayBuffer::Slots()
{
	return slots_;
}
int CircularCharArrayBuffer::SlotSize()
{
	return slotSize_;
}
int CircularCharArrayBuffer::count()
{
	return this->count_;
}
int CircularCharArrayBuffer::free()
{
	return ((this->slots_)-(this->count_));
}

void CircularCharArrayBuffer::back(char *& data, int *& size)
{
	data  = data_[it_back_];
	size  = size_[it_back_];
}
void CircularCharArrayBuffer::front(char *& data, int *& size)
{
	data  = data_[(it_back_+count_)%slots_];
	size  = size_[(it_back_+count_)%slots_];
}
void CircularCharArrayBuffer::advanceBack()
{
	if(count_>0)
	{
		it_back_++;
		it_back_%=this->slots_;
		count_--;
	}	
	else
	{
		std::cout << "Advancing across ending of circular array... OH NOES" << std::endl;
	}
}
void CircularCharArrayBuffer::advanceFront()
{
	if(count_ < slots_-1)
	{
		count_++;
	}
	else
	{
		
		std::cout << "Advancing across start of circular array... OH NOES" << std::endl;
	}

}



std::string CircularCharArrayBuffer::statusstring()
{   
	std::stringstream ss;
	ss << "count: " ;
	ss << (this->count_);
	ss << " it_back: "; 
	ss << (this->it_back_); 
	ss << " slotsize: " ;
	ss <<  (this->slotSize_);
	ss << " slots: ";
	ss << (this->slots_);
	return ss.str();
}
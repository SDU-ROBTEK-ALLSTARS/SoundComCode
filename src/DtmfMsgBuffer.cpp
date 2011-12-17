#include "DtmfMsgBuffer.h"

DtmfMsgBuffer::DtmfMsgBuffer()
{
}
DtmfMsgBuffer::~DtmfMsgBuffer(){}
bool DtmfMsgBuffer::empty()
{
	if(0 < queueSize())
		return false;
	return true;
}
unsigned int DtmfMsgBuffer::queueSize()
{
	unsigned int size;
	this->accessMutex.lock();
	size = this->buffer_.size();
	this->accessMutex.unlock();
	return size;
}
void DtmfMsgBuffer::pushMsg(char * inChar)
{
	// Lock mutex
	this->accessMutex.lock();
	this->buffer_.push_back(inChar);
	this->accessMutex.unlock();
}
char * DtmfMsgBuffer::pullMsg()
{
	char * outChar;
	this->accessMutex.lock();
	if (0 < this->buffer_.size())
	{
		outChar = this->buffer_.front();
		this->buffer_.pop_front();
	}
	this->accessMutex.unlock();
	return outChar;
}
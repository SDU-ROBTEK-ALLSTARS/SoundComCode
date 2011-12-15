#include "DtmfOutMessage.h"
#include "../api/DtmfApi.h"

DtmfOutMessage::DtmfOutMessage(DtmfApi * apiClass)
{
	this->addedToQue = this->processed = false;
	this->dataLength_ = 0;
	this->creatorApi = apiClass;
	this->data_ = NULL;
}
DtmfOutMessage::~DtmfOutMessage()
{
	if (this->data_ != NULL)
	{
		delete(this->data_);
	}
}
void DtmfOutMessage::send()
{
	this->addedToQue = true;
	this->creatorApi->msgSendCallback_(this);
}
void DtmfOutMessage::dispose()
{
	if (this->processed)
	{
		this->creatorApi->msgDispose_(this);
		delete(this); //commit suicide
	}
	// The message will be deleted automatically by transportlayer
}
void DtmfOutMessage::setData(unsigned char * data, unsigned long startAddress, unsigned long dataLength)
{
	unsigned char * newData; // New needed data mem pointer
	// Calculate size of new mem allocation
	unsigned int neededSpace = startAddress + dataLength;
	if (neededSpace < this->dataLength_)
		neededSpace = this->dataLength_;

	// Allocate
	newData = new unsigned char[neededSpace];

	// Any existing data?
	if (this->data_)
	{
		// Copy old data to new mem
		memcpy(newData,this->data_,this->dataLength_);

		// Delete old mem
		delete(this->data_);
	}

	// Copy new data
	memcpy((newData+startAddress),data, (size_t)dataLength);

	// Set new length
	this->dataLength_ = neededSpace;

	// Update pointer
	this->data_ = newData;
}
void DtmfOutMessage::setAddress(unsigned char rcvAddress, unsigned char rcvPort)
{
	this->rcvAddress_ = rcvAddress;
	this->rcvPort_ = rcvPort;
}
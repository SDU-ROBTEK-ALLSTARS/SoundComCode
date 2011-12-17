#include "DtmfInMessage.h"

DtmfInMessage::DtmfInMessage(unsigned char senderAddress, unsigned char senderPort, unsigned long dataLength, unsigned char * data) : senderAddress_(senderAddress), senderPort_(senderPort), dataLength_(dataLength), data_(data){}
DtmfInMessage::~DtmfInMessage()
{
	delete(this->data_);
}
unsigned char DtmfInMessage::getSenderAddress()
{
	return this->senderAddress_;
}
void DtmfInMessage::getData(unsigned char *& data, unsigned long startAddress, unsigned long stopAddress)
{
	// Valid address ?
	if ((startAddress >= stopAddress) || ((stopAddress-startAddress) > this->dataLength_))
		return;

	// Copy data to provided pointer
	std::memcpy(data, (this->data_+startAddress),(stopAddress-startAddress));
}
void DtmfInMessage::getData(unsigned char *& data)
{
	// Copies all data to provided data area (Be aware to allocate enough space!)
	std::memcpy(data, this->data_, this->dataLength_);
}
unsigned long DtmfInMessage::getMessageLength()
{
	return this->dataLength_;
}
unsigned char DtmfInMessage::getSenderPort()
{
	return this->senderPort_;
}
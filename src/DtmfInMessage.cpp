#include "DtmfInMessage.h"

//DtmfInMessage::DtmfInMessage();
DtmfInMessage::DtmfInMessage(unsigned char senderAddress, unsigned char senderPort, unsigned long dataLength, unsigned char * data) : senderAddress_(senderAddress), senderPort_(senderPort), dataLength_(dataLength), data_(data){}
DtmfInMessage::~DtmfInMessage()
{
	delete(this->data_);
}
//unsigned char DtmfInMessage::getSenderAdress();
//void DtmfInMessage::getData(unsigned char *& data, unsigned long startAddress, unsigned long stopAddress);
void DtmfInMessage::getData(unsigned char *& data)
{
	data = this->data_; 
}
unsigned long DtmfInMessage::getMessageLength()
{
	return this->dataLength_;
}
unsigned char DtmfInMessage::getSenderPort()
{
	return this->senderPort_;
}
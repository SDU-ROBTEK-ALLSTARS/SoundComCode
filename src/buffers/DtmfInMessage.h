#ifndef DTMFINMESSAGE_H
#define DTMFINMESSAGE_H

#include <cstring>

class DtmfInMessage{
protected:
	unsigned char senderAddress_;
	unsigned char senderPort_;
	unsigned long dataLength_;
	unsigned char * data_;
	DtmfInMessage(unsigned char senderAddress, unsigned char senderPort, unsigned long dataLength, unsigned char * data);
	~DtmfInMessage();
public:
    unsigned char getSenderAddress();
	void getData(unsigned char *& data, unsigned long startAddress, unsigned long stopAddress);
	void getData(unsigned char *& data);
	unsigned long getMessageLength();
	unsigned char getSenderPort();
};
#endif DTMFINMESSAGE_H;
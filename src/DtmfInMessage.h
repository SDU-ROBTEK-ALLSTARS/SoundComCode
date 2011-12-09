#ifndef DTMFINMESSAGE_H
#define DTMFINMESSAGE_H

class DtmfInMessage{
protected:
	unsigned char senderAddress_;
	unsigned char senderPort_;
	unsigned long dataLength_;
	unsigned char * data_;
public:
	DtmfInMessage();
	DtmfInMessage(unsigned char senderAddress, unsigned char senderPort, unsigned long dataLength, unsigned char * data) : senderAddress_(senderAddress), senderPort_(senderPort), dataLength_(dataLength), data_(data){}
	~DtmfInMessage()
	{
		delete(this->data_);
	}
    unsigned char getSenderAdress();
	void getData(unsigned char *& data, unsigned long startAddress, unsigned long stopAddress);
	void getData(unsigned char *& data){ data = this->data_; }
	unsigned long getMessageLength(){return this->dataLength_;}
	unsigned long getSenderPort(){return this->senderPort_;}
};
#endif DTMFINMESSAGE_H;
#ifndef DTMFOUTMESSAGE_H
#define DTMFOUTMESSAGE_H

class DtmfApi;

class DtmfOutMessage
{
	friend class DtmfApi;
	friend class DtmfBackbone;
private:
protected:
	unsigned char senderAddress_;
	unsigned char senderPort_;
	unsigned long dataLength_; // Only length og data in * data_
	unsigned char * data_;
	DtmfApi * creatorApi; // Reference to the API instance who created this message.
public:
	//Skal kun kunne instantieres fra API
	DtmfOutMessage(DtmfApi * apiClass);
	void send();
	void setData(char * data, unsigned long startAdress, unsigned long dataLength);
	void setAddress(unsigned char rcvAddress);
	void dispose();
};
#endif DTMFOUTMESSAGE_H

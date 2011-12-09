#ifndef DTMFOUTMESSAGE_H
#define DTMFOUTMESSAGE_H

class DtmfApi;

class DtmfOutMessage
{
private:
protected:
	bool isSend;
	DtmfApi * createrApi;
public:
	//Skal kun kunne instantieres fra API
	DtmfOutMessage(DtmfApi * apiClass);
	void send();
	void setData(char * data, unsigned long startAdress, unsigned long dataLength);
	void setAddress(unsigned char rcvAddress);
	void dispose();
};
#endif DTMFOUTMESSAGE_H

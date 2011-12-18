#ifndef DTMFOUTMESSAGE_H
#define DTMFOUTMESSAGE_H

#include <string> // Needed for memcpy

class DtmfApi;

class DtmfOutMessage
{
	friend class DtmfApi;
	friend class DtmfBackbone;
    friend class DtmfTransport;
private:
protected:
	bool addedToQue, processed;
	unsigned char rcvAddress_;
	unsigned char rcvPort_;
	unsigned long dataLength_; // Only length og data in * data_
	unsigned char * data_;
	DtmfApi * creatorApi; // Reference to the API instance who created this message.
	DtmfOutMessage(DtmfApi * apiClass); // Can only be created from friend classes.
	~DtmfOutMessage(); // Can only be deleted by friend class or it self.
public:
	void send();
	void setData(unsigned char * data, unsigned long startAddress, unsigned long dataLength); // Copies data from unsigned * char to message. Data can be appended by using startAddress.
	void setAddress(unsigned char rcvAddress, unsigned char rcvPort);
	void dispose(); // Can be used only on non send messages.
};
#endif DTMFOUTMESSAGE_H

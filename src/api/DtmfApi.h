#ifndef DTMFAPI_H
#define DTMFAPI_H
#include <list>
#include "DtmfCallbackThread.h"
#include "dummyBackbone.h"

#include <iostream>

// forward declarations
class DtmfCallback;
class DtmfMsgBuffer;
class DtmfOutMessage;
class DtmfBackbone;

class DtmfApi
{
	friend class DtmfOutMessage;
private:
protected:
	// Threads
	DtmfCallbackThread * callbackThread_;
	DtmfBackbone * backboneThread_;
	boost::mutex * callbackMainLoopMutex_;
	// Pointers to top level buffers (initiated by backbone class)
	DtmfMsgBuffer * apiTransportDown_, * transportApiUp_;
	// Address and list of messages
	unsigned char myAddress_;
	std::list<DtmfOutMessage *> dtmfOutMessages_; // Using a linked list, as this element is only used for holding msg pointers until ~destuct
	void cleanUpMessages_();

	// Methods used by messages to callback 
	void msgSendCallback_(DtmfOutMessage * dtmfOutMessage); // Messages calls this method with its pointer as argument.
	void msgDispose_(DtmfOutMessage * dtmfOutMessage);
public:
	DtmfApi(unsigned char myAddress, bool hasToken);
	~DtmfApi();
	void servicePort(unsigned char port, DtmfCallback * callbackMethod);
	DtmfApi::DtmfOutMessage * newMessage();
	void sendMessage(unsigned char rcvAddress, unsigned char rcvPort, unsigned char * data, unsigned long dataLength);
};

#endif DTMFAPI_H
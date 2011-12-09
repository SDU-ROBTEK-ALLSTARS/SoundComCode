#ifndef DTMFAPI_H
#define DTMFAPI_H

// CLEAN ME UP AREA
//#include "DtmfInMessage.h"
//#include "DtmfThread.h"
//#include "DtmfCallback.h"
//#include "dummyBuffer.h"
//#include "dummyMsgBuffer.h"
//#include "apiToTransport.h"

//class DtmfCallbackThread;
//class DtmfBackbone;
//class DtmfOutMessage;
// END CLEAN ME UP

// This is the main header file, and includes all necessary headers.
#include <list>
#include "DtmfOutMessage.h"
#include "DtmfCallbackThread.h"
#include "dummyBackbone.h"

#include <iostream>

// forward declarations
class DtmfCallback;
class DtmfMsgBuffer;

class DtmfApi
{
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
public:
	DtmfApi(unsigned char myAddress);
	~DtmfApi();
	void servicePort(unsigned char port, DtmfCallback * callbackMethod);
	DtmfOutMessage * newMessage();
	void sendMessage(unsigned char rcvAddress, char * data, unsigned long dataLength);
	void msgSendCallback();
	friend void DtmfOutMessage::send();
};

#endif DTMFAPI_H
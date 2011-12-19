#ifndef DTMFCALLBACKTHREAD_H
#define DTMFCALLBACKTHREAD_H

//#include "DtmfApi.h"
#include "DtmfThread.h"
#include <map>
#include "DtmfMsgBuffer.h"

// Forward declarations
//class DtmfMsgBuffer;
class DtmfCallback;
class DtmfInMessage;

class DtmfCallbackThread : public DtmfThread
{
private:
protected:
	void main();
	boost::mutex callbackMainLoopMutex_; 
	DtmfMsgBuffer * transportApiUp_;
	std::map<unsigned char, DtmfCallback *> serviceCallback_;
	void callback(unsigned char port, DtmfInMessage *& inMessage);
public:
	DtmfCallbackThread(DtmfMsgBuffer *& transportApiUp, boost::mutex *& callbackMainLoopMutex);
	~DtmfCallbackThread();
	void addServicePort(unsigned char port, DtmfCallback * callbackMethod);
	void clearServicePort(unsigned char port);
};
#endif DTMFCALLBACKTHREAD_H
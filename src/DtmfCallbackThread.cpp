#include "DtmfCallbackThread.h"
#include "DtmfCallback.h"
#include "buffers/DtmfInMessage.h"

DtmfCallbackThread::DtmfCallbackThread(DtmfMsgBuffer *& transportApiUp, boost::mutex *& callbackMainLoopMutex) : transportApiUp_(transportApiUp)
{
	// Initialize mutex
	//this->callbackMainLoopMutex_.initialize();
	callbackMainLoopMutex = &(this->callbackMainLoopMutex_);
	// Save buffer pointer
	this->transportApiUp_ = transportApiUp;
}
DtmfCallbackThread::~DtmfCallbackThread()
{
	// This need to be explicit called here also - elsewise, the thread can manage to lock before the variable is read again.
	this->continueRunning_ = false;
	// Unlock mutex to allow the thread to exit
	this->callbackMainLoopMutex_.unlock();
}
void DtmfCallbackThread::main()
{
	// Loop
	DtmfInMessage * dtmfInMessage; // Current message
	while (this->continueRunning_)
	{
		// Loop trough new messages
		while (0 < this->transportApiUp_->queueSize())
		{
			// Get pointer to message
			dtmfInMessage = (DtmfInMessage*)this->transportApiUp_->pullMsg();

			// Send to defined callback method
			this->callback(dtmfInMessage->getSenderPort(),dtmfInMessage);

			// Deallocate memory
			delete(dtmfInMessage);
		}

		// Lock mutex
		this->callbackMainLoopMutex_.lock();
	}
}
void DtmfCallbackThread::callback(unsigned char port, DtmfInMessage *& inMessage)
{
	// Make sure this is not a null pointer.
	if (this->serviceCallback_[port] != NULL)
	{
		// Call the callbackmethod
		this->serviceCallback_[port]->callbackMethod(inMessage);
	}
}
void DtmfCallbackThread::addServicePort(unsigned char port, DtmfCallback * callbackMethod)
{
	this->serviceCallback_[port] = callbackMethod;
}
void DtmfCallbackThread::clearServicePort(unsigned char port)
{
	this->serviceCallback_.erase(port);
}

#include "DtmfCallbackThread.h"
#include "DtmfCallback.h"
#include "DtmfInMessage.h"

DtmfCallbackThread::DtmfCallbackThread(DtmfMsgBuffer *& transportApiUp, boost::mutex *& callbackMainLoopMutex) : transportApiUp_(transportApiUp)
{
	// Initialize mutex
	this->callbackMainLoopMutex_.initialize();
	callbackMainLoopMutex = &(this->callbackMainLoopMutex_);
	// Get buffer pointer
	this->transportApiUp_ = transportApiUp;
}
DtmfCallbackThread::~DtmfCallbackThread()
{
	// Unlock mutex to allow the thread to exit
	this->callbackMainLoopMutex_.unlock();
}
void DtmfCallbackThread::main()
{
	// Loop
	DtmfInMessage * dtmfInMessage; // Current message
	unsigned char * data;
	while (this->continueRunning_)
	{
		// Operations...
		//boost::this_thread::sleep(boost::posix_time::seconds(1)); // Loop control

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
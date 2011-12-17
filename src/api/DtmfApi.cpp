#include "DtmfApi.h"
#include "DtmfOutMessage.h" // Work around on a circular include problem with DtmfApi and DtmfOutMessage

DtmfApi::DtmfApi(unsigned char myAddress, bool hasToken) : myAddress_(myAddress)
{
	// Start backbone
	this->backboneThread_ = new DtmfBackbone(this,this->apiTransportDown_,this->transportApiUp_, &(this->callbackMainLoopMutex_));
	this->callbackThread_ = new DtmfCallbackThread(this->transportApiUp_, this->callbackMainLoopMutex_);
}
DtmfApi::~DtmfApi()
{
	// Clean up messages
	this->cleanUpMessages_();

	// Stop threads
	delete(this->callbackThread_);
	delete(this->backboneThread_);
}
void DtmfApi::servicePort(unsigned char port, DtmfCallback * callbackMethod)
{
	// Tell the callback thread to listen on the new port.
	this->callbackThread_->addServicePort(port,callbackMethod);
}
void DtmfApi::cleanUpMessages_()
{
	// Run through all messages, and make sure they are deleted.
	for (unsigned int i = 0; i < this->dtmfOutMessages_.size(); i++)
	{
		// Check if the message already has been deleted.
		if (this->dtmfOutMessages_.back() != NULL)
		{
			// Delete allocated memory
			delete(this->dtmfOutMessages_.back());
			// The message destructor have the responsibilty for deleting internal allocated mem.
		}
		// Remove the message pointer for the list
		this->dtmfOutMessages_.pop_back();
	}
	// Make sure the list is empty
	this->dtmfOutMessages_.clear();
}
//DtmfApi::DtmfOutMessage * newMessage()
DtmfOutMessage * DtmfApi::newMessage()
{
	// Allocate new message
	DtmfOutMessage * msg = new DtmfOutMessage(this); // Pass on a function pointer, so the message will be able to callback when the user decides to send the data.
	// Save message pointer for later use.
	this->dtmfOutMessages_.push_front(msg);
	// Return message
	return msg;
}
void DtmfApi::msgSendCallback_(DtmfOutMessage * dtmfOutMessage)
{
	// Called by message - send to queue
	this->apiTransportDown_->pushMsg((char*)dtmfOutMessage);
}
void DtmfApi::msgDispose_(DtmfOutMessage * dtmfOutMessage)
{
	// Remove from my list of messages
	this->dtmfOutMessages_.remove(dtmfOutMessage);
}
void DtmfApi::sendMessage(unsigned char rcvAddress, unsigned char rcvPort, unsigned char * data, unsigned long dataLength)
{
	DtmfOutMessage * msg = this->newMessage();
	msg->setAddress(rcvAddress,rcvPort);
	msg->setData(data,0,dataLength);
	msg->send();
}
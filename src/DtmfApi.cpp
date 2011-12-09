#include "DtmfApi.h"


/*
Fremgangmåde..

Bliver noget med at tegne et flowchart - det giver ikke mening med dynamisk allokering af dataplads, hvis der f.eks skal sendes mange MB.
På den anden side, vil new kun blive kaldt en gang pr. besked, og så er det transport der skal slippe op og delete. <<<--- good point
*/


DtmfApi::DtmfApi(unsigned char myAddress) : myAddress_(myAddress)
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
		}
		// Remove the message pointer for the list
		this->dtmfOutMessages_.pop_back();
	}
	// Make sure the list is empty
	this->dtmfOutMessages_.clear();
}
DtmfOutMessage * DtmfApi::newMessage()
{
	// Allocate new message
	DtmfOutMessage * msg = new DtmfOutMessage(this); // Pass on a function pointer, so the message will be able to callback when the user decides to send the data.
	// Save message pointer for later use.
	this->dtmfOutMessages_.push_front(msg);
	// Return message
	return msg;
}
void DtmfApi::msgSendCallback()
{
	std::cout << "msgSendCallback here!";
}
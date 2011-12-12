#include "DtmfOutMessage.h"

DtmfOutMessage::DtmfOutMessage(DtmfApi * apiClass)
{
	this->creatorApi = apiClass;
}
void DtmfOutMessage::send()
{
	//this->creatorApi->msgSendCallback_(this);
}
void DtmfOutMessage::dispose()
{
	//this->creatorApi->msgSendCallback_();
}
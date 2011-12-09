#include "DtmfOutMessage.h"

DtmfOutMessage::DtmfOutMessage(DtmfApi * apiClass)
{
	this->createrApi = apiClass;
}
void DtmfOutMessage::send()
{
	this->createrApi->msgSendCallback();
}
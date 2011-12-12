#ifndef DTMFCALLBACK_H
#define DTMFCALLBACK_H

//#include "DtmfApi.h"

class DtmfCallback
{
private:
protected:
public:
	virtual void callbackMethod(DtmfInMessage * message){};
};

#endif
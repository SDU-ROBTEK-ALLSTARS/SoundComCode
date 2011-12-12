#include "DtmfApi.h"
#include "DtmfCallback.h"
#include <iostream>
#include "DtmfInMessage.h"
#include "DtmfOutMessage.h"

// This is a part of the testing of the API layer

class MyPort1 : public DtmfCallback
{
	virtual void callbackMethod(DtmfInMessage * message)
	{
		unsigned char * data;
		message->getData(data);
		std::cout << "PORT1: ";
		for (unsigned int i = 0; i < message->getMessageLength(); i++)
		{
			std::cout << data[i];
		}
		std::cout << std::endl;
	}
};
class MyPort2 : public DtmfCallback
{
	virtual void callbackMethod(DtmfInMessage * message)
	{
		unsigned char * data;
		message->getData(data);
		std::cout << "PORT2: ";
		for (unsigned int i = 0; i < message->getMessageLength(); i++)
		{
			std::cout << data[i];
		}
		std::cout << std::endl;
	}
};

int main(void)
{
	DtmfApi * dtmf = new DtmfApi(1);
	MyPort1 * myPort1 = new MyPort1();
	MyPort2 * myPort2 = new MyPort2();
	dtmf->servicePort('1',myPort1);
	dtmf->servicePort('2',myPort2);

	DtmfOutMessage * m = dtmf->newMessage();
	m->send();
	//DtmfMsgBuffer * m = new DtmfMsgBuffer();

	bool run = true;
	char cmd;
	while (run)
	{
		std::cin >> cmd;
		switch (cmd)
		{
		/*case 'p':
			std::cout << std::endl << m->queueSize() << " - " << (*(m->pullMsg()));
			break;*/
		case 'x':
			run = false;
			break;
		default:
			//m->pushMsg(new char(cmd));
			break;
		}
	}
	delete(dtmf);
	return 0;
}
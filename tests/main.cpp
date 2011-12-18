
#include "../src/api/DtmfApi.h"
#include "../src/DtmfCallback.h"
#include <iostream>
#include "../src/buffers/DtmfInMessage.h"
#include "../src/buffers/DtmfOutMessage.h"
#include <string>

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
	unsigned char * data1 = (unsigned char*)"Dette er en test\n";
	m->setData(data1,0,18);
	unsigned char * data2 = (unsigned char*)"ER";
	m->setData(data2,2,2);
	m->send(); // This will delete the message, when it's processed
	
	char * data3 = "Slip flodhestene loes!";
	dtmf->sendMessage(0,0,(unsigned char*)data3,std::strlen(data3));

	bool run = true;
	char cmd;
	while (run)
	{
		std::cin >> cmd;
		switch (cmd)
		{
		case '0':
			//std::cout << std::endl << m->queueSize() << " - " << (*(m->pullMsg()));
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
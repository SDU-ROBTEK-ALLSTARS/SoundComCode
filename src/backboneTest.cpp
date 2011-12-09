#include "DtmfBackbone.h"
#include "CircularCharArrayBuffer.h"

int main()
{
	//CircularCharBufferArray cArray;
	CircularCharArrayBuffer cArray(11,5);
	char * john;
	char * john2;
	int * i;
	int * i2;

	cArray.front(john,i);
	cArray.back(john2,i2);
	memcpy(john,"john",5);
	*i = 5;
	cArray.advanceFront();
	


	
	
	
	cArray.front(john,i);
	cArray.back(john2,i2);
	memcpy(john,"hans",5);
	*i = 5;
	cArray.advanceFront();
	
	cArray.front(john,i);
	cArray.back(john2,i2);
	memcpy(john,"flemming",9);
	*i = 9;
	cArray.advanceFront();
	
	cArray.front(john,i);
	cArray.back(john2,i2);
	memcpy(john,"ibrahimmaz",11);
	*i = 11;
	cArray.advanceFront();
	
	
	cArray.front(john,i);
	memcpy(john,"ib",3);
	*i = 3;
	cArray.advanceFront(); //Skulle denne ikke give fejl ?
	cArray.front(john,i);
	memcpy(john,"ib",3);
	*i = 3;
	
	
	std::cout << "Count : " << cArray.count()<<std::endl;
	std::cout << "reading:" << std::endl;
	while(cArray.count() != 0)
	{
		cArray.back(john,i);
		std::cout << "\tPayload: " << john << " size: " << *i << std::endl;
		cArray.advanceBack();
	}
	std::cout << cArray.statusstring() << std::endl;
	








	/*DtmfMsgBuffer * a = new DtmfMsgBuffer();
	DtmfMsgBuffer * b = new DtmfMsgBuffer();
	DtmfBackbone * bBone = new DtmfBackbone(new DtmfApi(),a,b);*/
	system("PAUSE");
	//delete bBone;
	return 0;
}
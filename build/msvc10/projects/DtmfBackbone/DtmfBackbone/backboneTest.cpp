#include "DtmfBackbone.h"
int main()
{
	DtmfBackbone * bBone = new DtmfBackbone(new DtmfApi);
	system("PAUSE");
	delete bBone;
	return 0;
}
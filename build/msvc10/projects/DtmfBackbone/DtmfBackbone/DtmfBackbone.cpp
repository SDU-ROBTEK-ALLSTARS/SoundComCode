#include "DtmfBackbone.h"
DtmfBackbone::DtmfBackbone(DtmfApi * dtmfApi)
{
	//Instantiate the layers, and threads.
	this->dtmfapi =  dtmfApi;
	this->dtmfBuffer = new DtmfBuffer(DATAGRAM_BUFFER_IN_SIZE,DATAGRAM_BUFFER_OUT_SIZE,FRAME_BUFFER_IN_SIZE,FRAME_BUFFER_OUT_SIZE);
	this->dtmfDatalink = new DtmfDatalink();
	this->dtmfPhysical = new DtmfPhysical();
	this->dtmfSession = new DtmfSession();
	this->portaudioInterface = new PortaudioInterface(); //Is this something the physical layers does for me ?
	

	
	//Spawn the backbone thread, this must be the last thing to do. Object is running
	workerThread = boost::thread(boost::ref(*this));
	
	     

}
DtmfBackbone::~DtmfBackbone()
{
	delete dtmfBuffer;
	delete dtmfDatalink;
	delete dtmfPhysical;
	delete dtmfSession;
	delete portaudioInterface;
}

//Main dispatching loop
void DtmfBackbone::operator()()
{
	//TODO: follow diagram
	while(true)
	{

		/*
		#define T_PCM_MAX	10
		#define T_PCM_MIN   20
#define T_FRAME_MAX	5 
#define T_FRAME_MIN 10
#define T_DGRAM_MAX 2
#define T_DGRAM_MIN 5
		*/




		std::cout << "test";
		//Primary thresholds
		/*if(TONE_IN > T_TONE_MAX)&&(ROOM_FOR_FRAME)
		{
			decodeTone();
		}*/
			
			
		//General work
		//else sleep
	}
}



//TODO: add actual encode/decode calls
void DtmfBackbone::decodeTone()
{
}
void DtmfBackbone::decodeFrame()
{
}
void DtmfBackbone::decodeDatagram()
{
}
void DtmfBackbone::encodeMessage()
{
}
void DtmfBackbone::encodeDatagram()
{
}
void DtmfBackbone::encodeFrame()
{
}
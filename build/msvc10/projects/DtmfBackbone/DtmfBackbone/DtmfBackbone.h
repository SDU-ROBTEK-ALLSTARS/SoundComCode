//Placeholders
class placeholder
{
public:
	void encode(char * inputUp,char * inputdown,char * outputUp,char * outputDown)
	{
	}
	void decode(char * inputUp,char * inputdown,char * outputUp,char * outputDown)
	{
	}
};

typedef placeholder DtmfApi; //To simulate, randomly deposit in msgIn.
typedef placeholder DtmfSession;
typedef placeholder DtmfDatalink;
typedef placeholder DtmfPhysical;

typedef placeholder PortaudioInterface; //Essentially two buffers.


//Everything above this line should be implemented by someone at some point.






#ifndef DTMFBACKBONE_H
#define DTMFBACKBONE_H


//Default buffer værdier, pt bare bogus.
#define T_TONE_MAX	10
#define T_TONE_MIN   20
#define T_FRAME_MAX	5 
#define T_FRAME_MIN 10
#define T_DGRAM_MAX 2
#define T_DGRAM_MIN 5
#define SLEEPTIME 10
#define FRAME_BUFFER_IN_SIZE 1000
#define FRAME_BUFFER_OUT_SIZE 1000
#define DATAGRAM_BUFFER_IN_SIZE 200
#define DATAGRAM_BUFFER_OUT_SIZE 200



#include <boost/thread.hpp>
#include "DtmfBuffer.h"


class DtmfBackbone
{
private:
	
	DtmfApi * dtmfapi;
	DtmfSession * dtmfSession;
	DtmfDatalink * dtmfDatalink;
	DtmfPhysical * dtmfPhysical;
	PortaudioInterface * portaudioInterface;
	DtmfBuffer * dtmfBuffer;
	boost::thread workerThread;
	int test;

public:
	void operator()();
	DtmfBackbone(DtmfApi * dtmfApi);
	~DtmfBackbone();
	//ASSUMEDIRECTCONTROL() //stop workerthread.
	//RELEASEDIRECTCONTROL() //start workerthread again.
	void decodeTone();
	void decodeFrame();
	void decodeDatagram();
	void encodeMessage();
	void encodeDatagram();
	void encodeFrame();
};

#endif DTMFBACKBONE_H
#ifndef DTMFDUMMYBUFFER_H
#define DTMFDUMMYBUFFER_H

//#include "DtmfApi.h"
#include <boost\circular_buffer.hpp>

#include "dummyMsgBuffer.h"
// Simulates the buffers used in the project

class DtmfBuffer
{
public:
	boost::circular_buffer<char> * datagramIn;
	boost::circular_buffer<char> * datagramOut;
	boost::circular_buffer<char> * frameIn;
	boost::circular_buffer<char> * frameOut;
	DtmfMsgBuffer * msgBufferIn;
	DtmfMsgBuffer * msgBufferOut;
	DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity);
	~DtmfBuffer();
private:
	//boost::circular_buffer<T> myBuffer_;
};
#endif DTMFDUMMYBUFFER_H


#ifndef DTMFBUFFER_H
#define DTMFBUFFER_H
#include <boost\circular_buffer.hpp>
#include "DtmfMsgBuffer.h"

class DtmfBuffer
{
private:
public:
	boost::circular_buffer<char> * datagramIn;
	boost::circular_buffer<char> * datagramOut;
	boost::circular_buffer<char> * frameIn;
	boost::circular_buffer<char> * frameOut;
	DtmfMsgBuffer * msgBufferIn;
	DtmfMsgBuffer * msgBufferOut;
	DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity);
	~DtmfBuffer();
};
#endif DTMFBUFFER_H
#include "DtmfBuffer.h"
DtmfBuffer::DtmfBuffer(int datagramInCapacity, int datagramOutCapacity, int frameInCapacity, int frameOutCapacity)
{
	datagramIn = new boost::circular_buffer<char>(datagramInCapacity); 
	datagramOut  = new boost::circular_buffer<char>(datagramOutCapacity);
	frameIn  = new boost::circular_buffer<char>(frameInCapacity);
	frameOut  = new boost::circular_buffer<char>(frameOutCapacity);
	msgBufferIn = new DtmfMsgBuffer();
	msgBufferOut = new DtmfMsgBuffer();
}

DtmfBuffer::~DtmfBuffer()
{
	delete datagramIn;
	delete datagramOut;
	delete frameIn;
	delete frameOut;
	delete msgBufferIn;
	delete msgBufferOut;
}
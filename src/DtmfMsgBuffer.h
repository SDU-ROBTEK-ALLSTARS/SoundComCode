//TODO: PLACEHOLDER
#ifndef DTMFMSGBUFFER_H
#define DTMFMSGBUFFER_H
#include <boost/circular_buffer.hpp>
class DtmfMsgBuffer
{
private:
public:
	void pushMsg();
	void pullMsg();
};
#endif DTMFMSGBUFFER_H

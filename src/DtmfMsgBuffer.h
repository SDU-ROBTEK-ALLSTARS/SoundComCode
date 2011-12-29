//TODO: PLACEHOLDER
#ifndef DTMFMSGBUFFER_H
#define DTMFMSGBUFFER_H

#include <boost/thread.hpp>
class DtmfMsgBuffer
{
private:
	std::list<char *> buffer_;
	boost::mutex accessMutex;
public:
	DtmfMsgBuffer();
	~DtmfMsgBuffer();
	bool empty();
	unsigned int queueSize();
	void pushMsg(char * inChar);
	char * pullMsg();
};
#endif //DTMFMSGBUFFER_H

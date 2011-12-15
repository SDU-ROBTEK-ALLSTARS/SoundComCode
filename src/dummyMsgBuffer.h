//TODO: PLACEHOLDER
#ifndef DTMFMSGBUFFER_H
#define DTMFMSGBUFFER_H

#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>

class DtmfMsgBuffer
{
private:
	boost::circular_buffer<char *> * buffer_;
	boost::mutex accessMutex;
public:
	DtmfMsgBuffer()
	{
		// Create circular buffer
		this->buffer_ = new boost::circular_buffer<char *>();
		this->buffer_->set_capacity(2048); // This class is only for test, so this will do
	}
	~DtmfMsgBuffer()
	{
		delete(this->buffer_);
	}
	bool empty()
	{
		if(0 < queueSize())
			return false;
		return true;
	}
	unsigned int queueSize()
	{
		unsigned int size;
		this->accessMutex.lock();
		size = this->buffer_->size();
		this->accessMutex.unlock();
		return size;
	}
	void pushMsg(char * inChar)
	{
		this->accessMutex.lock();
		this->buffer_->push_back(inChar);
		this->accessMutex.unlock();
	}
	char * pullMsg()
	{
		char * outChar;
		this->accessMutex.lock();
		if (0 < this->buffer_->size())
		{
			outChar = this->buffer_->front();
			this->buffer_->pop_front();
		}
		this->accessMutex.unlock();
		return outChar;
	}
};
#endif DTMFMSGBUFFER_H

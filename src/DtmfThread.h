#ifndef DTMFTHREAD_H
#define DTMFTHREAD_H

//#include "DtmfApi.h"
#include <boost/thread.hpp>

// Used for providing threading support to classes
class DtmfThread
{
private:
	boost::thread thread_; // Boost thread
protected:
	virtual void main(); // Main loop dummy - this method should exit if continueRunning_ turns to false
	volatile bool continueRunning_; // Used for stopping and starting main loop -- http://drdobbs.com/cpp/184403766 for volatile
public:
	DtmfThread();
	~DtmfThread(); // Stop main loop, wait for it to exit, exit thread
	void start();  // Create thread and allow running of main loop
};

#endif DTMFTHREAD_H

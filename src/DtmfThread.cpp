#include "DtmfThread.h"

DtmfThread::DtmfThread()
{
}
void DtmfThread::start()
{
	// Allow main loop to run
	this->continueRunning_ = true;

	// Create thread
	this->thread_ = boost::thread(boost::bind(&DtmfThread::main, boost::ref(*this)));

}

DtmfThread::~DtmfThread()
{
	// Thread is destroyed
	// Stop main loop
	this->continueRunning_ = false;

	// Wait for running task to finish, before continuing
	this->thread_.join();
}
void DtmfThread::main()
{
	// Placeholder
}

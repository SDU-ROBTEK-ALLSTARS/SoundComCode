#ifndef DTMFDUMMYBACKBONE_H
#define DTMFDUMMYBACKBONE_H

// this is used for simulating the backbone
#include "DtmfThread.h"
#include "dummyBuffer.h"
#include "DtmfInMessage.h"
//#include "DtmfApi.h"
#include <fstream>
#include <iostream>

// Forward declarations
class DtmfApi;
//class DtmfMsgBuffer;
class DtmfThread;

#include <iostream>

// LOOP:
//	Read from file "fromTransport.txt".
//  If there's any data in the file, copy it to the buffer and signal callback.
//  If there's any data in in the buffer, write to "toTransport.txt"

class DtmfBackbone: public DtmfThread
{
protected:
	virtual void main()
	{
		char readTemp[2048];
		DtmfInMessage * m;
		std::fstream fromTransportStream;
		while (this->continueRunning_)
		{
			// Check for new data
			// Set up IO stream
			fromTransportStream.open("fromTransport.txt",  std::fstream::in ); // std::fstream::trunc | std::fstream::out 

			while (fromTransportStream.getline(readTemp,2048))
			{
				// DEBUG: Send data to cout
				/*std::cout << "DATA IN FROM TRANSPORT: ";
				for (int i = 0; i < fromTransportStream.gcount(); i++)
					std::cout << readTemp[i];
				std::cout << std::endl;*/
				// Allocate mem
				unsigned char * passOn = new unsigned char[fromTransportStream.gcount()];
				m = new DtmfInMessage('2','1',fromTransportStream.gcount(),passOn);
				memcpy(passOn,readTemp,fromTransportStream.gcount()); // Copy data to dataarea
				// Send data to buffer
				this->transportApiUp_->pushMsg((char*)m);
				(*(this->callbackMainLoopMutex_))->unlock(); // Unlock callback thread
			}
			// Close file stream
			fromTransportStream.clear(); // This may be redundant?
			fromTransportStream.close();

			// Empty the file
			fromTransportStream.open("fromTransport.txt",  std::fstream::trunc | std::fstream::out );
			fromTransportStream.close();

			// Small delay to save resources
			boost::this_thread::sleep(boost::posix_time::seconds(1));
		}
	}
	DtmfMsgBuffer * apiTransportDown_, * transportApiUp_;
	boost::mutex * * callbackMainLoopMutex_;
public:
	DtmfBackbone(DtmfApi * dtmfApi, DtmfMsgBuffer *& apiTransportDown, DtmfMsgBuffer *& transportApiUp, boost::mutex ** callbackMainLoopMutex)
	{
		// Buffers
		this->apiTransportDown_ = apiTransportDown = new DtmfMsgBuffer();
		this->transportApiUp_ = transportApiUp = new DtmfMsgBuffer();
		this->callbackMainLoopMutex_ = callbackMainLoopMutex;
	}
	~DtmfBackbone()
	{
		
	}
	//dummyBuffer<apiToTransport> * apiToTransportBuffer;
};
#endif DTMFDUMMYBACKBONE_H
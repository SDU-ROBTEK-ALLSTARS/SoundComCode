
#include <iostream>
#include <boost/circular_buffer.hpp>
#include "packet.h"
#include "DtmfTransport.h"


void portList(const DtmfTransport& tp);


int main()
{
    // Set up test buffers (normally done by DtmfTransport itself)
    std::deque<Packet *> DllTransportUp_;
    std::deque<Packet *> TransportApiUp_;
    std::deque<Packet *> ApiTransportDown_;
    std::deque<Packet *> TransportDllDown_;

    // Set up two transport layers
    DtmfTransport tp1, tp2;
    tp1.setPort(45);
    tp2.setPort(70);

    // Make test packets (these are "normal" packets -
    // SYN and the likes are auto-generated).
    Packet pack[10];
    bool flags[8] = {false};
    pack[0].make(tp1.port(),tp2.port(),flags,0,0,0,0);


    std::cout << "test";
    // Put in queue
    ApiTransportDown_.push_back(&pack[0]);

    std::cout << "Size before: " << TransportDllDown_.size() << std::endl;
    // tp1 connects to tp2
    tp1.encodePacket(&ApiTransportDown_, &TransportDllDown_);

    std::cout << "Size after: " << TransportDllDown_.size() << std::endl;

    return 0;
}


void portList(const DtmfTransport& tp)
{
	std::cout << std::endl << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << "Port" << (char)9 << "In use" << (char)9 << std::endl;
	bool *portTable = tp.getPortTable();
	int j1, j2, j3, j4;
	for (int k=0; k<64; k++) {
		j1=k;
		j2=k+64;
		j3=k+128;
		j4=k+192;
		std::cout << j1 << (char)9 << (int)portTable[j1] << (char)9
		          << j2 << (char)9 << (int)portTable[j2] << (char)9
				  << j3 << (char)9 << (int)portTable[j3] << (char)9
				  << j4 << (char)9 << (int)portTable[j4] << std::endl;
	}
}

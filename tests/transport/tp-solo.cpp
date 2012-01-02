#include <iostream>
#include <map>
#include <boost/circular_buffer.hpp>
#include "packet.h"
#include "DtmfTransport.h"

void portList(const DtmfTransport &tp);

int main()
{
    try {

        // Set up test buffers (normally done by DtmfTransport itself)
        std::deque<Packet *> tp1DllTransportUp;
        std::deque<Packet *> tp1TransportApiUp;
        std::deque<Packet *> tp1ApiTransportDown;
        std::deque<Packet *> tp1TransportDllDown;

        std::deque<Packet *> tp2DllTransportUp;
        std::deque<Packet *> tp2TransportApiUp;
        std::deque<Packet *> tp2ApiTransportDown;
        std::deque<Packet *> tp2TransportDllDown;

        // Set up two transport layers
        DtmfTransport tp1, tp2;
        tp1.setPort(45);
        tp2.setPort(70);

        //portList(tp1);

        // Make test packets (these are "normal" packets -
        // SYN and the likes are auto-generated).
        // + Put them in queue
        int numPackets = 10;
        Packet pack[numPackets];
        bool flags[8] = {false};
        unsigned char data[5] = {1,2,3,4,5};
        for (int i=0; i<numPackets; i++) {
            pack[i].make(tp1.port(),tp2.port(),flags,0,0,5,data);
            tp1ApiTransportDown.push_back(&pack[i]);
        }
        tp2ApiTransportDown = tp1ApiTransportDown;

        std::cout << "#1\n";
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);

        std::cout << "#2\n";
        tp2DllTransportUp = tp1TransportDllDown;
        tp2.processReceivedPacket(&tp2DllTransportUp, &tp2TransportApiUp);
        tp2.processOutgoingPacket(&tp2ApiTransportDown, &tp2TransportDllDown);

        std::cout << "#1\n";
        tp1DllTransportUp = tp2TransportDllDown;
        tp1.processReceivedPacket(&tp1DllTransportUp, &tp1TransportApiUp);
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);

        std::cout << "#2\n";
        tp2DllTransportUp = tp1TransportDllDown;
        tp2.processReceivedPacket(&tp2DllTransportUp, &tp2TransportApiUp);
        tp2.processOutgoingPacket(&tp2ApiTransportDown, &tp2TransportDllDown);

        std::cout << "#1\n";
        tp1DllTransportUp = tp2TransportDllDown;
        tp1.processReceivedPacket(&tp1DllTransportUp, &tp1TransportApiUp);
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);



        std::cout << "#2\n";
        tp2DllTransportUp = tp1TransportDllDown;
        tp2.processReceivedPacket(&tp2DllTransportUp, &tp2TransportApiUp);
        tp2.processOutgoingPacket(&tp2ApiTransportDown, &tp2TransportDllDown);

        std::cout << "#1\n";
        tp1DllTransportUp = tp2TransportDllDown;
        tp1.processReceivedPacket(&tp1DllTransportUp, &tp1TransportApiUp);
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);

        std::cout << "#2\n";
        tp2DllTransportUp = tp1TransportDllDown;
        tp2.processReceivedPacket(&tp2DllTransportUp, &tp2TransportApiUp);
        tp2.processOutgoingPacket(&tp2ApiTransportDown, &tp2TransportDllDown);

        std::cout << "#1\n";
        tp1DllTransportUp = tp2TransportDllDown;
        tp1.processReceivedPacket(&tp1DllTransportUp, &tp1TransportApiUp);
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);

        std::cout << "#2\n";
        tp2DllTransportUp = tp1TransportDllDown;
        tp2.processReceivedPacket(&tp2DllTransportUp, &tp2TransportApiUp);
        tp2.processOutgoingPacket(&tp2ApiTransportDown, &tp2TransportDllDown);

        std::cout << "#1\n";
        tp1DllTransportUp = tp2TransportDllDown;
        tp1.processReceivedPacket(&tp1DllTransportUp, &tp1TransportApiUp);
        tp1.processOutgoingPacket(&tp1ApiTransportDown, &tp1TransportDllDown);




        std::map<unsigned char,Packet *>::iterator it;

        std::cout << "\ntp1 recvUnAckPackets_:\n";
        it = tp1.recvUnAckPackets_.begin();
        while(it != tp1.recvUnAckPackets_.end()) {
            std::cout << (int)(*it).first << std::endl;
            it++;
        }
        std::cout << "tp1 sentUnAckPackets_:\n";
        it = tp1.sentUnAckPackets_.begin();
        while(it != tp1.sentUnAckPackets_.end()) {
            std::cout << (int)(*it).first << std::endl;
            it++;
        }

        std::cout << "\ntp2 recvUnAckPackets_:\n";
        it = tp2.recvUnAckPackets_.begin();
        while(it != tp2.recvUnAckPackets_.end()) {
            std::cout << (int)(*it).first << std::endl;
            it++;
        }
        std::cout << "tp2 sentUnAckPackets_:\n";
        it = tp2.sentUnAckPackets_.begin();
        while(it != tp2.sentUnAckPackets_.end()) {
            std::cout << (int)(*it).first << std::endl;
            it++;
        }


    } catch (const std::exception &e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    return 0;
}


void portList(const DtmfTransport &tp)
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


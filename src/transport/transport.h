

/*
 * Transport class declaration.
 *
 *
 * The goal of the Transport class is to implement a simple protocol handling
 * tasks most often assigned to the transport layer protocols (OSI model).
 * That is, process-to-process communication.
 *
 * For that we require a port number to be set by the application using
 * this class.
 * 
 */

/*
 * TODO / probs
 *
 * -Check if port number assigned is unique
 *
 */


#ifndef TRANSPORT_H
#define TRANSPORT_H

//#include

class Transport
{
public:
	// def
	Transport();

	//g
	~Transport();

	//
	int setClientPort(unsigned char);

	//
	int setServerPort(unsigned char);

	//
	void encode(unsigned char destServerPort/*, data*/);

	//
	void decode();

	//
	//

private:
	// We allow different client and server ports. Which is used depends on traffic direction.
	// The port number is 8 bit long.
	unsigned char clientPort;
	unsigned char serverPort;

	//
	//pack();
	//unpack();

	//handshake() ?
	//ack() ?

};
#endif //TRANSPORT_H
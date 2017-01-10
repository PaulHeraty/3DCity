/*
 * C++ wrapper around an UDP socket
 */

#ifndef __UDP_SOCKET_H__
#define __UDP_SOCKET_H__

#include "Common.h"

#include <sys/types.h>	// for data types
#include <sys/socket.h>	// for socket(), connect(), send(), recv()
#include <netinet/in.h>	// for IPPROTO_UDP, sockadd_in
#include <arpa/inet.h>	// for inet_ntoa()
#include <unistd.h>	// for close()
#include <netdb.h>	// for hostent, gethostbyname()
#include <fcntl.h>	// for fcntl()
#include <errno.h>
#include <cstring>	// for memset

#ifndef SERVER_ADDR
#define SERVER_ADDR "localhost"
#endif
#ifndef SERVER_PORT
#define SERVER_PORT 8888
#endif
#ifndef SERVER_MAX_MSG_LENGTH
#define SERVER_MAX_MSG_LENGTH 255
#endif

class UDPSocket
{
public:
	/* Constructors */
	/**
	 * Construct generic UDPSocket
	 **/
	UDPSocket() throw(std::runtime_error);

	/**
	 * Construct UDPSocket that binds to the given local port
	 * parameters:
	 * - localPort: port to bind to
	 * - blocking: if true, socket is blocking on recv(), if false it is not
	 **/
	UDPSocket(unsigned short localPort, bool blocking = false) throw(std::runtime_error);


	/* Destructor */
	~UDPSocket();


	/* Send datagram */
	/**
	 * Sends the given buffer as a UDP datagram to the specified address and port.
	 * parameters:
	 * - buffer:     buffer to send
	 * - bufferLen:  number of bytes in buffer
	 * - remoteAddr: address (IP or FQDN) to send to
	 * - remotePort: port to send to
	 * return value:
	 *   none
	 **/
	void send(const void *buffer, const int bufferLen, const std::string &remoteAddr, const unsigned short remotePort);

	/**
	 * Sends the given string as a UDP datagram to the specified address and port.
	 * This is a convenience method which calls the previous method with the correct
	 * length parameter.
	 * parameters:
	 * - message:    message to send
	 * - remoteAddr: address (IP or FQDN) to send to
	 * - remotePort: port to send to
	 * return value:
	 *   none
	 **/
	void send(const std::string &message, const std::string &remoteAddr, const unsigned short remotePort);


	/* Receive datagram from this socket */
	/**
	 * Receives a message from this socket.
	 * The data will be placed in the given buffer, the source address and port will be
	 * placed in the given parameters.
	 * parameters:
	 * - buffer: buffer to receive data
	 * - bufferLen: maximum number of bytes to receive
	 * - sourceAddr: address of datagram source
	 * - sourcePort: port of datagram source
	 * return value:
	 *   number of bytes received, -1 for error
	 **/
	int recv(void *buffer, const int bufferLen, std::string &sourceAddr, unsigned short &sourcePort);

	/**
	 * Read up to bufferLen bytes of data into the given buffer from this socket.
	 * parameters:
	 * - buffer: buffer to receive data
	 * - bufferLen: maximum number of bytes to receive
	 * return value:
	 *   number of bytes read, 0 for EOF, -1 for error
	 **/
	int recv(void *buffer, int bufferLen);
private:
	// don't allow value semantics on this object
	UDPSocket(const UDPSocket &sock);
	void operator=(const UDPSocket &sock);

	int sockDesc;	// socket descriptor
};

#endif // __UDP_SOCKET_H__

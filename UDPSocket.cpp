/*
 * C++ wrapper around an UDP socket
 */

#include "UDPSocket.h"


using std::string;

/* Constructors */
UDPSocket::UDPSocket() throw(std::runtime_error)
{
	// create new socket
	if ((sockDesc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		throw std::runtime_error("Socket creation failed (socket())");
	}
}

UDPSocket::UDPSocket(unsigned short localPort, bool blocking) throw(std::runtime_error)
{
	// create new socket
	if ((sockDesc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		throw std::runtime_error("Socket creation failed (socket())");
	}

	// bind the socket to its port
	sockaddr_in localAddr;
	::memset(&localAddr, 0, sizeof(localAddr));

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons(localPort);

	if (bind(sockDesc, (sockaddr *) &localAddr, sizeof(sockaddr_in)) < 0)
	{
		throw std::runtime_error("Bind to local port failed (bind())");
	}

	if (!blocking)
	{
		if (fcntl(sockDesc, F_SETFL, O_NONBLOCK) < 0)
		{
			throw std::runtime_error("Set socket to non-blocking failed (fcntl())");
		}
	}
}


/* Destructor */
UDPSocket::~UDPSocket()
{
	::close(sockDesc);
}


/* Send datagram */
void UDPSocket::send(const void *buffer, const int bufferLen, const string &remoteAddr, const unsigned short remotePort)
{
	sockaddr_in destAddr;
	::memset(&destAddr, 0, sizeof(destAddr));

	destAddr.sin_family = AF_INET;

	hostent *host;	// resolve remote address
	if ((host = gethostbyname(remoteAddr.c_str())) == NULL)
	{
		throw std::runtime_error("Failed to resolve hostname (gethostbyname())");
	}

	destAddr.sin_addr.s_addr = *((unsigned long *) host->h_addr_list[0]);
	destAddr.sin_port = htons(remotePort);

	// write buffer as a single message
	if (sendto(sockDesc, buffer, bufferLen, 0, (sockaddr *) &destAddr, sizeof(destAddr)) != bufferLen)
	{
		throw std::runtime_error("Send failed (sendto())");
	}
}

void UDPSocket::send(const string &message, const string &remoteAddr, const unsigned short remotePort)
{
	send(message.c_str(), message.length(), remoteAddr, remotePort);
}


/* Receive datagram */
int UDPSocket::recv(void *buffer, const int bufferLen, string &sourceAddr, unsigned short &sourcePort)
{
	sockaddr_in remoteAddr;
	socklen_t addrLen = sizeof(remoteAddr);

	int result;
	if (((result = recvfrom(sockDesc, buffer, bufferLen, 0, (sockaddr *) &remoteAddr, (socklen_t *) &addrLen)) < 0) && (errno != EWOULDBLOCK))
	{
		throw std::runtime_error("Receive failed (recvfrom())");
	}

	sourceAddr = inet_ntoa(remoteAddr.sin_addr);
	sourcePort = ntohs(remoteAddr.sin_port);

	return result;
}

int UDPSocket::recv(void *buffer, const int bufferLen)
{
	int result;
	if (((result = ::recv(sockDesc, buffer, bufferLen, 0)) < 0 && (errno != EWOULDBLOCK)))
	{
		throw std::runtime_error("Receive failed (recv())");
	}

	return result;
}

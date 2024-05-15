#include "UDPCommunication.h"
#include <iostream>
#include <sys/types.h>
#include <netdb.h>

#define BUF_SIZE 141

UDPCommunication::UDPCommunication(std::string ServerAddr, std::string ServerPort)
    : mClientPort(ServerPort), mClientAddr(ServerAddr), mPeer_addr_len(0) {

	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_UDP;	/* UDP */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	int r (getaddrinfo(mClientAddr.c_str(), mClientPort.c_str(), &hints, &result));

	if (r != 0 || result == NULL) {
		throw UDPCommunication_runtime_error(("invalid address or port for UDP socket: \"" + mClientAddr + ":" + mClientPort + "\"").c_str());
	}

	mClientSocketFd = socket(result->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

	if (mClientSocketFd == -1) {
		freeaddrinfo(result);
		throw UDPCommunication_runtime_error(("could not create UDP socket for: \"" + mClientAddr + ":" + mClientPort + "\"").c_str());
	}

	r = bind(mClientSocketFd, result->ai_addr, result->ai_addrlen);

	freeaddrinfo(result);           /* No longer needed */

	if(r != 0) {
		close(mClientSocketFd);
		throw UDPCommunication_runtime_error(("could not bind UDP socket with: \"" + mClientAddr + ":" + mClientPort + "\"").c_str());
	}
}

UDPCommunication::~UDPCommunication() {
    close(mClientSocketFd);
}

int UDPCommunication::GetClientSocketFd(void) const {
    return mClientSocketFd;
}

std::string UDPCommunication::GetClientPort(void) const {
    return mClientPort;
}

std::string UDPCommunication::GetClientAddr(void) const {
    return mClientAddr;
}

int UDPCommunication::Receive(char *msg) {
	int s;
	ssize_t nread;
	char host[NI_MAXHOST], service[NI_MAXSERV];

	mPeer_addr_len = sizeof(struct sockaddr_storage);

	do {
		nread = recvfrom(mClientSocketFd, msg, BUF_SIZE, 0, (struct sockaddr *) &mPeer_addr, &mPeer_addr_len);
	} while (nread == -1);	/* Ignore failed request */

	s = getnameinfo((struct sockaddr *) &mPeer_addr, mPeer_addr_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

	if (s == 0) {
		printf("Received %ld bytes from %s:%s\n", (long)nread, host, service);
		return 1;
	}
	else {
		fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
		return 1;
	}
}

int UDPCommunication::Send(std::string serverAddr, std::string serverPort, std::string msg) {
	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_UDP;	/* UDP */
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	int r (getaddrinfo(serverAddr.c_str(), serverPort.c_str(), &hints, &result));

	if (r != 0 || result == NULL) {
		throw UDPCommunication_runtime_error(("invalid address or port for UDP socket: \"" + serverAddr + ":" + serverPort + "\"").c_str());
	}

	 int clientSocketFd = socket(result->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

	if (clientSocketFd == -1) {
		freeaddrinfo(result);
		throw UDPCommunication_runtime_error(("could not create UDP socket for: \"" + serverAddr + ":" + serverPort + "\"").c_str());
	}

	r = connect(clientSocketFd, result->ai_addr, result->ai_addrlen);


	if(r != 0) {
		close(clientSocketFd);
		throw UDPCommunication_runtime_error(("could not bind UDP socket with: \"" + serverAddr + ":" + serverPort + "\"").c_str());
	}


	printf("Preparing message \"%s\" to be sent to %s:%s\n", msg, serverAddr, serverPort);

	if (sendto(clientSocketFd, msg.c_str(), sizeof(msg), 0, NULL, 0) == -1) {
		std::cout << "Message couldn't be sent" << std::endl;
		freeaddrinfo(result);
		close(clientSocketFd);
		return 0;
	}
	std::cout << "Message sent successfully" << std::endl;
	freeaddrinfo(result);
	close(clientSocketFd);
	return 1;
}

#include "UDPCommunication.h"
#include <iostream>
#include <sys/types.h>
#include <netdb.h>

#define BUF_SIZE 141

UDPCommunication::UDPCommunication(std::string ServerAddr, std::string ServerPort)
    : mServerPort(ServerPort), mServerAddr(ServerAddr), mPeer_addr_len(0) {

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

	int r (getaddrinfo(mServerAddr.c_str(), mServerPort.c_str(), &hints, &result));

	if (r != 0 || result == NULL) {
		throw UDPCommunication_runtime_error(("invalid address or port for UDP socket: \"" + mServerAddr + ":" + mServerPort + "\"").c_str());
	}

	mServerSocketFd = socket(result->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

	if (mServerSocketFd == -1) {
		freeaddrinfo(result);
		throw UDPCommunication_runtime_error(("could not create UDP socket for: \"" + mServerAddr + ":" + mServerPort + "\"").c_str());
	}

	r = bind(mServerSocketFd, result->ai_addr, result->ai_addrlen);

	freeaddrinfo(result);           /* No longer needed */

	if(r != 0) {
		close(mServerSocketFd);
		throw UDPCommunication_runtime_error(("could not bind UDP socket with: \"" + mServerAddr + ":" + mServerPort + "\"").c_str());
	}
}

UDPCommunication::~UDPCommunication() {
    close(mServerSocketFd);
}

int UDPCommunication::GetServerSocketFd(void) const {
    return mServerSocketFd;
}

std::string UDPCommunication::GetServerPort(void) const {
    return mServerPort;
}

std::string UDPCommunication::GetServerAddr(void) const {
    return mServerAddr;
}

int UDPCommunication::Receive(char *msg) {
	int s;
	ssize_t nread;
	char host[NI_MAXHOST], service[NI_MAXSERV];

	mPeer_addr_len = sizeof(struct sockaddr_storage);

	do {
		nread = recvfrom(mServerSocketFd, msg, BUF_SIZE, 0, (struct sockaddr *) &mPeer_addr, &mPeer_addr_len);
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

int UDPCommunication::Send(std::string msg) {
	std::string host, service;

	getnameinfo((struct sockaddr *) &mPeer_addr, mPeer_addr_len, host.data(), NI_MAXHOST, service.data(), NI_MAXSERV, NI_NUMERICSERV);

	printf("Preparing message \"%s\" to be sent to %s:%s\n", msg, host, service);

	if (sendto(mServerSocketFd, msg.c_str(), sizeof(msg), 0, (struct sockaddr *) &mPeer_addr, mPeer_addr_len) == -1) {
		std::cout << "Message couldn't be sent" << std::endl;
		return 0;
	}
	std::cout << "Message sent successfully" << std::endl;
	return 1;
}

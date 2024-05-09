#include "iot_server.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

UDPServer::UDPServer(const std::string addr, const std::string port)
    : mServerPort(port), mServerAddr(addr) {

    struct addrinfo hints;

    //memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int r (getaddrinfo(addr.c_str(), mServerPort.c_str(), &hints, &mServerAddrinfo));	// Por qué no inicializar con =?

    if (r != 0 || mServerAddrinfo == NULL) {
    	throw UDPServer_runtime_error(("invalid address or port for UDP socket: \"" + addr + ":" + mServerPort + "\"").c_str());
    }

    mServerSocket = socket(mServerAddrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    if (mServerSocket == -1) {
        freeaddrinfo(mServerAddrinfo);
        throw UDPServer_runtime_error(("could not create UDP socket for: \"" + addr + ":" + mServerPort + "\"").c_str());
    }

    r = bind(std::stoi(mServerSocket), mServerAddrinfo->ai_addr, mServerAddrinfo->ai_addrlen);
    if(r != 0) {
        freeaddrinfo(mServerAddrinfo);
        close(std::stoi(mServerSocket));
        throw UDPServer_runtime_error(("could not bind UDP socket with: \"" + addr + ":" + mServerPort + "\"").c_str());
    }
}

UDPServer::~UDPServer() {
    freeaddrinfo(mServerAddrinfo);
    close(std::stoi(mServerSocket));
}

int UDPServer::GetServerSocket(void) const {
    return mServerSocket;
}

int UDPServer::GetServerPort(void) const {
    return mServerPort;
}

std::string UDPServer::GetServerAddr(void) const {
    return mServerAddr;
}

int UDPServer::Receive(char *msg, size_t maxSize) {
    return recv(std::stoi(mServerSocket), msg, maxSize, 0);		//Estaba como ::recv(...) -> por qué? (scope operator)
}

int UDPServer::Send(const char *msg, size_t size) {

    return sendto(std::stoi(mServerSocket), msg, size, 0, mServerAddrinfo->ai_addr, ddrinfo->ai_addrlen);
}

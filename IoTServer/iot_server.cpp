#include "iot_server.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

UDPServer::UDPServer(std::string ServerAddr, std::string ServerPort)
    : mServerPort(ServerPort), mServerAddr(ServerAddr) {

    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int r (getaddrinfo(ServerAddr.c_str(), mServerPort.c_str(), &hints, &mServerAddrinfo));	// Por qué no inicializar con =?

    if (r != 0 || mServerAddrinfo == NULL) {
    	throw UDPServer_runtime_error(("invalid address or port for UDP socket: \"" + ServerAddr + ":" + mServerPort + "\"").c_str());
    }

    mServerSocket = socket(mServerAddrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);

    if (mServerSocket == -1) {
        freeaddrinfo(mServerAddrinfo);
        throw UDPServer_runtime_error(("could not create UDP socket for: \"" + ServerAddr + ":" + mServerPort + "\"").c_str());
    }

    r = bind(mServerSocket, mServerAddrinfo->ai_addr, mServerAddrinfo->ai_addrlen);
    if(r != 0) {
        freeaddrinfo(mServerAddrinfo);
        close(mServerSocket);
        throw UDPServer_runtime_error(("could not bind UDP socket with: \"" + ServerAddr + ":" + mServerPort + "\"").c_str());
    }
}

UDPServer::~UDPServer() {
    freeaddrinfo(mServerAddrinfo);
    close(mServerSocket);
}

int UDPServer::GetServerSocket(void) const {
    return mServerSocket;
}

std::string UDPServer::GetServerPort(void) const {
    return mServerPort;
}

std::string UDPServer::GetServerAddr(void) const {
    return mServerAddr;
}

int UDPServer::Receive(char *msg, size_t maxSize) {
    return recv(mServerSocket, msg, maxSize, 0);		//Estaba como ::recv(...) -> por qué? (scope operator)
}

int UDPServer::Send(const std::string clientAddr, const std::string clientPort, std::string msg, size_t size) {
	struct addrinfo* clientAddrinfo;
	struct addrinfo hints;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    int r (getaddrinfo(clientAddr.c_str(), clientPort.c_str(), &hints, &clientAddrinfo));

	if(r != 0 || clientAddrinfo == NULL) {
		throw UDPServer_runtime_error(("invalid address or port: \"" + clientAddr + ":" + clientPort + "\"").c_str());
	}

	int clientSocket = socket(clientAddrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	if(clientSocket == -1) {
		freeaddrinfo(clientAddrinfo);
		throw UDPServer_runtime_error(("could not create socket for: \"" + clientAddr + ":" + clientPort + "\"").c_str());
	}

	//Creation socket file descriptor
	if((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		UDPServer_runtime_error("Creation socket failed");
		close(clientSocket);
	}

    int sendMsg = sendto(clientSocket, msg.c_str(), size, 0, clientAddrinfo->ai_addr, clientAddrinfo->ai_addrlen);

    //Check if it has occurred an error
    if(sendMsg == -1){
    	UDPServer_runtime_error("Creation socket failed");
        close(clientSocket);
    }

    return sendMsg;
}

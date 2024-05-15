#ifndef IOT_SERVER_H_
#define IOT_SERVER_H_

#include <string.h>
#include <stdexcept>

class UDPServer_runtime_error : public std::runtime_error {
public:
	UDPServer_runtime_error(const char *msg) : std::runtime_error(msg){
	}
};

class UDPServer {
public:
	UDPServer(std::string addr, std::string port);
    ~UDPServer();
    int CloseServerSocket(void);
    int GetServerSocket(void) const;
    std::string GetServerPort(void) const;
    std::string GetServerAddr(void) const;

    int Receive(char *msg, size_t maxSize);
    int Send(const std::string clientAddr, const std::string clientPort, std::string msg, size_t size);

private:
    int mServerSocket;
    std::string mServerPort;
    std::string mServerAddr;
    struct addrinfo* mServerAddrinfo;
};

#endif /* IOT_SERVER_H_ */

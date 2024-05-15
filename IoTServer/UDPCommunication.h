#ifndef UDPCOMMUNICATION_H_
#define UDPCOMMUNICATION_H_

#include <string.h>
#include <stdexcept>
#include <unistd.h>
#include <sys/socket.h>

#define OK "OK"
#define NOK "NOK"


class UDPCommunication_runtime_error : public std::runtime_error {
public:
	UDPCommunication_runtime_error(const char *msg) : std::runtime_error(msg){
	}
};

class UDPCommunication {
public:
	UDPCommunication(std::string addr, std::string port);
    ~UDPCommunication();

    int GetServerSocketFd(void) const;
    std::string GetServerPort(void) const;
    std::string GetServerAddr(void) const;

    int Receive(char *msg);
    int Send(std::string msg);

private:
    int mServerSocketFd;
    std::string mServerPort;
    std::string mServerAddr;
    struct sockaddr_storage mPeer_addr;
    socklen_t mPeer_addr_len;
};

#endif /* UDPCOMMUNICATION_H_ */

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

    int GetClientSocketFd(void) const;
    std::string GetClientPort(void) const;
    std::string GetClientAddr(void) const;

    int Receive(char *msg);
    int Send(std::string serverAddr, std::string serverPort, std::string msg);

private:
    int mClientSocketFd;
    std::string mClientPort;
    std::string mClientAddr;
    struct sockaddr_storage mPeer_addr;
    socklen_t mPeer_addr_len;
};

#endif /* UDPCOMMUNICATION_H_ */

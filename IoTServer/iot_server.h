/*
 * iot_server.h
 *
 *  Created on: Apr 20, 2024
 *      Author: ubuntu
 */

#ifndef IOT_SERVER_H_
#define IOT_SERVER_H_

#include <string.h>
#include <stdexcept>

#define OK "OK"
#define NOK "NOK"


class UDPServer_runtime_error : public std::runtime_error {
public:
	UDPServer_runtime_error(const char *msg) : std::runtime_error(msg){

	}
};

class UDPServer {
public:
	UDPServer(const std::string addr, const std::string port);
    ~UDPServer();

    int GetServerSocket(void) const;
    int GetServerPort(void) const;
    std::string GetServerAddr(void) const;

    int Receive(char *msg, size_t maxSize);
    int Send(const char *msg, size_t size);

private:
    std::string mServerSocket;
    std::string mServerPort;
    std::string mServerAddr;
    struct addrinfo* mServerAddrinfo;
};

#endif /* IOT_SERVER_H_ */

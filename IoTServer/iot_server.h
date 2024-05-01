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


class udp_server_runtime_error : public std::runtime_error {
public:
    udp_server_runtime_error(const char *w) : std::runtime_error(w) {}
};

class udp_server {
public:
	udp_server(const std::string& addr, int port);
    ~udp_server();

    int get_socket() const;
    int get_port() const;
    std::string get_addr() const;

    int receive(char *msg, size_t max_size);
    int timed_receive(char *msg, size_t max_size, int max_wait_ms);
    int send(const char *msg, size_t size);

private:
    int f_socket;
    int f_port;
    std::string f_addr;
    struct addrinfo* f_addrinfo;
};

#endif /* IOT_SERVER_H_ */
// SNAP_UDP_CLIENT_SERVER_H
// vim: ts=4 sw=4 et

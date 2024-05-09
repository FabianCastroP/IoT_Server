#include "iot_server.h"
#include <iostream>

int main(void) {
    std::string server_address = "192.168.1.168";
    std::string client_address;
    int server_port = 5000;

    try {
        udp_server server(server_address, server_port);
        std::cout << "UDP server is running on " << server.get_addr() << ":" << server.get_port() << std::endl;

        char buffer[1024];
        while (true) {
        	std::cout << "Waiting for message..." << std::endl;
            int received_bytes = server.receive(buffer, sizeof(buffer));
            if (received_bytes > 0) {
                buffer[received_bytes] = '\0';
                std::cout << "Received message: " << buffer << std::endl;
                server.send()
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

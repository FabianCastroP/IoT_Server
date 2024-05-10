#include "iot_server.h"
#include <iostream>

int main(void) {
    std::string serverAddress = "192.168.1.168";
    std::string clientAddress = "192.168.1.168";
    std::string serverPort = "5000";
    std::string clientPort = "5000";
    const char* fileName = "log.txt";
    std::string msg = OK;

    try {
    	UDPServer server(serverAddress, serverPort);
        std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
        //std::fstream(fileName, std::ios::out | std::ios::trunc) << "Start";
        char buffer[1000];	//11

        while (true) {
        	std::cout << "Sending message" << std::endl;
        	int sent_bytes = server.Send(clientAddress, clientPort, msg, sizeof(msg));
        	std::cout << "Message sent" << std::endl;
        	std::cout << "Waiting for message..." << std::endl;

            int received_bytes = server.Receive(buffer, sizeof(buffer));
            if (received_bytes > 0) {
                buffer[received_bytes] = '\0';
                std::cout << "Received message: " << buffer << std::endl;
                //msg = OK;

                //server.Send(clientAddress, clientPort, msg, sizeof(msg));
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "An exception occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

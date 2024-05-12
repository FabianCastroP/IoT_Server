#include "iot_server.h"
#include <iostream>
#include <unistd.h>

int testUDP1(void);

int main(void) {
    testUDP1();
	return 0;
}

int testUDP1(void) {
	std::string serverAddress = "192.168.1.148";
		std::string clientAddress = "192.168.1.148";
		std::string serverPort = "5000";
		std::string clientPort = "6000";	//Tienen que ser diferentes
		const char* fileName = "log.txt";
		std::string msg;
		int i = 0;

		try {
			UDPServer server(serverAddress, serverPort);
			UDPServer client(clientAddress, clientPort);
			std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
			//std::fstream(fileName, std::ios::out | std::ios::trunc) << "Start";
			char buffer[1000];	// Tiene que ser un array de char

			while (true) {
				std::cout << "Waiting for message..." << std::endl;
				if (i%2) {
					msg = OK;
				} else {
					msg = NOK;
				}
				std::cout << "TEST1" << std::endl;
				client.Send(serverAddress, serverPort, msg, sizeof(msg));
				std::cout << "TEST2" << std::endl;
				int received_bytes = server.Receive(buffer, sizeof(buffer));
				if (received_bytes > 0) {
					std::cout << "Received message: " << buffer << std::endl;
					msg = OK;
					i++;
					usleep(50000);
				}
			}
		} catch (const std::exception &e) {
			std::cerr << "An exception occurred: " << e.what() << std::endl;
			return 1;
		}

		return 0;
}

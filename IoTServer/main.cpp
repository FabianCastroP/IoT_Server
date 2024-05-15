#include <iostream>
#include "UDPCommunication.h"

int testUDP1(void);

int main(void) {
    testUDP1();
	return 0;
}

int testUDP1(void) {
	std::string serverAddress = "192.168.1.24";
	std::string clientAddress = "192.168.1.148";
	std::string serverPort = "5000";
	std::string clientPort = "5000";
	std::string msg;

	try {
		UDPCommunication server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
		//std::fstream(fileName, std::ios::out | std::ios::trunc) << "Start";
		char buffer[1000];	// Tiene que ser un array de char

		while (true) {
			std::cout << "Waiting for message..." << std::endl;
			if (server.Receive(buffer)) {
				std::cout << "Received message: " << buffer << std::endl;
				usleep(50000);
				server.Send(msg);
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

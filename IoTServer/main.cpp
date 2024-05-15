#include "iot_server.h"
#include <iostream>
#include <unistd.h>

#define SERVER_ADDRESS 	"192.168.1.24"
#define SERVER_PORT		"5000"
#define CLIENT_ADDRESS	"192.168.1.148"
#define CLIENT_PORT		"5000"

#define OK 	"OK"
#define NOK "Wrong Message"
#define CLIENT_GREET	"Hello Server"
#define SERVER_GREET	"Hello RPI"

int InitialGreet(void);
void testUDP1(void);
void testUDP2(void);

int main(void) {
    InitialGreet();
	//testUDP2();
    return 0;
}

int InitialGreet(void) {
	std::string serverAddress = SERVER_ADDRESS;
	std::string clientAddress = CLIENT_ADDRESS;
	std::string serverPort = SERVER_PORT;
	std::string clientPort = CLIENT_PORT;
	std::string handshakeSend = SERVER_GREET;
	std::string handshakeRecv = CLIENT_GREET;
	char buffer[15];

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
		std::cout << "Waiting for message..." << std::endl;
		int received_bytes = server.Receive(buffer, sizeof(buffer));
		std::cout << "buffer: " << buffer << std::endl;
		std::cout << "msg: " << handshakeRecv << std::endl;
		std::cout << "cmp: " << (std::string(buffer) == handshakeRecv) << std::endl;
		if (received_bytes > 0) {
			if (std::string(buffer) != handshakeRecv) {
				handshakeSend = NOK;
			}
			std::cout << "Received message: " << std::string(buffer) << std::endl;
			std::cout << "Sending message: " << handshakeSend << " to " << clientAddress << ":" << clientPort << std::endl;
			if (server.Send(clientAddress, serverPort, handshakeSend, sizeof(handshakeSend))) {
				std::cout << "Message sent" << std::endl;
				usleep(500000);
				if (std::string(buffer) == handshakeRecv) {
					std::cout << "Closing socket" << std::endl;
					server.CloseServerSocket();
				}
				return 1;
			}
			std::cout << "Error! Couldn't send message" << std::endl;
			return 0;
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return 0;
	}
	std::cout << "Finished" << std::endl;
	return 0;
}

void testUDP1(void) {
	std::string serverAddress = "192.168.1.24";
	std::string clientAddress = "192.168.1.148";
	std::string serverPort = "5000";
	std::string clientPort = "5000";
	std::string msg = OK;
	char buffer[1000];	//4(6+8+1)

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;

		while (true) {
			std::cout << "Waiting for message..." << std::endl;

			int received_bytes = server.Receive(buffer, sizeof(buffer));
			if (received_bytes > 0) {

				buffer[received_bytes] = '\0';
				std::cout << "Received message: " << buffer << std::endl;
				msg = OK;
				std::cout << "Sending message: " << msg << " to " << clientAddress << ":" << clientPort << std::endl;
				server.Send(clientAddress, clientPort, msg, sizeof(msg));
				std::cout << "Message sent" << std::endl;
				usleep(50000);
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
	}
}

void testUDP2(void) {
	std::string serverAddress = SERVER_ADDRESS;
	std::string clientAddress = CLIENT_ADDRESS;
	std::string serverPort = SERVER_PORT;
	std::string clientPort = CLIENT_PORT;
	std::string msg = SERVER_GREET;
	char buffer[1000];	//4(6+8+1)

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;

		while (true) {
			std::cout << "Waiting for message..." << std::endl;

			int received_bytes = server.Receive(buffer, sizeof(buffer));
			if (received_bytes > 0) {

				buffer[received_bytes] = '\0';
				std::cout << "Received message: " << buffer << std::endl;
				std::cout << "Sending message: " << msg << " to " << clientAddress << ":" << clientPort << std::endl;
				server.Send(clientAddress, clientPort, msg, sizeof(msg));
				std::cout << "Message sent" << std::endl;
				usleep(500000);
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
	}
}

#include "iot_server.h"
#include <iostream>
#include <unistd.h>
#include "math.h"

// RPI
//#define SERVER_ADDRESS 	"192.168.1.168"
//#define CLIENT_ADDRESS	"192.168.1.108"
//Home
#define SERVER_ADDRESS 	"192.168.1.24"
#define CLIENT_ADDRESS	"192.168.1.148"
#define SERVER_PORT		"5000"
#define CLIENT_PORT		"5000"

#define OK 	"OK"
#define NOK "Wrong Message"
#define CLIENT_GREET	"Hello Server"
#define SERVER_GREET	"Hello RPI"
#define NUMBER_OF_SAMPLES 	10
#define NUMBER_OF_VARIABLES 7
#define BYTES_PER_VARIABLE 	2

static double x[NUMBER_OF_SAMPLES*6], y[NUMBER_OF_SAMPLES*6], z[NUMBER_OF_SAMPLES*6];
static uint16_t clear[NUMBER_OF_SAMPLES*6], red[NUMBER_OF_SAMPLES*6], blue[NUMBER_OF_SAMPLES*6], green[NUMBER_OF_SAMPLES*6];

int InitialGreet(void);
void DivideData(const char* rawData, int& cnt);
void ProcessData(void);

int main(void) {
	int cnt = 0;
    InitialGreet();

    std::string serverAddress = SERVER_ADDRESS;
    std::string clientAddress = CLIENT_ADDRESS;
    std::string serverPort = SERVER_PORT;
    std::string clientPort = CLIENT_PORT;
    std::string ack = OK;
    char rawData[NUMBER_OF_VARIABLES*NUMBER_OF_SAMPLES];

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
		while (1) {
			std::cout << "Waiting for message..." << std::endl;
			int received_bytes = server.Receive(rawData, sizeof(rawData));
			std::cout << "Message received" << std::endl;
			std::cout << "Number of bytes received: " << received_bytes << std::endl;
			if ((0 < received_bytes) && (received_bytes < ((NUMBER_OF_VARIABLES*NUMBER_OF_SAMPLES)+1))) {
				std::cout << "Received measures" << std::endl;
				std::cout << "Sending message: " << ack << " to " << clientAddress << ":" << clientPort << std::endl;
				if (server.Send(clientAddress, serverPort, ack, sizeof(ack))) {
					std::cout << "Message sent" << std::endl;
					DivideData(rawData, cnt);
					if (cnt == 6) {
						printf("Processing results...\n");
						ProcessData();
						cnt = 0;
					}
				} else{
					std::cout << "Error! Couldn't send message" << std::endl;
				}
				usleep(50000);
			}
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return 0;
	}
	std::cout << "Finished" << std::endl;
	return 0;
}

int InitialGreet(void) {
	std::string serverAddress = SERVER_ADDRESS;
	std::string clientAddress = CLIENT_ADDRESS;
	std::string serverPort = SERVER_PORT;
	std::string clientPort = CLIENT_PORT;
	std::string handshakeSend = SERVER_GREET;
	std::string handshakeRecv = CLIENT_GREET;
	char buffer[sizeof(handshakeRecv)];

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
		std::cout << "Waiting for message..." << std::endl;
		int received_bytes = server.Receive(buffer, sizeof(buffer));
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
					std::cout << "Closing socket\n\n" << std::endl;
					server.CloseServerSocket();
				}
				return 1;
			}
			std::cout << "Error! Couldn't send message\n\n" << std::endl;
			return 0;
		}
	} catch (const std::exception &e) {
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return 0;
	}
	std::cout << "Finished" << std::endl;
	return 0;
}

void DivideData(const char* rawData, int& cnt) {
	double sensibilityAccel = 2.0/32767.5;
	double sensibilityCOlor = 255 / 65535;

	for (int i = 0; i < 10; ++i) {
		x[i+cnt*NUMBER_OF_SAMPLES] 		= ((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+1]) * sensibilityAccel;
		y[i+cnt*NUMBER_OF_SAMPLES] 		= ((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+2] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+3]) * sensibilityAccel;
		z[i+cnt*NUMBER_OF_SAMPLES] 		= ((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+4] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+5]) * sensibilityAccel;
		clear[i+cnt*NUMBER_OF_SAMPLES] 	= round(((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+6] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+7]) * sensibilityCOlor);
		red[i+cnt*NUMBER_OF_SAMPLES] 		= round(((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+8] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+9]) * sensibilityCOlor);
		blue[i+cnt*NUMBER_OF_SAMPLES] 	= round(((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+10] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+11]) * sensibilityCOlor);
		green[i+cnt*NUMBER_OF_SAMPLES] 	= round(((rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+12] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+13]) * sensibilityCOlor);
	}
	cnt = cnt+1;
}

void ProcessData(void) {
	double mean_x, mean_y, mean_z;
	double min_x, min_y, min_z;
	double max_x, max_y, max_z;
	double std_x, std_y, std_z;
	uint16_t mean_clear, mean_red, mean_blue, mean_green;
	uint16_t min_clear, min_red, min_blue, min_green = 0;
	uint16_t max_clear, max_red, max_blue, max_green = 0;
	double std_clear, std_red, std_blue, std_green;

	for (int i = 0; i < NUMBER_OF_SAMPLES*6; ++i) {
		mean_x += x[i];
		mean_y += y[i];
		mean_z += z[i];
		mean_clear += clear[i];
		mean_red += red[i];
		mean_blue += blue[i];
		mean_green += green[i];

		if (0 == i) {
			min_x = x[i];
			min_y = y[i];
			min_z = z[i];
			min_clear = clear[i];
			min_red = red[i];
			min_blue = blue[i];
			min_green = green[i];

			max_x = x[i];
			max_y = y[i];
			max_z = z[i];
			max_clear = clear[i];
			max_red = red[i];
			max_blue = blue[i];
			max_green = green[i];
		} else {
			min_x = (x[i] < min_x) ? x[i] : min_x;
			min_y = (y[i] < min_y) ? y[i] : min_y;
			min_z = (z[i] < min_z) ? z[i] : min_z;
			min_clear = (clear[i] < min_clear) ? clear[i] : min_clear;
			min_red = (red[i] < min_red) ? red[i] : min_red;
			min_blue = (blue[i] < min_blue) ? blue[i] : min_blue;
			min_green = (green[i] < min_green) ? green[i] : min_green;

			max_x = (x[i] > max_x) ? x[i] : max_x;
			max_y = (y[i] > max_y) ? y[i] : max_y;
			max_z = (z[i] > max_z) ? z[i] : max_z;
			max_clear = (clear[i] > max_clear) ? clear[i] : max_clear;
			max_red = (red[i] > max_red) ? red[i] : max_red;
			max_blue = (blue[i] > max_blue) ? blue[i] : max_blue;
			max_green = (green[i] > max_green) ? green[i] : min_green;
		}
	}
	mean_x = mean_x/(NUMBER_OF_SAMPLES*6);
	mean_y = mean_y/(NUMBER_OF_SAMPLES*6);
	mean_z = mean_z/(NUMBER_OF_SAMPLES*6);
	mean_clear = mean_clear/(NUMBER_OF_SAMPLES*6);
	mean_red = mean_red/(NUMBER_OF_SAMPLES*6);
	mean_blue = mean_blue/(NUMBER_OF_SAMPLES*6);
	mean_green = mean_green/(NUMBER_OF_SAMPLES*6);

	for (int i = 0; i < NUMBER_OF_SAMPLES*6; ++i) {
		std_x = (x[i] - mean_x)*(x[i] - mean_x);
		std_y = (x[i] - mean_y)*(y[i] - mean_y);
		std_z = (z[i] - mean_z)*(z[i] - mean_z);
		std_clear = (clear[i] - mean_clear)*(clear[i] - mean_clear);
		std_red = (red[i] - mean_red)*(red[i] - mean_red);
		std_blue = (blue[i] - mean_blue)*(blue[i] - mean_blue);
		std_green = (green[i] - mean_green)*(green[i] - mean_green);
	}
	std_x = sqrt(std_x/NUMBER_OF_SAMPLES*6);
	std_y = sqrt(std_y/NUMBER_OF_SAMPLES*6);
	std_z = sqrt(std_z/NUMBER_OF_SAMPLES*6);
	std_clear = sqrt(std_clear/NUMBER_OF_SAMPLES*6);
	std_red = sqrt(std_red/NUMBER_OF_SAMPLES*6);
	std_blue = sqrt(std_blue/NUMBER_OF_SAMPLES*6);
	std_green = sqrt(std_green/NUMBER_OF_SAMPLES*6);

	printf("min(x) = %.1lf\n", min_x);
	printf("min(y) = %.1lf\n", min_y);
	printf("min(z) = %.1lf\n", min_z);
	printf("min(clear) = %d\n", min_clear);
	printf("min(red) = %d\n", min_red);
	printf("min(blue) = %d\n", min_blue);
	printf("min(green) = %d\n", min_green);

	printf("max(x) = %.1lf\n", max_x);
	printf("max(y) = %.1lf\n", max_y);
	printf("max(z) = %.1lf\n", max_z);
	printf("max(clear) = %d\n", max_clear);
	printf("max(red) = %d\n", max_red);
	printf("max(blue) = %d\n", max_blue);
	printf("max(green) = %d\n", max_green);

	printf("min(x) = %.1lf\n", mean_x);
	printf("mean(y) = %.1lf\n", mean_y);
	printf("mean(z) = %.1lf\n", mean_z);
	printf("mean(clear) = %d\n", mean_clear);
	printf("mean(red) = %d\n", mean_red);
	printf("mean(blue) = %d\n", mean_blue);
	printf("mean(green) = %d\n", mean_green);

	printf("std(x) = %.1lf\n", std_x);
	printf("std(y) = %.1lf\n", std_y);
	printf("std(z) = %.1lf\n", std_z);
	printf("std(clear) = %.1lf\n", std_clear);
	printf("std(red) = %.1lf\n", std_red);
	printf("std(blue) = %.1lf\n", std_blue);
	printf("std(green) = %.1lf\n", std_green);
}

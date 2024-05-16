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

int InitialGreet(void);
void ProcessData(const char* rawData, int* mean, int16_t* max, int16_t* min, int* std);
double round_to(double value, double precision);
double calculateAccelerationCoordinate( int16_t meausureAcceleration);
uint16_t calculateMeasureColorimeter( uint16_t measureColorimeter);
void testUDP1(void);
void testUDP2(void);

int main(void) {
    InitialGreet();

    std::string serverAddress = SERVER_ADDRESS;
    std::string clientAddress = CLIENT_ADDRESS;
    std::string serverPort = SERVER_PORT;
    std::string clientPort = CLIENT_PORT;
    std::string ack = OK;
    char rawData[NUMBER_OF_VARIABLES*NUMBER_OF_SAMPLES];
    int meanData[NUMBER_OF_VARIABLES] = {0};
    int16_t maxData[NUMBER_OF_VARIABLES] = {0};
    int16_t minData[NUMBER_OF_VARIABLES] = {0};
    int stdData[NUMBER_OF_VARIABLES] = {0};

	try {
		UDPServer server(serverAddress, serverPort);
		std::cout << "UDP server is running on " << server.GetServerAddr() << ":" << server.GetServerPort() << std::endl;
		while (1) {
			std::cout << "Waiting for message..." << std::endl;
			int received_bytes = server.Receive(rawData, sizeof(rawData));
			std::cout << "Message received: " << std::string(rawData) << std::endl;
			std::cout << "Number of bytes received: " << received_bytes << std::endl;
			if ((0 < received_bytes) && (received_bytes < ((NUMBER_OF_VARIABLES*NUMBER_OF_SAMPLES)+1))) {
				std::cout << "Received measures" << std::endl;
				ProcessData(rawData, meanData, maxData, minData, stdData);
				std::cout << "Sending message: " << ack << " to " << clientAddress << ":" << clientPort << std::endl;
				if (server.Send(clientAddress, serverPort, ack, sizeof(ack))) {
					std::cout << "Message sent" << std::endl;
					std::cout << "Printing processed data:" << std::endl;

					std::cout << "Min(x): " << minData[0] << std::endl;
					std::cout << "Min(y): " << minData[1] << std::endl;
					std::cout << "Min(z): " << minData[2] << std::endl;
					std::cout << "Min(clear): " << minData[3] << std::endl;
					std::cout << "Min(red): " << minData[4] << std::endl;
					std::cout << "Min(blue): " << minData[5] << std::endl;
					std::cout << "Min(green): " << minData[6] << std::endl;

					std::cout << "Max(x): " << maxData[0] << std::endl;
					std::cout << "Max(y): " << maxData[1] << std::endl;
					std::cout << "Max(z): " << maxData[2] << std::endl;
					std::cout << "Max(clear): " << maxData[3] << std::endl;
					std::cout << "Max(red): " << maxData[4] << std::endl;
					std::cout << "Max(blue): " << maxData[5] << std::endl;
					std::cout << "Max(green): " << maxData[6] << std::endl;

					std::cout << "Mean(x): " << meanData[0] << std::endl;
					std::cout << "Mean(y): " << meanData[1] << std::endl;
					std::cout << "Mean(z): " << meanData[2] << std::endl;
					std::cout << "Mean(clear): " << meanData[3] << std::endl;
					std::cout << "Mean(red): " << meanData[4] << std::endl;
					std::cout << "Mean(blue): " << meanData[5] << std::endl;
					std::cout << "Mean(green): " << meanData[6] << std::endl;

					std::cout << "Std(x): " << stdData[0] << std::endl;
					std::cout << "Std(y): " << stdData[1] << std::endl;
					std::cout << "Std(z): " << stdData[2] << std::endl;
					std::cout << "Std(clear): " << stdData[3] << std::endl;
					std::cout << "Std(red): " << stdData[4] << std::endl;
					std::cout << "Std(blue): " << stdData[5] << std::endl;
					std::cout << "Std(green): " << stdData[6] << std::endl;

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

void ProcessData(const char* rawData, int* mean, int16_t* max, int16_t* min, int* std) {
	int16_t x[NUMBER_OF_SAMPLES], y[NUMBER_OF_SAMPLES], z[NUMBER_OF_SAMPLES] = {0};
	uint16_t clear[NUMBER_OF_SAMPLES], red[NUMBER_OF_SAMPLES], blue[NUMBER_OF_SAMPLES], green[NUMBER_OF_SAMPLES] = {0};
	int mean_x, mean_y, mean_z, mean_clear, mean_red, mean_blue, mean_green = 0;
	int min_x, min_y, min_z, min_clear, min_red, min_blue, min_green = 0;
	int max_x, max_y, max_z, max_clear, max_red, max_blue, max_green = 0;
	int std_x, std_y, std_z, std_clear, std_red, std_blue, std_green = 0;


	for (int i = 0; i < 10; ++i) {
		x[i] 		= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+1];
		y[i] 		= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+2] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+3];
		z[i] 		= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+4] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+5];
		clear[i] 	= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+6] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+7];
		red[i] 		= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+8] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+9];
		blue[i] 	= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+10] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+11];
		green[i] 	= (rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+12] << 8) | rawData[i*NUMBER_OF_VARIABLES*BYTES_PER_VARIABLE+13];
	}

	for (int i = 0; i < 10; ++i) {
		if (0 == i) {
			min_x = x[i];
			max_x = x[i];
			mean_x = x[i];

			min_y = y[i];
			max_y = y[i];
			mean_y = y[i];

			min_z = z[i];
			max_z = z[i];
			mean_z = z[i];

			min_clear = clear[i];
			max_clear = clear[i];
			mean_clear = clear[i];

			min_red = red[i];
			max_red = red[i];
			mean_red = red[i];

			min_blue = blue[i];
			max_blue = blue[i];
			mean_blue = blue[i];

			min_green = green[i];
			max_green = green[i];
			mean_green = green[i];
		} else {
			if (x[i] <= min_x) {
				min_x = x[i];
			}
			if (x[i] >= max_x) {
				max_x = x[i];
			}

			if (y[i] <= min_y) {
				min_y = y[i];
			}
			if (y[i] >= max_y) {
				max_y = y[i];
			}

			if (z[i] <= min_z) {
				min_z = z[i];
			}
			if (z[i] >= max_z) {
				max_z = z[i];
			}

			if (clear[i] <= min_clear) {
				min_clear = clear[i];
			}
			if (clear[i] >= max_clear) {
				max_clear = clear[i];
			}

			if (red[i] <= min_red) {
				min_red = red[i];
			}
			if (red[i] >= max_red) {
				max_red = red[i];
			}

			if (blue[i] <= min_blue) {
				min_blue = blue[i];
			}
			if (blue[i] >= max_blue) {
				max_blue = blue[i];
			}

			if (green[i] <= min_green) {
				min_green = green[i];
			}
			if (green[i] >= max_green) {
				max_green = green[i];
			}
			mean_x += x[i];
			mean_y += y[i];
			mean_z += z[i];
			mean_clear += clear[i];
			mean_red += red[i];
			mean_blue += blue[i];
			mean_green += green[i];
		}
	}
	mean_x = round_to(mean_x/NUMBER_OF_SAMPLES, 1);
	mean_y = round_to(mean_y/NUMBER_OF_SAMPLES, 1);
	mean_z = round_to(mean_z/NUMBER_OF_SAMPLES, 1);
	mean_clear = round_to(mean_clear/NUMBER_OF_SAMPLES, 1);
	mean_red = round_to(mean_red/NUMBER_OF_SAMPLES, 1);
	mean_blue = round_to(mean_blue/NUMBER_OF_SAMPLES, 1);
	mean_green = round_to(mean_green/NUMBER_OF_SAMPLES, 1);

	for (int i = 0; i < 10; ++i) {
		std_x += (x[i] - mean_x)*(x[i] - mean_x);
		std_y += (y[i] - mean_y)*(y[i] - mean_y);
		std_z += (z[i] - mean_z)*(z[i] - mean_z);
		std_clear += (clear[i] - mean_clear)*(clear[i] - mean_clear);
		std_red += (red[i] - mean_red)*(red[i] - mean_red);
		std_blue += (blue[i] - mean_blue)*(blue[i] - mean_blue);
		std_green += (green[i] - mean_green)*(green[i] - mean_green);
	}

	std_x = round_to(sqrt(std_x/NUMBER_OF_SAMPLES), 1);
	std_y = round_to(sqrt(std_y/NUMBER_OF_SAMPLES), 1);
	std_z = round_to(sqrt(std_z/NUMBER_OF_SAMPLES), 1);
	std_clear = round_to(sqrt(std_clear/NUMBER_OF_SAMPLES), 1);
	std_red = round_to(sqrt(std_red/NUMBER_OF_SAMPLES), 1);
	std_blue = round_to(sqrt(std_blue/NUMBER_OF_SAMPLES), 1);
	std_green = round_to(sqrt(std_green/NUMBER_OF_SAMPLES), 1);

	mean[0] = calculateAccelerationCoordinate(mean_x);
	mean[1] = calculateAccelerationCoordinate(mean_y);
	mean[2] = calculateAccelerationCoordinate(mean_z);
	mean[3] = calculateMeasureColorimeter(mean_clear);
	mean[4] = calculateMeasureColorimeter(mean_red);
	mean[5] = calculateMeasureColorimeter(mean_blue);
	mean[6] = calculateMeasureColorimeter(mean_green);

	min[0] = calculateAccelerationCoordinate(min_x);
	min[1] = calculateAccelerationCoordinate(min_y);
	min[2] = calculateAccelerationCoordinate(min_z);
	min[3] = calculateMeasureColorimeter(min_clear);
	min[4] = calculateMeasureColorimeter(min_red);
	min[5] = calculateMeasureColorimeter(min_blue);
	min[6] = calculateMeasureColorimeter(min_green);

	max[0] = calculateAccelerationCoordinate(max_x);
	max[1] = calculateAccelerationCoordinate(max_y);
	max[2] = calculateAccelerationCoordinate(max_z);
	max[3] = calculateMeasureColorimeter(max_clear);
	max[4] = calculateMeasureColorimeter(max_red);
	max[5] = calculateMeasureColorimeter(max_blue);
	max[6] = calculateMeasureColorimeter(max_green);

	std[0] = calculateAccelerationCoordinate(std_x);
	std[1] = calculateAccelerationCoordinate(std_y);
	std[2] = calculateAccelerationCoordinate(std_z);
	std[3] = calculateMeasureColorimeter(std_clear);
	std[4] = calculateMeasureColorimeter(std_red);
	std[5] = calculateMeasureColorimeter(std_blue);
	std[6] = calculateMeasureColorimeter(std_green);
}

double round_to(double value, double precision) {
    return std::round(value / precision) * precision;
}

double calculateAccelerationCoordinate( int16_t meausureAcceleration){
	double sensibility = 2.0/32767.5; //accelerometer sensitivity
	return round_to(meausureAcceleration*sensibility, 0.01);
}

uint16_t calculateMeasureColorimeter( uint16_t measureColorimeter){
	double sensibility = 255 / 65535; //colorimeter sensitivity

	return round_to(measureColorimeter*sensibility, 1);
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

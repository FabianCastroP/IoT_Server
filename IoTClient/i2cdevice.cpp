#include <iostream>	//
#include <linux/i2c.h>
#include <sys/ioctl.h> 		// Funcion ioctl
#include <linux/i2c-dev.h>  	// Para I2C_SLAVE
#include <fcntl.h> 		// Funcion open
#include <signal.h>
#include <unistd.h> //usleep, close
#include <cstdint> //uint_t
#include "i2cdevice.h"

#define  FILE_DESCRIPTOR "/dev/i2c-"	// Archivo I2C: i2c-1

I2CDevice::I2CDevice(uint8_t deviceNum, uint8_t deviceAddr) 
	: mDevAddr(deviceAddr) {
	std::cout << "OPENNING FILE DESCRIPTOR FOR THE INTERFACE: " << deviceNum << ", ADDRESS SENSOR: " << mDevAddr << std::endl;
	
	//String with the file descriptor path
	mI2CFile = FILE_DESCRIPTOR + std::to_string(deviceNum);

	mFD = -1;
}
	
int I2CDevice::OpenFD(void) {
	//Open the file descriptor
	if (!mI2CFile.empty()) {
		mFD = open(mI2CFile.c_str(), O_RDWR);
		if (-1 == mFD) {
			std::cout << "Error: can't open the file " << mI2CFile << std::endl;
			return 0;
		}
		// Configure the file for I2C communications with slave at the specified address
		if (-1 == ioctl(mFD, I2C_SLAVE, mDevAddr)) {
			std::cout << "ioctl error" << std::endl;
			std::cout << "\tmFD: " << mFD << std::endl;
			std::cout << "\tmDevAddr: " << mDevAddr << std::endl;
			CloseFD();
			return 0;
		}
		std::cout << "CORRECT" << std::endl;
		return 1;
	}
	return 0;
}


int I2CDevice::Write(uint8_t regAddr, uint8_t value) {
	if (-1 != mFD) {
		struct i2c_rdwr_ioctl_data packets;
		struct i2c_msg messages[2];

		uint8_t writeBytes[]  = {regAddr, value};

		//Creamos el mensaje a enviar
		messages[0].addr	= mDevAddr; // Device's address
		messages[0].flags	= 0;		// write
		messages[0].len		= sizeof(writeBytes);	// bytes in buffer
		messages[0].buf		= writeBytes;

		//Creamos el paquete a enviar
		packets.msgs  = messages;
		packets.nmsgs = 2;

		//Enviamos el paquete
		if (-1 == ioctl(mFD, I2C_RDWR, &packets)) {
			std::cout << "ioctl error" << std::endl;
			std::cout << "\tregAddr: " << regAddr << std::endl;
			std::cout << "\tvalue: " << value << std::endl;
			return 0;	// Error
		}
		return 1;
	}
	return 0;
}

int I2CDevice::Read(uint8_t regAddr, uint8_t *value) {
	if (-1 != mFD) {
		struct i2c_rdwr_ioctl_data packets; //
		struct i2c_msg messages[2];

		// Creamos el mensaje a enviar
		messages[0].addr	= mDevAddr; //Establecemos la dirección del esclavo I2C
		messages[0].flags	= 0;	// Establecemos la operacion de escritura
		messages[0].len		= 2;	// Establecemos los bytes que queremos enviar
		messages[0].buf		= &regAddr; //Establecemos los registros de los que queremos leer

		// Creamos el mensaje de lectura
		messages[1].addr	= mDevAddr; //Establecemos la dirección del esclavo I2C
		messages[1].flags	= I2C_M_RD;	//Establecemos la operacion de lectura
		messages[1].len		= 2; //Establecemos los bytes que queremose enviar
		messages[1].buf		= value; //Esblecemos las variables donde se guardan los registros de los que queremos leer


		// Creamos el paquete a enviar
		packets.msgs  = messages; //Mensajes que se enviar
		packets.nmsgs = 2; //Numero de mensajes que contiene el paquete

		// Enviamos el paquete
		if (-1 == ioctl(mFD, I2C_RDWR, &packets)) {
			std::cout << "ioctl error" << std::endl;
			std::cout << "\tmFD: " << mFD << std::endl;
			std::cout << "\tregAddr: " << regAddr << std::endl;
			std::cout << "\tvalue: " << value << std::endl;
			return 0;	// Error
		}
		return 1;
	}
	return 0;
}

int I2CDevice::CloseFD (void)
{
	if(-1 == close(mFD)) {
		std::cout << "close error" << std::endl;
		std::cout << "\tmFD: " << mFD << std::endl;
		return -1;
	}
	return 1;
}




#include "colorimeter.h"
#include <iostream>

/*-------------- ADDRESS ------------------*/
#define COLORIMETER_ADDR 0x29

/*-------------- CONFIGURATION REGISTER ------------------*/
#define PWR_CONFIG 0x80
#define POWER_ON 0x01
#define POWER_OFF uint8_t0x00
#define COLORIMETER_MEASURE 0x03

/*-------------- MEASURE REGISTER ------------------*/
#define CLEAR_L 0x94
#define CLEAR_H 0x95
#define RED_L 0x96
#define RED_H 0x97
#define GREEN_L 0x98
#define GREEN_H 0x99
#define BLUE_L 0x9A
#define BLUE_H 0x9B

Colorimeter::Colorimeter(void)
    : I2CDevice(1, COLORIMETER_ADDR) {
	std::cout << "INTIALIZING COLORIMETER CONFIGURATION" << std::endl;
	
	for (int i = 0; i < 8; ++i) {
		mMeasures[i] = 0;
	}

    //Init the TSC3472 
	if(!I2CDevice::Write(PWR_CONFIG, POWER_ON)){
		std::cout << "Error Colorimeter CONFIGURATION: PWR_CONFIG" << std::endl;
	}
	
	std::cout << "CORRECT" << std::endl;
}

int Colorimeter::Measure(void){
	if(!I2CDevice::Read(CLEAR_H, &mMeasures[0]) || !I2CDevice::Read(CLEAR_L, &mMeasures[1]) ||
	   !I2CDevice::Read(RED_H, &mMeasures[2]) || !I2CDevice::Read(RED_L, &mMeasures[3]) ||
	   !I2CDevice::Read(GREEN_H, &mMeasures[4]) || !I2CDevice::Read(GREEN_L, &mMeasures[5]) ||
	   !I2CDevice::Read(BLUE_H, &mMeasures[6]) || !I2CDevice::Read(BLUE_L, &mMeasures[7])) {
		std::cout << "Error Reading Measures Accelerometer" << std::endl;
		return 0;
	}
	return 1;
}

std::vector<uint8_t> Colorimeter::GetMeasures(void) const {
	return mMeasures;
}

#include "accelerometer.h"
#include <iostream>

/*-------------- ADDRESS ------------------*/
#define	 ACCELEROMETER_ADDR 0x68

#define	 SMPRT_DIV 0x19
#define	 CONFIG 0x1A
#define	 ACCEL_CONFIG 0x1C
#define	 INT_ENABLE 0x38	// Enables interrupt generation by interrupt sources
#define	 X_H_REG 0x3B	// [15:8]
#define	 X_L_REG 0x3C	// [7:0]
#define	 Y_H_REG 0x3D
#define	 Y_L_REG 0x3E
#define	 Z_H_REG 0x3F
#define	 Z_L_REG 0x40
#define  PWR_MGMT_1 0x6B
#define  PWR_MGMT_2 0x6C

Accelerometer::Accelerometer(void)
    : I2CDevice(1, ACCELEROMETER_ADDR) {
	std::cout << "INTIALIZATING ACCELEROMETER CONFIGURATION" << std::endl;
	
	for (int i = 0; i < 6; ++i) {
		mMeasures[i] = 0;
	}

	// Configuration Power management 1
	int device_reset = 0; // [7] Resets all internal registers to their default value (does not work if enabled)
	int sleep = 0;	// [6] Low power mode
	int cycle = 1;	// [5] Cycle between sleep and waking up to take a single sample of data from accel at LP_WAKE_CTRL rate (sleep must be 0)
	int temp_dis = 1;	// [3] Disables temp sensor
	int clksel = 0; // [2:0] 0 = internal 8MHz oscillator Activamos el oscilador interno de 8 MHz
	uint8_t pwr_mgmt1 = (device_reset << 7) | (sleep << 6) | (cycle << 5) | (temp_dis << 3) | (clksel);

	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(PWR_MGMT_1, pwr_mgmt1)) {
		std::cout << "Error Accelerometer Configuration: Power Management 1" << std::endl;	// SE PODRÍA IMPLEMENTAR UNA EXCEPCIÓN PARA ESTE CASO
	}
	
	//Configuration Power management 2
	uint8_t lp_wake_ctrl = 3 << 6; // [7:6] Wake-up frequency = 40Hz = 25ms
	int stby_xa = 0 << 5;	// [5] Low power mode Axis X Accelerometer
	int stby_ya = 0 << 4;	// [4] Low power mode Axis Y Accelerometer
	int stby_za = 0 << 3;	// [3] Low power mode Axis Z Accelerometer
	int stby_xg = 1 << 2;	// [2] Low power mode Axis X Gyroscope
	int stby_yg = 1 << 1;	// [1] Low power mode Axis Y Gyroscope
	int stby_zg = 1;	// [0] Low power mode Axis Z Gyroscope
	uint8_t pwr_mgmt2 = (lp_wake_ctrl) | (stby_xa) | (stby_ya) | (stby_za) | (stby_xg) | (stby_yg) | (stby_zg);
	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(PWR_MGMT_2, pwr_mgmt2)) {
		std::cout << "Error Configuration Accelerometer: Power Management 2" << std::endl;
	}
	
	
	//Specify the sampling rate
	uint8_t smplrt_div = 7;
	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(SMPRT_DIV, smplrt_div)) {
		std::cout << "Error Configuration Accelerometer: Sampling Rate" << std::endl;
	}
	
	
	//Set the bandwidth of the digital low pass filter
	uint8_t dlpf_cfg = 3;
	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(CONFIG, dlpf_cfg)) {
		std::cout << "Error Configuration Accelerometer: Bandwidth of the Digital Low Pass Filter" << std::endl;
	}
	
	
	//Selects the full scale range of the accelerometer outputs	
	uint8_t afs_sel = 0;	
	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(ACCEL_CONFIG, afs_sel)) {
		std::cout << "Error Configuration Accelerometer: Scale Range" << std::endl;
	}
	
	//Activates the interruption of the read value, which occurs every time a write operation is completed (the values that we request to read from a register have been written)
	uint8_t data_rdy_en = 1;	
	//Send the configuration to the I2C interface
	if (!I2CDevice::Write(INT_ENABLE, data_rdy_en)) {
		std::cout << "Error Configuration Accelerometer: Interruption Activation" << std::endl;
	}
	
	std::cout << "CORRECT" << std::endl;
}

int Accelerometer::Measure(void) {
	if(!I2CDevice::Read(X_H_REG, &mMeasures[0]) || !I2CDevice::Read(X_L_REG, &mMeasures[1]) ||
	   !I2CDevice::Read(Y_H_REG, &mMeasures[2]) || !I2CDevice::Read(Y_L_REG, &mMeasures[3]) ||
	   !I2CDevice::Read(Z_H_REG, &mMeasures[4]) || !I2CDevice::Read(Z_L_REG, &mMeasures[5])) {
		std::cout << "Error Reading Measures Accelerometer" << std::endl;
		return 0;
	}
	return 1;
}

std::vector<uint8_t> Accelerometer::GetMeasures(void) const {
	return mMeasures;
}

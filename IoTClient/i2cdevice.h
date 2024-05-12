#ifndef I2CDEVICE_H
	#define I2CDEVICE_H

	#include <string>
				
	class I2CDevice {
		private:
			int 		mFD;
			std::string mI2CFile;
			uint8_t 	mDevAddr;
		public:
			I2CDevice(uint8_t devNum, uint8_t devAddr);
			virtual ~I2CDevice() {}	//Virtual destructor

			int		GetFD(void) const;
			std::string	GetI2CFile(void) const;
			int		GetDevAddr(void) const;

			int 	Write(uint8_t regAddr, uint8_t value);
			int 	Read(uint8_t regAddr, uint8_t *value);
			int 	OpenFD(void);
			int 	CloseFD(void);
	};
#endif

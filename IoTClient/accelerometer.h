#ifndef ACCELEROMETER_H
	#define ACCELEROMETER_H

	#include <cstdint> //uint_t
	#include <vector>

	#include "i2cdevice.h"

	class Accelerometer : public I2CDevice {
		private:
			std::vector<uint8_t> mMeasures;
		public:
			Accelerometer(void);
			virtual ~Accelerometer() {}	//Virtual destructor
			int	 	 Measure(void);
			std::vector<uint8_t> GetMeasures(void) const;
	};
#endif

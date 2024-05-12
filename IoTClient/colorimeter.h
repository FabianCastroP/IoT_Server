#ifndef COLORIMETER_H
	#define COLORIMETER_H
		
	#include "i2cdevice.h"
	#include <cstdint>
	#include <vector>

	class Colorimeter : public I2CDevice {
		private:
			std::vector<uint8_t> mMeasures;
		public:
			Colorimeter(void);
			virtual ~Colorimeter() {}	//Virtual destructor
			int	 	 Measure(void);
			std::vector<uint8_t> GetMeasures(void) const;
	};
#endif

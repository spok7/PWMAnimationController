#include <stdint.h>
#include <Adafruit_PWMServoDriver.h>

#ifndef PWMHANDLER_H
#define PWMHANDLER_H

class PWMHandler 
{
	private:
		uint16_t min_val;
		uint16_t max_val;
		uint16_t offset;  
		uint8_t pin;
		Adafruit_PWMServoDriver *driver;
	protected:
		PWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver);
	public:
		void setPWM(uint16_t val);
		void setOn();
		void setOff();
		void driverOn();
		void driverOff();
		uint16_t getMin();
		uint16_t getMax();
		uint16_t getOffset();
};

class ServoHandler: public PWMHandler {
	public:
		ServoHandler(uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver);
};

class RotServoHandler: public PWMHandler {
	public:
		RotServoHandler(uint8_t pin, Adafruit_PWMServoDriver *driver);
};

class LedHandler: public PWMHandler {
	public:
		LedHandler(uint8_t pin, Adafruit_PWMServoDriver *driver);
};

#endif // PWMHANDLER_H
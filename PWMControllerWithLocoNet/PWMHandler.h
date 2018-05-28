#include <stdint.h>
#include <Adafruit_PWMServoDriver.h>

#ifndef PWMHANDLER_H
#define PWMHANDLER_H


class PWMHandler 
{
	private:
		uint16_t min_val;					// lower bound of PWM values
		uint16_t max_val;					// higher bound of PWM balues
		uint16_t offset;  					// used for setting multiple servos easily
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

// below are the only objects and methods you should use

class ServoHandler: public PWMHandler {
	public:
		ServoHandler(uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver);
};

class RotServoHandler: public PWMHandler {
	public:
		RotServoHandler(uint8_t pin, Adafruit_PWMServoDriver *driver);
};

class LEDHandler: public PWMHandler {
	public:
		LEDHandler(uint8_t pin, Adafruit_PWMServoDriver *driver);
};


#endif // PWMHANDLER_H
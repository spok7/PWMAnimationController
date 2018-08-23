#include <stdint.h>
#include <Adafruit_PWMServoDriver.h>

#ifndef PWMHANDLER_H
#define PWMHANDLER_H


class PWMHandler 
{
	protected:
		uint16_t min_val;					// lower bound of PWM values
		uint16_t max_val;					// higher bound of PWM balues
		uint16_t offset;  					// used for setting multiple servos easily
		uint8_t pin;
	public:
		virtual void setPWM(uint16_t val);
		virtual void kill();
		uint16_t getMin();
		uint16_t getMax();
		uint16_t getOffset();
};

class ArduinoPWMHandler : public PWMHandler
{
	public:
		ArduinoPWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset);
		void setPWM(uint8_t val);
		void kill();
};

class AdafruitPWMHandler : public PWMHandler
{
	private:
		Adafruit_PWMServoDriver *driver;
	public:
		AdafruitPWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver);
		void setPWM(uint16_t val);
		void kill();
		// unused methods below
		void setOn();
		void setOff();
		void driverOn();
		void driverOff();
};


enum HandlerType
{
	SERVO,
	ROTSERVO,
	LED
};

class HandlerFactory
{
	public:
		static ArduinoPWMHandler* createArduinoPWMHandler(HandlerType type, uint8_t pin, uint16_t offset);
		static AdafruitPWMHandler* createAdafruitPWMHandler(HandlerType type, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver);
};


#endif // PWMHANDLER_H
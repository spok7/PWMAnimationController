#include "PWMHandler.h"

PWMHandler::PWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver)
{
	if (max_val > 4095) max_val = 4095;
	if (offset > max_val - min_val || min_val > max_val || pin > 15) return;
	this->min_val = min_val;
	this->max_val = max_val;
	this->offset = offset;
	this->pin = pin;
	this->driver = driver;

	Serial.print(F("    H: Min Val:\t")); Serial.println(min_val);
	Serial.print(F("    H: Max Val:\t")); Serial.println(max_val);
	Serial.print(F("    H: Pin:\t"));     Serial.println(pin);
	Serial.print(F("    H: Offset:\t"));  Serial.println(offset);
	Serial.println(F("    H: Created PWMHandler"));
}

void PWMHandler::setPWM(uint16_t val)
{
	driver->setPWM(pin, 0, val);
	// Serial.print(F("    H: Raw PWM val:\t")); Serial.println(val);
}

void PWMHandler::setOn()
{
	driver->setPWM(pin, 4096, 0);
}

void PWMHandler::setOff()
{
	driver->setPWM(pin, 0, 4096);
}

void PWMHandler::driverOn()
{
	for (int i = 0; i < 16; ++i) {
		driver->setPWM(i, 4096, 0);
	}
}

void PWMHandler::driverOff()
{
	for (int i = 0; i < 16; ++i) {
		driver->setPWM(i, 0, 4096);
	}
}

uint16_t PWMHandler::getMin()
{
	return min_val;
}

uint16_t PWMHandler::getMax()
{
	return max_val;
}

uint16_t PWMHandler::getOffset()
{
	return offset;
}

ServoHandler::ServoHandler(uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver) : PWMHandler(135, 635, pin, offset, driver)
{
	Serial.println(F("    H: New Servo!"));
}

RotServoHandler::RotServoHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : PWMHandler(0, 4095, pin, 0, driver)
{
	Serial.println(F("    H: New Rotary Servo!"));
}

LedHandler::LedHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : PWMHandler(0, 4096, pin, 0, driver) //4096 is completely off
{
	Serial.println(F("    H: New LED!"));
}
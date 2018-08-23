#include "PWMHandler.h"


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

ArduinoPWMHandler::ArduinoPWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset)
{
	if (max_val > 255) max_val = 255;
	if (offset > max_val - min_val || min_val > max_val || pin > 15) return;
	this->min_val = min_val;
	this->max_val = max_val;
	this->offset = offset;
	this->pin = pin;

	Serial.print(F("    H: Min Val:\t")); Serial.println(min_val);
	Serial.print(F("    H: Max Val:\t")); Serial.println(max_val);
	Serial.print(F("    H: Pin:\t"));     Serial.println(pin);
	Serial.print(F("    H: Offset:\t"));  Serial.println(offset);
	Serial.println(F("    H: Created Arduino PWMHandler"));
}

void ArduinoPWMHandler::setPWM(uint8_t val)
{
	analogWrite(pin, val);
}

void ArduinoPWMHandler::kill()
{
	analogWrite(pin, 0);
}


AdafruitPWMHandler::AdafruitPWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver)
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
	Serial.println(F("    H: Created Adafruit PWMHandler"));
}

void AdafruitPWMHandler::setPWM(uint16_t val)
{
	driver->setPWM(pin, 0, val);
	// Serial.print(F("    H: Raw PWM val:\t")); Serial.println(val);
}

void AdafruitPWMHandler::setOn()
{
	driver->setPWM(pin, 4096, 0);
}

void AdafruitPWMHandler::setOff()
{
	driver->setPWM(pin, 0, 4096);
}

void AdafruitPWMHandler::driverOn()
{
	for (int i = 0; i < 16; ++i) {
		driver->setPWM(i, 4096, 0);
	}
}

void AdafruitPWMHandler::driverOff()
{
	for (int i = 0; i < 16; ++i) {
		driver->setPWM(i, 0, 4096);
	}
}

void AdafruitPWMHandler::kill() {
	driverOff();
}

// ServoHandler::ServoHandler(uint8_t pin, uint16_t Offset, Adafruit_PWMServoDriver *driver) : AdafruitPWMHandler(135, 635, pin, offset, driver)
// {
// 	Serial.println(F("    H: Created New Servo"));
// }

// RotServoHandler::RotServoHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : AdafruitPWMHandler(0, 4096, pin, 0, driver) //4096 is completely off
// {
// 	Serial.println(F("    H: Created New Rotary Servo"));
// }

// LEDHandler::LEDHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : AdafruitPWMHandler(0, 4096, pin, 0, driver) //4096 is completely on
// {
// 	Serial.println(F("    H: Created New LED"));
// }


static ArduinoPWMHandler* HandlerFactory::createArduinoPWMHandler(HandlerType type, uint8_t pin, uint16_t offset) {
	ArduinoPWMHandler *handler = NULL;
	switch (type) {
	    case SERVO:
	      handler = new ArduinoPWMHandler(135, 635, pin, offset);
	      break;
	    case ROTSERVO:
	      handler = new ArduinoPWMHandler(0, 4096, pin, 0);
	      break;
	    case LED:
	      handler = new ArduinoPWMHandler(0, 4096, pin, 0);
	}
	return handler;
}

static AdafruitPWMHandler* HandlerFactory::createAdafruitPWMHandler(HandlerType type, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver) {
	AdafruitPWMHandler *handler = NULL;
	switch (type) {
	    case SERVO:
	      handler = new AdafruitPWMHandler(135, 635, pin, offset, driver);
	      break;
	    case ROTSERVO:
	      handler = new AdafruitPWMHandler(0, 4096, pin, 0, driver);
	      break;
	    case LED:
	      handler = new AdafruitPWMHandler(0, 4096, pin, 0, driver);
	}
	return handler;
}
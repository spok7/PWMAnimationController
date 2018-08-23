#include <Adafruit_PWMServoDriver.h>

class PWMHandler {

private:

	uint16_t min_val;
	uint16_t max_val;
	uint16_t offset;  
	uint8_t pin;
	Adafruit_PWMServoDriver *driver;
	
protected:

	PWMHandler(uint16_t min_val, uint16_t max_val, uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver) {
		if (max_val > 4095) max_val = 4095;
		if (offset > max_val - min_val || min_val > max_val || pin > 15) return;
		this->min_val = min_val;
		this->max_val = max_val;
		this->offset = offset;
		this->pin = pin;
		this->driver = driver;

		Serial.print(F("   H: Min Val:\t")); Serial.println(min_val);
		Serial.print(F("   H: Max Val:\t")); Serial.println(max_val);
		Serial.print(F("   H: Pin:\t"));     Serial.println(pin);
		Serial.print(F("   H: Offset:\t"));  Serial.println(offset);
	}
	
public:

	void setPWM(uint16_t base_val) {
		Serial.println("   H: I got to the PWM handler.");
		base_val = min_val + offset + base_val;
		driver->setPWM(pin, 0, (base_val < max_val) ? base_val : max_val);
		Serial.print(F("   H: New PWM val:\t")); Serial.println((base_val < max_val) ? base_val : max_val);
	}
};

class ServoHandler: public PWMHandler {
public:
	ServoHandler(uint8_t pin, uint16_t offset, Adafruit_PWMServoDriver *driver) : PWMHandler(130, 640, pin, offset, driver) {Serial.println(F("New Servo!"));}
};

class RotServoHandler: public PWMHandler {
public:
	RotServoHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : PWMHandler(0, 4095, pin, 0, driver) {Serial.println(F("New Rotary Servo!"));}
};

class LedHandler: public PWMHandler {
public:
	LedHandler(uint8_t pin, Adafruit_PWMServoDriver *driver) : PWMHandler(0, 4095, pin, 0, driver) {Serial.println(F("New LED!"));}
};

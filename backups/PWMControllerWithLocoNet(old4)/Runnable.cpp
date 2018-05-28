#include "Runnable.h"

// creates sequence of PWM values and stores it in a struct
PWMController::PWMEventLink* PWMController::generatePWMEventLink(uint16_t target_pwm, float speed)
{
	PWMEventLink *link = new PWMEventLink;
	link->len = (speed == 0) ? 1 : ceil(abs((int16_t) current_pwm - (int16_t) target_pwm) / speed);
	link->seq = new uint16_t[link->len];

	Serial.print(F("   R: Current:\t"));			Serial.println(current_pwm);
	Serial.print(F("   R: Target:\t"));				Serial.println(target_pwm);
	Serial.print(F("   R: Speed:\t"));				Serial.println(speed);
	Serial.print(F("   R: Sequence Length:\t"));	Serial.println(link->len);
	Serial.println();

	for (int pos = 0; pos < link->len; ++pos) {
		if (abs(current_pwm - target_pwm) < speed || speed == 0) {
			current_pwm = target_pwm;
		} else if (current_pwm < target_pwm) {
			current_pwm += speed;
		} else {
			current_pwm -= speed;
		}

		uint16_t max_pwm = handler->getMax();
		uint16_t modded_pwm = current_pwm + handler->getMin() + handler->getOffset();
		link->seq[pos] = (modded_pwm < max_pwm) ? modded_pwm : max_pwm;
		Serial.print(F("   R: Value: ")); Serial.print(current_pwm); Serial.print(F(" at pos ")); Serial.println(pos);
	}

	Serial.println(F("   R: Generated Link"));
	return link;
}


PWMController::PWMController(PWMHandler *handler)
{
	this->handler = handler;
	root_link = NULL;
	current_link = NULL;
	current_pwm = 0;
	seq_pos = 0;

	Serial.println(F("   R: PWMController Created"));
}

void PWMController::addEvent(uint16_t target, float speed)
{
	Serial.println(F("   R: Adding PWM Event"));
	if (!root_link) {
		root_link = generatePWMEventLink(target, speed);
		current_link = root_link;
	} else {
		PWMEventLink *traversal = root_link;
		while (traversal->next) traversal = traversal->next;
		traversal->next = generatePWMEventLink(target, speed);
	}

	Serial.println(F("   R: Added PWM Event"));
}

bool PWMController::run()
{
	// Serial.print(F("   R: Current Time:")); Serial.print(millis());
	handler->setPWM(current_link->seq[seq_pos]);
	if (++seq_pos == current_link->len) {
		seq_pos = 0;
		if (!(current_link = current_link->next)) current_link = root_link;
		return false;
	}
	return true;
}

PWMHandler* PWMController::getHandler()
{
	return handler;
}

void PWMController::kill()
{
	handler->driverOff();
}


Delay::Delay(unsigned long delay)
{
	this->delay = delay;
	target_time = 0;
	Serial.println(F("   R: Delay Created"));
}

bool Delay::run()
{
	if (target_time == 0) target_time = millis() + delay;
	// Serial.print(F("   R: Current Time:")); Serial.print(millis()); Serial.print(F("\tWaiting for:")); Serial.println(target_time);
	if (target_time < millis()) {
		target_time = 0;
		return false;
	}
	return true;
}


Pin::Pin(uint8_t pin, uint8_t value)
{
	this->pin = pin;
	this->value = value;
	pinMode(pin, OUTPUT);
	Serial.println(F("   R: Pin Created"));
}

bool Pin::run()
{
	digitalWrite(pin, value);
	return false;
}
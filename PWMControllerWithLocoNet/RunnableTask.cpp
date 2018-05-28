#include "RunnableTask.h"

// creates sequence of PWM values and stores it in a struct
PWMTask::PWMSequence* PWMTask::generatePWMSequence(uint16_t target_pwm, float speed)
{

	PWMSequence *link = new PWMSequence;
	// prevent speeds that cause an integer overflow, and lengths of 0
	link->len = (speed < 0.0626) ? 0.626 : ((current_pwm == target_pwm) ? 1 : ceil(abs((int16_t) current_pwm - (int16_t) target_pwm) / speed));
	link->seq = new uint16_t[link->len];

	Serial.print(F("   R: Current:\t"));			Serial.println(current_pwm);
	Serial.print(F("   R: Target:\t"));				Serial.println(target_pwm);
	Serial.print(F("   R: Speed:\t"));				Serial.println(speed);
	Serial.print(F("   R: Sequence Length:\t"));	Serial.println(link->len);

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


// constructor
PWMTask::PWMTask(PWMHandler *handler)
{
	this->handler = handler;
	root_link = NULL;
	current_link = NULL;
	current_pwm = 0;
	seq_pos = 0;

	Serial.println(F("   R: PWMTask Created"));
}

// create and add a PWM sequence to the linked list
void PWMTask::addSequence(uint16_t target, float speed)
{

	Serial.println(F("   R: Adding PWM Task"));
	if (!root_link) {
		root_link = generatePWMSequence(target, speed);
		current_link = root_link;
	} else {
		PWMSequence *traversal = root_link;
		while (traversal->next) traversal = traversal->next;
		traversal->next = generatePWMSequence(target, speed);
	}

	Serial.println(F("   R: Added PWM Task"));
}

// runs sequence, returns false when done
bool PWMTask::run()
{
	// Serial.print(F("   R: Pos:")); Serial.print(seq_pos);
	handler->setPWM(current_link->seq[seq_pos]);
	if (++seq_pos == current_link->len) {
		seq_pos = 0;
		if (!(current_link = current_link->next)) current_link = root_link;
		return false;
	}
	return true;
}

PWMHandler* PWMTask::getHandler()
{
	return handler;
}

void PWMTask::kill()
{
	handler->driverOff();
}


// constructor
DelayTask::DelayTask(unsigned long delay)
{
	this->delay = delay;
	target_time = 0;
	Serial.println(F("   R: Delay Task Created"));
}

// waits an amount, returns false when done
bool DelayTask::run()
{
	if (target_time == 0) target_time = millis() + delay;
	// Serial.print(F("   R: Current Time:")); Serial.print(millis()); Serial.print(F("\tWaiting for:")); Serial.println(target_time);
	if (target_time < millis()) {
		target_time = 0;
		return false;
	}
	return true;
}


// constructor
PinTask::PinTask(uint8_t pin, uint8_t value)
{
	this->pin = pin;
	this->value = value;
	pinMode(pin, OUTPUT);
	Serial.println(F("   R: Pin Task Created"));
}

// changes pin, allways done
bool PinTask::run()
{
	digitalWrite(pin, value);
	return false;
}
#include "RunnableTask.h"

TaskType RunnableTask::getType()
{
	return type;
}

// creates sequence of PWM values and stores it in a struct
PWMTask::PWMSequence* PWMTask::generatePWMSequence(uint16_t target_pwm, float speed)
{

	PWMSequence *link = new PWMSequence;
	// prTaskLink speeds that cause an integer overflow, and lengths of 0
	// TODO - change magic values to 1 and 1?
	// old values: 0.0636 and 0.62
	link->len = (speed < 0.0626) ? 0.0626 : ((current_pwm == target_pwm) ? 1 : ceil(abs((int16_t) current_pwm - (int16_t) target_pwm) / speed));
	link->seq = new uint16_t[link->len];

	Serial.print(F("   R: Current:\t"));			Serial.println(current_pwm);
	Serial.print(F("   R: Target:\t"));				Serial.println(target_pwm);
	Serial.print(F("   R: Speed:\t"));				Serial.println(speed);
	Serial.print(F("   R: Sequence Length:\t"));	Serial.println(link->len);

	for (uint16_t pos = 0; pos < link->len; ++pos) {
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
	this->root_link = NULL;
	this->current_link = NULL;
	this->current_pwm = 0;
	this->seq_pos = 0;
	this->type = PWM;

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
	handler->kill();
}


// constructor
DelayTask::DelayTask(unsigned long delay)
{
	this->delay = delay;
	this->target_time = 0;
	this->type = DELAY;

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
	this->type = PIN;
	pinMode(pin, OUTPUT);

	Serial.println(F("   R: Pin Task Created"));
}

// changes pin, allways done
bool PinTask::run()
{
	digitalWrite(pin, value);
	return false;
}


StepperTask::StepperSequence::StepperSequence(bool d, uint8_t sd) : reverse_dir(d), step_delay(sd), last_run_time(0), next(NULL) {}

bool StepperTask::StepperCountSequence::setup(uint8_t step_pin, uint8_t dir_pin)
{
	digitalWrite(dir_pin, (reverse_dir ? HIGH : LOW));
	delay(1);
	digitalWrite(step_pin, HIGH);
	delay(1);
	digitalWrite(step_pin, LOW);
	return false;
}

bool StepperTask::StepperCountSequence::isDone()
{
	if (current_step == amount) {
		current_step = 0;
		return true;
	} else {
		++current_step;
		return false;			
	}
}

StepperTask::StepperCountSequence::StepperCountSequence(uint8_t a, bool d, uint8_t sd) : amount(a), current_step(0), StepperSequence(d, sd) {}

bool StepperTask::StepperSenseSequence::setup(uint8_t step_pin, uint8_t dir_pin)
{
	digitalWrite(dir_pin, (reverse_dir ? HIGH : LOW));
	uint8_t sensor_val = analogRead(sensor_pin);
	if (sensor_val >= value) increaseSensorToValue = false;
	digitalWrite(step_pin, HIGH);
	delay(1);
	digitalWrite(step_pin, LOW);
	uint8_t sensor_val2 = analogRead(sensor_pin);
	return ((value < sensor_val && sensor_val < sensor_val2) || (value > sensor_val && sensor_val > sensor_val2));
}

bool StepperTask::StepperSenseSequence::isDone()
{
	return (increaseSensorToValue && analogRead(sensor_pin) >= value) || (not increaseSensorToValue && analogRead(sensor_pin) <= value);
}

StepperTask::StepperSenseSequence::StepperSenseSequence(uint8_t v, bool isv, bool d, uint8_t sd) : value(v), increaseSensorToValue(isv), StepperSequence(d, sd) {}


StepperTask::StepperTask(uint8_t step_pin, uint8_t dir_pin)
{
	this->step_pin = step_pin;
	this->dir_pin = dir_pin;
	this->root_link = NULL;
	this->current_link = NULL;
	this->type = STEPPER;
}

void StepperTask::addStepAmount(uint8_t amount, bool reverse_dir, uint16_t step_delay)
{
	if (!root_link) {
		root_link = new StepperCountSequence(amount, reverse_dir, step_delay);
		current_link = root_link;
	} else {
		StepperSequence *traversal = root_link;
		while (traversal->next) traversal = traversal->next;
		traversal->next = new StepperCountSequence(amount, reverse_dir, step_delay);
	}
}

void StepperTask::addStepUntil(uint8_t analogPin, uint16_t analogValue, bool reverse_dir, uint16_t step_delay)
{
	if (!root_link) {
		root_link = new StepperSenseSequence(analogPin, analogValue, reverse_dir, step_delay);
		current_link = root_link;
	} else {
		StepperSequence *traversal = root_link;
		while (traversal->next) traversal = traversal->next;
		root_link = new StepperSenseSequence(analogPin, analogValue, reverse_dir, step_delay);
	}
}

void StepperTask::step(StepperSequence *seq)
{
	if (seq->last_run_time + seq->step_delay <= millis()) {
		digitalWrite(step_pin, HIGH);
		delay(1);
		digitalWrite(step_pin, LOW);
	}

	seq->last_run_time = millis();
}

bool StepperTask::run()
{
	if (new_sequence) {
		if (current_link->setup(step_pin, dir_pin) || current_link->isDone()) {
			if (not (current_link = current_link->next)) current_link = root_link;
			return false;
		} else {
			new_sequence = false;
			return true;
		}
	} else {
		step(current_link);
	}

	if (current_link->isDone()) {
		new_sequence = true;
		if (not (current_link = current_link->next)) current_link = root_link;
		return false;
	}

	return true;
}
#include "PWMHandler.cpp"

class RunnableTask {
public:
	virtual bool run();
};

class PWMController : public RunnableTask {

private:

	struct PWMEventLink {
		uint16_t *seq;
		uint16_t len; // allows for a slow mode of up to 10 repetitions
		PWMEventLink *next = NULL;
	};

	PWMEventLink *root_link;
	PWMEventLink *current_link;
	PWMHandler *handler;
	uint16_t current_pwm;
	uint16_t seq_pos;

	PWMEventLink *generatePWMEventLink(uint16_t target_pwm, uint16_t speed) {
		PWMEventLink *link = new PWMEventLink;
		link->len = (speed == 0) ? 1 : ceil(abs((int16_t) current_pwm - (int16_t) target_pwm) / speed);
		link->seq = new uint16_t[link->len];

		Serial.print(F("  R: Current:\t"));			Serial.println(current_pwm);
		Serial.print(F("  R: Target:\t"));			Serial.println(target_pwm);
		Serial.print(F("  R: Speed:\t"));			Serial.println(speed);
		Serial.print(F("  R: Sequence Length:\t"));	Serial.println(link->len);
		Serial.println();

		for (int pos = 0; pos < link->len; ++pos) {
			if (abs(current_pwm - target_pwm) < speed || speed == 0) {
				current_pwm = target_pwm;
			} else if (current_pwm < target_pwm) {
				current_pwm += speed;
			} else if (current_pwm > target_pwm) {
				current_pwm -= speed;
			} else {
				Serial.println(F("  R: Error, sequence_length is too long."));
			}

			link->seq[pos] = current_pwm;
			Serial.print(F("  R: Value: ")); Serial.print(current_pwm); Serial.print(F(" at pos ")); Serial.println(pos);
		}

		Serial.println(F("  R: Generated Link"));
		return link;
	}

public:

	PWMController(PWMHandler *handler) {
		this->handler = handler;
		root_link = NULL;
		current_link = NULL;
		current_pwm = 0;
		seq_pos = 0;

		Serial.println(F("  R: PWMController Created"));
	}

	void addEvent(uint16_t target, uint16_t speed) {
		Serial.println(F("  R: Adding PWM Event"));
		if (!root_link) {
			root_link = generatePWMEventLink(target, speed);
			current_link = root_link;
		} else {
			PWMEventLink *traversal = root_link;
			while (traversal->next) traversal = traversal->next;
			traversal->next = generatePWMEventLink(target, speed);
		}

		Serial.println(F("  R: Added PWM Event"));
	}

	bool run() {
		handler->setPWM(current_link->seq[seq_pos]);
		if (++seq_pos == current_link->len) {
			seq_pos = 0;
			if (!(current_link = current_link->next)) current_link = root_link;
			return false;
		}
		return true;
	}

	PWMHandler *getHandler() {
		return handler;
	}
};

class Delay : public RunnableTask {

private:

	unsigned long start_time;
	unsigned long delay;

public:

	Delay(unsigned long delay) {
		this->delay = delay;
		start_time = 0;
		Serial.println(F("  R: Delay Created"));
	}

	bool run() {
		if (!start_time) start_time = millis();
		Serial.print(F("  E: Current Time:")); Serial.print(millis()); Serial.print(F("\tWaiting for:")); Serial.println(start_time + delay);
		if (start_time + delay < millis()) {
			start_time = 0;
			return false;
		}
		return true;
	}
};
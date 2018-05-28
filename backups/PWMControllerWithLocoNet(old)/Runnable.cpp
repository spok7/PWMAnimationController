class RunnableTask {
public:
	virtual bool run();
};

class PWMController : public RunnableTask {

private:

	struct PWMEventLink {
		uint16_t *seq;
		uint16_t len; // allows for a slow mode of up to 10 repetitions
		PWMEventLink *next;
		PWMEventLink() : next(NULL) {}
	};

	PWMEventLink *root_event;
	PWMEventLink *current_event;
	PWMHandler *handler;
	uint16_t current_pwm;
	uint16_t seq_pos;

	PWMEventLink generatePWMEventLink(uint16_t target_pwm, uint16_t speed) {
		PWMEventLink event;
		uint16_t len = (speed == 0) ? 1 : ceil(abs((int16_t) current_pwm - target_pwm) / speed);
		uint16_t *seq = new uint16_t[len];

		Serial.print(F("  E: Current:\t"));			Serial.println(current);
		Serial.print(F("  E: Target:\t"));			Serial.println(target);
		Serial.print(F("  E: Speed:\t"));			Serial.println(speed);
		Serial.print(F("  E: Sequence Length:\t"));	Serial.println(sequence_length);
		Serial.println();

		while(seq_pos < len) {
			if (abs(current_pwm - target_pwm) < speed || speed == 0) {
				current_pwn = target_pwm;
			} else if (current_pwm < target_pwm) {
				current_pwm += speed;
			} else if (current_pwm > target_pwm) {
				current_pwm -= speed;
			} else {
				Serial.println(F("  E: Error, sequence_length is too long."));
			}

			seq[seq_pos] = current_pwm;
			++seq_pos;

			Serial.print(F("  E: Value: ")); Serial.print(current); Serial.print(F(" at pos ")); Serial.println(pos - 1);
		}

		return event;
	}

public:

	PWMController(PWMHandler handler) {
		this->handler = handler;
		root_event = NULL;
		current_event = NULL;
		current_pwm = 0;
		seq_pos = 0;
	}

	void addEvent(uint16_t target, uint16_t speed) {
		if (!root_event) {
			root_event = generatePWMEventLink(target, speed);
			current_event = root_event;
		} else {
			PWMEventLink *traversal = root_branch;
			while (traversal->next) traversal = traversal->next;
			traversal->next = generatePWMEventLink(target, speed);
		}
	}

	bool run() {
		handler->setPWM(current_event->seq[seq_pos]);
		if (++seq_pos == current_event->len) {
			seq_pos = 0;
			return false;
		}
		return true;
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
}
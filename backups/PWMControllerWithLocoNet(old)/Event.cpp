#include "PWMHandler.cpp"

class Event {
protected:
	uint16_t last_val = 65535;
public:
	virtual bool runEvent() {}
	virtual void print();
	getLastVal() {return last_val};
};


class PWMEvent: public Event {

private:

	PWMHandler *pwm;
	uint16_t *sequence;
	uint32_t sequence_length;
	uint32_t pos;

	void generatePositionsSequcence(uint16_t current, uint16_t target, uint32_t speed) {
		sequence_length = (speed == 0) ? 1 : ceil(abs((int32_t) current - target) / speed);
		sequence = new uint16_t[sequence_length];

		Serial.print(F("  E: Current:\t"));			Serial.println(current);
		Serial.print(F("  E: Target:\t"));			Serial.println(target);
		Serial.print(F("  E: Speed:\t"));			Serial.println(speed);
		Serial.print(F("  E: Sequence Length:\t"));	Serial.println(sequence_length);
		Serial.println();

		while(pos < sequence_length) {
			if (abs(current - target) < speed || speed == 0) {
				current = target;
			} else if (current < target) {
				current += speed;
			} else if (current > target) {
				current -= speed;
			} else {
				Serial.println(F("  E: Error, sequence_length is too long."));
			}

			sequence[pos] = current;
			++pos;

			Serial.print(F("  E: Value: ")); Serial.print(current); Serial.print(F(" at pos ")); Serial.println(pos - 1);
		}
		last_val = sequence[pos - 1];

		pos = 0;
	}

public:

	PWMEvent(PWMHandler *pwm, uint16_t current, uint16_t target, uint16_t speed) {
		pos = 0;
		generatePositionsSequcence(current, target, speed);			
		this->pwm = pwm; 
		Serial.println(F("  E: Created PE"));
	}

	bool runEvent() {
		Serial.println(F("  E: I got to the PWMEvent."));
		pwm->setPWM(sequence[pos]);
		if (pos == sequence_length - 1) {
			pos = 0;
			return false;
		} else {
			++pos;
			return true;
		}
	}

	void print() {
		Serial.println(F("E:PWMEvent"));
	}

	PWMHandler *getHandler() {
		return pwm;
	}
};

class PWMEventScheduler: public Event {
	
}


class DelayEvent: public Event {

private:

	unsigned long start_time;
	unsigned long delay;

public:

	DelayEvent(unsigned long delay) {
		Serial.println(F("  E: Creating DE"));
		this->delay = delay;
		start_time = 0;
		Serial.println(F("  E: Created DE"));
	}

	bool runEvent() {
		if (!start_time) start_time = millis();
		Serial.print(F("  E: Current Time:")); Serial.print(millis()); Serial.print(F("\tWaiting for:")); Serial.println(start_time + delay);
		if (start_time + delay < millis()) {
			start_time = 0;
			return false;
		}
		return true;
	}

	void print() {
		Serial.println(F("E:DelayEvent"));
	}
};

#include "Event.cpp"

class Animation {
private:
	Animation *next;
public:
	virtual bool runAnimation() {}
	Animation *getNext() {return next;}
	void setNext(Animation *nextAnimation) {next = nextAnimation;}
};


class AnimationChain : public Animation {

private:

	struct event_link {
		Event *event;
		event_link *next;
		event_link(Event *e) : event(e), next(NULL) {}
	};

	event_link *root_event, *current_event;

	struct Command {
		Runnable *event;
		Command *next;
		Command(Runnable *e) : event(e), next(NULL) {}
	}

	struct PWMDevice {
		PWMController *controller;
		PWMDevice *next;
		Command(PWMController *c) : controller(c), next(NULL) {}
	}

	uint16_t last_pwm;

	void setLastEvent(Event *event) {
		Serial.print(F(" A: Adding ")); event->print();
		if (!root_event) {
			Serial.println(F(" A: First Element"));
			root_event = new event_link(event);
			Serial.print(F(" A: Added ")); root_event->event->print();
		} else {
			Serial.println(F(" A: Next Element"));
			event_link *traversal = root_event;
			while (traversal->next) {
				traversal->next->event->print();
				traversal = traversal->next;
			}
			traversal->next = new event_link(event);
			Serial.print(F(" A: Added ")); traversal->next->event->print();
		}
	}

	uint16_t getLastPos(PWMHandler *pwm) {
		uint16_t last_pos = 0;
		if (root_event) {
			event_link *traversal = root_event;
			do {
				if (traversal->event->getLastVal < 4096 && traversal->event->getHandler() == pwm) {
					last_pos = checker.getLastVal();
				}
				traversal = traversal->next;
			} while (traversal->next);
		}
		return last_pos;
	}

public:

	AnimationChain() {
		root_event = NULL;
		current_event = NULL;
	}

	void addPWM(PWMHandler *pwm, uint16_t pos, uint32_t speed) {
		Serial.println(F(" A: Adding PE"));
		setLastEvent(new PWMEvent(pwm, last_pwm, pos, speed));
		last_pwm = pos;
	}

	void addDelay(long delay) {
		Serial.println(F(" A: Adding DE"));
		setLastEvent(new DelayEvent(delay));
	}

	bool runAnimation() {
		Serial.println(" A: I got to the animation");
		if (!current_event) {
			current_event = root_event;
			Serial.print(" A: I set the current event to ");
			current_event->event->print();
		}
		if (!current_event->event->runEvent()) {
			return (current_event = current_event->next) != 0;
		}
		return true;
	}

	void printEvents() {
		Serial.println("Printing events:");
		if (root_event) {
			Serial.print("- ");
			root_event->event->print();
			event_link *traversal = root_event;
			while(traversal->next) {
				traversal = traversal->next;
				Serial.print("- ");
				traversal->event->print();
			}
		}
		Serial.println("Done pringing events.");
	}
};


class AnimationNode : public Animation {

private:

	struct branch {
		Animation *ani;
		bool loopable;
		bool ran;
		branch *next;
		branch(Animation *a, double loop) : ani(a), loopable(loop), ran(false) {}
	};

	branch *root_branch;

public:

	AnimationNode() {}

	void addAnimation(Animation *ani, bool loopable = false) {
		if (!root_branch) {
			root_branch = new branch(ani, loopable);
		} else {
			branch *traversal = root_branch;
			while (traversal->next) traversal = traversal->next;
			traversal->next = new branch(ani, loopable);
		}
	}

	bool runAnimation() {
		Serial.println(F(" A: I'm a node, lol."));
		bool animation_complete = true;
		for (branch *traversal = root_branch; traversal; traversal = traversal->next) {
			if ((traversal->loopable || !traversal->ran) && !traversal->ani->runAnimation()) traversal->ran = true;
			if (!traversal->ran) animation_complete = false;
		}
		if (animation_complete) for (branch *traversal = root_branch; traversal; traversal = traversal->next) traversal->ran = false;
		return animation_complete;
	}
};

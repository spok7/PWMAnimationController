#include "Runnable.cpp"

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

	struct Event {
		RunnableTask *event;
		Event *next;
		Event(RunnableTask *e) : event(e), next(NULL) {}
	};

	Event *root_event, *current_event;

	struct PWMDevice {
		PWMController *controller;
		PWMDevice *next;
		PWMDevice(PWMController *c) : controller(c), next(NULL) {}
	};

	PWMDevice *root_device;

	void appendEvent(RunnableTask *event) {
		if (!root_event) {
			root_event = new Event(event);
			current_event = root_event;
		} else {
			Event *traversal = root_event;
			while (traversal->next) traversal = traversal->next;
			traversal->next = new Event(event);
		}
		Serial.println(F(" A: Appended Event"));
	}

public:

	AnimationChain() {
		root_event = NULL;
		current_event = NULL;
		root_device = NULL;
		Serial.println(F(" A: Initialized AnimationChain"));
	}

	void addPWM(PWMHandler *pwm, uint16_t pos, uint32_t speed = 0) {
		Serial.println(F(" A: Adding PE"));
		if (!root_device) {
			root_device = new PWMDevice(new PWMController(pwm));
			root_device->controller->addEvent(pos, speed);
			appendEvent(root_device->controller);
		} else {
			PWMDevice *traversal = root_device;

			do if (traversal->controller->getHandler() == pwm) break;
			while (traversal = traversal->next);

			if (!traversal) traversal = new PWMDevice(new PWMController(pwm));
			traversal->controller->addEvent(pos, speed);
			appendEvent(traversal->controller);
		}
	}

	void addDelay(long delay) {
		Serial.println(F(" A: Adding DE"));
		appendEvent(new Delay(delay));
	}

	bool runAnimation() {
		if (!current_event) current_event = root_event;
		return (current_event->event->run() || (current_event = current_event->next));
	}
};


class AnimationNode : public Animation {

private:

	struct branch {
		Animation *ani;
		bool loopable;
		bool running;
		bool complete;
		branch *next;
		branch(Animation *a, double loop) : ani(a), loopable(loop), running(true), complete(false), next(NULL) {}
	};

	branch *root_branch = NULL;

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
		Serial.println(F(" AN: Appended Animation to Node"));
	}

	bool runAnimation() {
		Serial.println(F(" AN: Running Node"));
		bool animation_complete = true;
		for (branch *traversal = root_branch; traversal; traversal = traversal->next) {

			if (!traversal->complete && !(traversal->running = traversal->ani->runAnimation())) {
				traversal->complete = true;
				if (traversal->loopable) {
					for (branch *traversal2 = root_branch; traversal2; traversal2 = traversal2->next) {
						if (!traversal->loopable && traversal->running) {
							traversal->complete = false;
							animation_complete = false;
							break;
						}
					}
				}
			}

			// if (traversal->running || traversal->loopable) traversal->running = traversal->ani->runAnimation();
			// if (traversal->running) animation_complete = false;
		}
		
		if (animation_complete) for (branch *traversal = root_branch; traversal; traversal = traversal->next) {
			traversal->running = true;
			traversal->complete = false;
		}

		return !animation_complete;
	}
};
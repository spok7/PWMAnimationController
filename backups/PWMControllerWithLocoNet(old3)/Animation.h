#include "Runnable.h"

class Animation {
	private:
		Animation *next;
	public:
		virtual bool runAnimation();
		virtual void killPWM();
		Animation *getNext();
		void setNext(Animation *nextAnimation);
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
		void appendEvent(RunnableTask *event);
	public:
		AnimationChain();
		void addPWM(PWMHandler *pwm, uint16_t pos, float speed);
		void addDelay(long delay);
		void addPin(uint8_t pin, uint8_t value);
		bool runAnimation();
		void killPWM();
};


class AnimationNode : public Animation {
private:
	struct Branch {
		Animation *ani;
		bool running;
		Branch *next;
		Branch(Animation *a) : ani(a), running(true), next(NULL) {}
	};
	Branch *non_loopable_root;
	Branch *loopable_root;
public:
	AnimationNode();
	void addAnimation(Animation *ani, bool loopable = false);
	bool runAnimation();
	void killPWM();
};
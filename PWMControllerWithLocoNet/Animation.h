#include "RunnableTask.h"

#ifndef ANIMATION_H
#define ANIMATION_H


class Animation : public RunnableTask {
	public:
		virtual void killPWM();
};


class AnimationChain : public Animation {
	private:
		struct Event {
			RunnableTask *task;
			Event *next;
			Event(RunnableTask *t) : task(t), next(NULL) {}
		};
		Event *root_event, *current_event;
		struct PWMDevice {
			PWMTask *controller;
			PWMDevice *next;
			PWMDevice(PWMTask *c) : controller(c), next(NULL) {}
		};
		PWMDevice *root_device;
		void appendTask(RunnableTask *event);
	public:
		AnimationChain();
		void addPWM(PWMHandler *pwm, uint16_t pos, float speed);	// bounds for pos depend on handler; lowest speed is 0.0626
		void addDelay(long delay);
		void addPin(uint8_t pin, uint8_t value);					// pin must be valid on board, value is either HIGH or LOW
		void addAnimation(Animation *ani);

		bool run();													// do not use locally
		void killPWM();												// do not use locally
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
	void addAnimation(Animation *ani, bool loopable = false);		// if loopable, ani will restart if non-loopable anis still running

	bool run();														// do not use locally
	void killPWM();													// do not use locally
};


#endif // ANIMATION_H
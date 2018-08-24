#include "RunnableTask.h"

#ifndef ANIMATION_H
#define ANIMATION_H


class Animation : public RunnableTask {
	public:
		virtual void killPWM();
};


class AnimationChain : public Animation {
	private:
		struct TaskLink {
			RunnableTask *task;
			TaskLink *next;
			TaskLink(RunnableTask *t) : task(t), next(NULL) {}
		};
		TaskLink *root_task, *current_task;
		void appendTask(RunnableTask *task);
	public:
		AnimationChain();
		void addPWM(PWMHandler *pwm, uint16_t pos, float speed);	// bounds for pos depend on handler; lowest speed is 0.0626
		void addDelay(long delay);
		void addPin(uint8_t pin, uint8_t value);					// pin must be valid on board, value is either HIGH or LOW
		void addStepper(uint8_t step_pin, uint8_t dir_pin, uint8_t amount, bool reverse_dir, uint16_t step_delay);			// both pins must be unique to one Stepper else no task is created
		void addStepperWithSensor(uint8_t step_pin, uint8_t dir_pin, uint8_t sensor_pin, uint8_t target_value, bool reverse_dir, uint16_t step_delay);
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
#include "PWMHandler.h"

#ifndef RUNNABLETASK_H
#define RUNNABLETASK_H


class RunnableTask {
	public:
		virtual bool run();
};

class PWMTask : public RunnableTask
{
	private:
		struct PWMSequence {
			uint16_t *seq;
			uint16_t len; 										// allows for a slow mode of up to around 16 repetitions
			PWMSequence *next = NULL;
		};
		PWMSequence *root_link;
		PWMSequence *current_link;
		PWMHandler *handler;
		float current_pwm;
		uint16_t seq_pos;
		PWMSequence *generatePWMSequence(uint16_t target_pwm, float speed);
	public:
		PWMTask(PWMHandler *handler);
		void addSequence(uint16_t target, float speed);			// speeds less than 0.0626 will be set to 0.0626
		PWMHandler *getHandler();
		void kill();
		bool run();
};

class DelayTask : public RunnableTask
{
	private:
		unsigned long target_time;
		unsigned long delay;
	public:
		DelayTask(unsigned long delay);
		bool run();
};

class PinTask : public RunnableTask
{
	private:
		uint8_t pin;
		uint8_t value;
	public:
		PinTask(uint8_t pin, uint8_t value);
		bool run();
};


#endif // RUNNABLETASK_H
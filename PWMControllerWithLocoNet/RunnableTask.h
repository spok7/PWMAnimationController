#include "PWMHandler.h"

#ifndef RUNNABLETASK_H
#define RUNNABLETASK_H


enum TaskType {
	PWM,
	PIN,
	DELAY,
	STEPPER
};

class RunnableTask
{
	protected:
		TaskType type;
	public:
		TaskType getType();
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

class StepperTask : public RunnableTask
{
	private:
		uint8_t step_pin;
		uint8_t dir_pin;
		bool new_sequence;

		struct StepperSequence {
			uint8_t step_delay;
			unsigned long last_run_time;
			bool reverse_dir;
			StepperSequence *next;
			virtual bool setup(uint8_t step_pin, uint8_t dir_pin);
			virtual bool isDone();
			StepperSequence(bool d, uint8_t sd);
		};

	protected:
		StepperSequence *root_link;
		StepperSequence *current_link;

		struct StepperCountSequence : StepperSequence {
			uint8_t amount;
			uint8_t current_step;

			bool setup(uint8_t step_pin, uint8_t dir_pin);
			bool isDone();
			StepperCountSequence(uint8_t a, bool d, uint8_t sd);
		};

		struct StepperSenseSequence : StepperSequence {
			uint8_t sensor_pin;
			uint8_t value;
			bool increaseSensorToValue;

			bool setup(uint8_t step_pin, uint8_t dir_pin);
			bool isDone();

			StepperSenseSequence(uint8_t v, bool isv, bool d, uint8_t sd);
		};

		void step(StepperSequence *seq);

	public:
		StepperTask(uint8_t step_pin, uint8_t dir_pin);
		void addStepAmount(uint8_t amount, bool reverse_dir, uint16_t step_delay);
		void addStepUntil(uint8_t analogPin, uint16_t analogValue, bool reverse_dir, uint16_t step_delay);
		bool run();
};


#endif // RUNNABLETASK_H
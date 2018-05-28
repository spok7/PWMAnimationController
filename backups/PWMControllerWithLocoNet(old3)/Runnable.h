#include "PWMHandler.h"

class RunnableTask {
	public:
		virtual bool run();
};

class PWMController : public RunnableTask
{
	private:
		struct PWMEventLink {
			uint16_t *seq;
			uint16_t len; // allows for a slow mode of up to 10 repetitions
			PWMEventLink *next = NULL;
		};
		PWMEventLink *root_link;
		PWMEventLink *current_link;
		PWMHandler *handler;
		float current_pwm;
		uint16_t seq_pos;
		PWMEventLink *generatePWMEventLink(uint16_t target_pwm, float speed);
	public:
		PWMController(PWMHandler *handler);
		void addEvent(uint16_t target, float speed);
		PWMHandler *getHandler();
		void kill();
		bool run();
};

class Delay : public RunnableTask
{
	private:
		unsigned long target_time;
		unsigned long delay;
	public:
		Delay(unsigned long delay);
		bool run();
};

class Pin : public RunnableTask
{
	private:
		uint8_t pin;
		uint8_t value;
	public:
		Pin(uint8_t pin, uint8_t value);
		bool run();
};
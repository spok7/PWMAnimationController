#include "Animation.h"


// add task to chain of events
void AnimationChain::appendTask(RunnableTask *task)
{
	if (!root_event) {
		root_event = new Event(task);
		current_event = root_event;
	} else {
		Event *traversal = root_event;
		while (traversal->next) traversal = traversal->next;
		traversal->next = new Event(task);
	}
	Serial.println(F("  AC: Appended Event"));
}

AnimationChain::AnimationChain()
{
	root_event = NULL;
	current_event = NULL;
	root_device = NULL;
	Serial.println(F("  AC: Initialized AnimationChain"));
}

// creates and adds a PWM task with the given parameters
void AnimationChain::addPWM(PWMHandler *pwm, uint16_t pos, float speed)
{
	Serial.println(F("  AC: Adding PWM"));
	if (!root_device) {
		root_device = new PWMDevice(new PWMTask(pwm));
		root_device->controller->addSequence(pos, speed);
		appendTask(root_device->controller);
	} else {
		PWMDevice *traversal = root_device;

		do if (traversal->controller->getHandler() == pwm) break;
		while (traversal = traversal->next);

		if (!traversal) traversal = new PWMDevice(new PWMTask(pwm));
		traversal->controller->addSequence(pos, speed);
		appendTask(traversal->controller);
	}
}

// creates and adds a delay task with the given parameters
void AnimationChain::addDelay(long delay)
{
	Serial.println(F("  AC: Adding Delay"));
	appendTask(new DelayTask(delay));
}

// creates and adds a pin task with the given parameters
void AnimationChain::addPin(uint8_t pin, uint8_t value)
{
	Serial.println(F("  AC: Adding Pin"));
	appendTask(new PinTask(pin, value));
}

// adds an existing animation as a task to the event list
void AnimationChain::addAnimation(Animation *ani)
{
	Serial.println(F("  AC: Adding Animation"));
	appendTask(ani);
}

// runs the animation, returns false when no more moves events are left to run
bool AnimationChain::run()
{
	if (!current_event) current_event = root_event;
	return (current_event->task->run() || (current_event = current_event->next));
}

// disables all pwm values
void AnimationChain::killPWM() {
	Serial.println(F("  AC: Killing All PWM Values"));
	for (PWMDevice *traversal = root_device; traversal; traversal = traversal->next) traversal->controller->kill();
}




AnimationNode::AnimationNode()
{
	non_loopable_root = NULL;
	loopable_root = NULL;
	Serial.println(F("  AN: Created Node"));
}

// adds an existing animation as a task to its respective linked list
void AnimationNode::addAnimation(Animation *ani, bool loopable = false)
{
	if (loopable) {
		if (!loopable_root) {
			loopable_root = new Branch(ani);
		} else {
			Branch *traversal = loopable_root;
			while (traversal->next) traversal = traversal->next;
			traversal->next = new Branch(ani);
		}
	} else {
		if (!non_loopable_root) {
			non_loopable_root = new Branch(ani);
		} else {
			Branch *traversal = non_loopable_root;
			while (traversal->next) traversal = traversal->next;
			traversal->next = new Branch(ani);
		}
	}
	Serial.println(F("  AN: Appended Animation to Node"));
}

// runs animations, returns false when all have completed
bool AnimationNode::run()
{
	// run all unfinished non-loopable animations
	bool non_loopable_complete = true;
	for (Branch *traversal = non_loopable_root; traversal; traversal = traversal->next) {
		if (traversal->running) {
			traversal->running = traversal->ani->run();
			non_loopable_complete = false;
		}
	}

	// run all loopable animations
	bool animation_complete = true;
	if (non_loopable_complete) {
		for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
			if (traversal->running) {
				traversal->running = traversal->ani->run();
				animation_complete = false;
			}
		}
	} else { // run all unfinished loopable animations
		animation_complete = false;
		for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
			traversal->running = traversal->ani->run();
		}
	}
	
	// if all animations are ocmplete / have stopped running, reset their status to incomplete and return false
	if (animation_complete) {
		for (Branch *traversal = non_loopable_root; traversal; traversal = traversal->next) {
			traversal->running = true;
		}
		for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
			traversal->running = true;
		} 
	}

	return !animation_complete;
}


// disables all pwm values
void AnimationNode::killPWM()
{
	Serial.println(F("  AN: Killing All PWM Values"));
	
	for (Branch *traversal = non_loopable_root; traversal; traversal = traversal->next) {
		traversal->ani->killPWM();
	}
	for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
		traversal->ani->killPWM();
	}
}
#include "Animation.h"


// add task to chain of TaskLinks
void AnimationChain::appendTask(RunnableTask *task)
{
	if (!root_task) {
		root_task = new TaskLink(task);
		current_task = root_task;
	} else {
		TaskLink *traversal = root_task;
		while (traversal->next) traversal = traversal->next;
		traversal->next = new TaskLink(task);
	}
	Serial.println(F("  AC: Appended TaskLink"));
}

AnimationChain::AnimationChain()
{
	root_task = NULL;
	current_task = NULL;
	Serial.println(F("  AC: Initialized AnimationChain"));
}

// creates and adds a PWM task with the given parameters
void AnimationChain::addPWM(PWMHandler *pwm, uint16_t pos, float speed)
{
	Serial.println(F("  AC: Adding PWM"));
	if (!root_task) {
		root_task = new TaskLink(new PWMTask(pwm));
		((PWMTask *) root_task->task)->addSequence(pos, speed);
	} else {
		TaskLink *traversal = root_task;

		do if (traversal->task->getType() == PWM && ((PWMTask *) traversal->task)->getHandler() == pwm) break;
		while ((traversal = traversal->next));

		if (!traversal) traversal = new TaskLink(new PWMTask(pwm));
		((PWMTask *) traversal->task)->addSequence(pos, speed);
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

// adds an existing animation as a task to the TaskLink list
void AnimationChain::addAnimation(Animation *ani)
{
	Serial.println(F("  AC: Adding Animation"));
	appendTask(ani);
}

// runs the animation, returns false when no more moves TaskLinks are left to run
bool AnimationChain::run()
{
	if (!current_task) current_task = root_task;
	return (current_task->task->run() || (current_task = current_task->next));
}

// disables all pwm values
void AnimationChain::killPWM() {
	Serial.println(F("  AC: Killing All PWM Values"));
	for (TaskLink *traversal = root_task; traversal; traversal = traversal->next) ((PWMTask *) traversal->task)->kill();
}




AnimationNode::AnimationNode()
{
	non_loopable_root = NULL;
	loopable_root = NULL;
	Serial.println(F("  AN: Created Node"));
}

// adds an existing animation as a task to its respective linked list
void AnimationNode::addAnimation(Animation *ani, bool loopable)
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
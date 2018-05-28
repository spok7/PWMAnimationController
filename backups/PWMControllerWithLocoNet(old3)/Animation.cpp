#include "Animation.h"

Animation* Animation::getNext()
{
	return next;
}

void Animation::setNext(Animation *nextAnimation)
{
	next = nextAnimation;
}


void AnimationChain::appendEvent(RunnableTask *event)
{
	if (!root_event) {
		root_event = new Event(event);
		current_event = root_event;
	} else {
		Event *traversal = root_event;
		while (traversal->next) traversal = traversal->next;
		traversal->next = new Event(event);
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

void AnimationChain::addPWM(PWMHandler *pwm, uint16_t pos, float speed)
{
	Serial.println(F("  AC: Adding PE"));
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

void AnimationChain::addDelay(long delay)
{
	Serial.println(F("  AC: Adding DE"));
	appendEvent(new Delay(delay));
}

void AnimationChain::addPin(uint8_t pin, uint8_t value)
{
	Serial.println(F("  AC: Adding Pin"));
	appendEvent(new Pin(pin, value));
}

bool AnimationChain::runAnimation()
{
	if (!current_event) current_event = root_event;
	return (current_event->event->run() || (current_event = current_event->next));
}

void AnimationChain::killPWM() {
	Serial.println(F("Killing All PWM Values"));
	for (PWMDevice *traversal = root_device; traversal; traversal = traversal->next) traversal->controller->kill();
	Serial.println(F("Killed one set of values."));
}




AnimationNode::AnimationNode()
{
	non_loopable_root = NULL;
	loopable_root = NULL;
	Serial.println(F("  AN: Created Node"));
}

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

bool AnimationNode::runAnimation()
{
	// Serial.println(F("  AN: Running Node"));

	bool non_loopable_complete = true;
	for (Branch *traversal = non_loopable_root; traversal; traversal = traversal->next) {
		if (traversal->running) {
			traversal->running = traversal->ani->runAnimation();
			non_loopable_complete = false;
		}
	}

	bool animation_complete = true;
	if (non_loopable_complete) {
		for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
			if (traversal->running) {
				traversal->running = traversal->ani->runAnimation();
				animation_complete = false;
			}
		}
	} else {
		animation_complete = false;
		for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
			traversal->running = traversal->ani->runAnimation();
		}
	}
	
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

void AnimationNode::killPWM() {
	for (Branch *traversal = non_loopable_root; traversal; traversal = traversal->next) {
		traversal->ani->killPWM();
	}
	for (Branch *traversal = loopable_root; traversal; traversal = traversal->next) {
		traversal->ani->killPWM();
	}
}
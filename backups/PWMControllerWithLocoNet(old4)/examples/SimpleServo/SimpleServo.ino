#include "../../LNInterface.h"

LNInterface *controller;

void setup()
{
	Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40);
	pwm->begin();
	pwm->setPWMFreq(60);

	ServoHandler *s1 = new ServoHandler(0, 0, pwm);
	AnimationChain *a1 = new AnimationChain();
	a1->addPWM(s1, 100, 100);
	a1->addPWM(s1, 200, 1);
	a1->addDelay(200);
	a1->addPWM(s1, 100, 1);
	a1->addDelay(200);

	controller = new LNInterface(46, 9002, 16);
	controller->map(a1, 0, true);
}

void loop()
{
	controller->update();
}
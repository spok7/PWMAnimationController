#include <LNInterface.h>

LNInterface *controller;

void setup()
{
	Serial.begin(9600);
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

	AnimationChain *a2 = new AnimationChain();
	a2->addPWM(s1, 100, 1);
	a2->addPWM(s1, 0, 1);

	AnimationChain *a3 = new AnimationChain();
	a3->addAnimation(a1);
	a3->addAnimation(a2);

	controller = new LNInterface(46, 9002, 16);
	controller->mapAnimation(a1, 0, false);
	controller->mapAnimation(a2, 1, true);
	controller->mapAnimation(a3, 2, false);
	controller->mapButton(8, 0);
}

void loop()
{
	controller->update();
}
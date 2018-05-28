#include <LNInterface.h>

LNInterface *controller;

void setup()
{
	Serial.begin(9600);
	Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40);
	pwm->begin();
	pwm->setPWMFreq(60);

	LEDHandler *l1 = new LEDHandler(2, pwm);
	AnimationChain *a1 = new AnimationChain();					
	a1->addPWM(l1, 4096, 100);
	a1->addDelay(500);
	a1->addPWM(l1, 0, 100);

	controller = new LNInterface(46, 9002, 16);
	controller->mapAnimation(a1, 0, false);
	controller->mapButton(8, 0);
}

void loop()
{
	controller->update();
}
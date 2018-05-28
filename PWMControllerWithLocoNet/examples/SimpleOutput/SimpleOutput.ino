#include <LNInterface.h>

LNInterface *controller;

void setup()
{
	Serial.begin(9600);
	Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40);
	pwm->begin();
	pwm->setPWMFreq(60);

	AnimationChain *a1 = new AnimationChain();					
	a1->addPin(12, HIGH);
	a1->addDelay(500);
	a1->addPin(12, LOW);

	controller = new LNInterface(46, 9002, 16);
	controller->mapAnimation(a1, 0, false);
	controller->mapButton(8, 0);
}

void loop()
{
	controller->update();
}
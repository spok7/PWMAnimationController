#include "LNInterface.h"
#include <MemoryFree.h>

LNInterface *controller;

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
	Serial.println(F("-----------SETUP BEGAN-----------"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	
	Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40);
	pwm->begin();
	pwm->setPWMFreq(60);

	// ServoHandler *s1 = new ServoHandler(0, 0, pwm);
	// AnimationChain *a1 = new AnimationChain();
	// a1->addPWM(s1, 100, 1);								// set values between 0 and 500
	// a1->addDelay(1000);
	// a1->addPWM(s1, 200, 1);
	// a1->addDelay(2000);

	ServoHandler *s2 = new ServoHandler(1, 0, pwm);
	AnimationChain *a2 = new AnimationChain();
	a2->addPWM(s2, 50, 0.5);
	a2->addPWM(s2, 0, 0.5);

	// AnimationNode *n1 = new AnimationNode();
	// n1->addAnimation(a1, false);
	// n1->addAnimation(a2, true);

	controller = new LNInterface(46, 9002, 16);				// delay of 16 is recommended to get 62.5 fps, 32 for 31.25;
	// controller->mapAnimation(a1, 0, false);
	controller->mapAnimation(a2, 1, true);
	// controller->mapAnimation(n1, 2, false);
	// controller->printRunningSpeeds();

	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	Serial.println(F("-----------SETUP ENDED-----------"));
}

void loop() {
	controller->update();
}

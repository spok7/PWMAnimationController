#include "LNInterface.cpp"
#include <MemoryFree.h>

LNInterface *controller;

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	Serial.println(F("-------------I AM THE MIGHTY TROGDOR!!!-----------------"));
	Serial.println(F("-----------------FEAR MY WRATH!!!-------------------"));

	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	
	Adafruit_PWMServoDriver *pwm = new Adafruit_PWMServoDriver(0x40);
	ServoHandler *s1 = new ServoHandler(0, 0, pwm);
	Serial.println(F("Servo Declared\n"));

	// AnimationChain *a1 = new AnimationChain();
	// Serial.println(F("Animation Declared\n"));
	// Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // a1->printEvents();

	// a1->addPWM(s1, 100, 1);
	// Serial.println(F("PWM Event added\n"));
	// Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // a1->printEvents();

	// a1->addDelay(1000);
	// Serial.println(F("Delay Event added\n"));
	// Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // a1->printEvents();

	// a1->addPWM(s1, 200, 2);
	// Serial.println("PWM Event added\n");
	// Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // a1->printEvents();

	// a1->addDelay(2000);
	// Serial.println(F("Delay Event added\n"));
	// Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // a1->printEvents();

	// // Serial.println(F("\nRunnign Animation\n"));
	// // a1->runAnimation();
	// // Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	// // while(a1->runAnimation());

	// Serial.println(F("Creating Node"));
	// ServoHandler *s2 = new ServoHandler(0, 0, pwm);
	// AnimationChain *a2 = new AnimationChain();
	// a2->addPWM(s2, 300, 10);
	// a2->addPWM(s2, 0, 10);
	// AnimationNode *n1 = new AnimationNode();
	// n1->addAnimation(a1, false);
	// n1->addAnimation(a2, true);

	// // while(n1->runAnimation());

	Serial.println("Creating and initializing controller.");
	controller = new LNInterface(46, 9002);
	// controller->map(a1, 0, false);
	// controller->map(a2, 1, true);
	// controller->map(n1, 2, false);

	Serial.println(F("potato"));
	Serial.println(F("-----------SETUP ENDED-----------"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
}

void loop() {
	controller->update();
}

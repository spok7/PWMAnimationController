#include "Animation.cpp"
#include <MemoryFree.h>

void setup() {
  // put your setup code here, to run once:
	Serial.begin(9600);
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	Serial.println(F("-------------I AM THE MIGHTY TROGDOR!!!-----------------"));
	Serial.println(F("-----------------FEAR MY WRATH!!!-------------------"));

	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	
	Adafruit_PWMServoDriver *pwm;
	ServoHandler *s1 = new ServoHandler(0, 0, pwm);
	Serial.println(F("Servo Declared\n"));

	AnimationChain *a1 = new AnimationChain(NULL);
	Serial.println(F("Animation Declared\n"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	a1->printEvents();

	a1->addPWM(s1, 100, 1);
	Serial.println(F("PWM Event added\n"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	a1->printEvents();

	a1->addDelay(1000);
	Serial.println(F("Delay Event added\n"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	a1->printEvents();

	a1->addPWM(s1, 200, 2);
	Serial.println("PWM Event added\n");
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	a1->printEvents();

	a1->addDelay(2000);
	Serial.println(F("Delay Event added\n"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	a1->printEvents();

	Serial.println(F("\nRunnign Animation\n"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
	while(a1->runAnimation()) delay(100);
	Serial.println(F("potato"));
	Serial.println(F("-----------SEQUENCE ENDED-----------"));
	Serial.print(F("Free Memory: ")); Serial.println(freeMemory());
}

void loop() {
  // put your main code here, to run repeatedly:
	
}

#include "PWMHandler.cpp"

#define SERVOMIN  130 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  640 // this is the 'maximum' pulse length count (out of 4096)
// absolute low is about 137, absolute high is around 600

#define NUM_SERVOS 4
#define NUM_POS 5

Adafruit_PWMServoDriver pwm;
ServoHandler *servos;
uint16_t *positions;

void setup() {

  servos = new ServoHandler[NUM_SERVOS] {
    ServoHandler(0, 0, &pwm),
    ServoHandler(0, 1, &pwm),
    ServoHandler(0, 2, &pwm),
    ServoHandler(0, 3, &pwm)
  };

  positions = new uint16_t[NUM_POS] {0, 30, 60, 90, 120};

  pwm = Adafruit_PWMServoDriver(0x40);
  pwm.begin();
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates
}

void loop() {
  for (uint8_t i = 0; i < NUM_POS; ++i) {
    for (uint8_t j = 0; j < NUM_SERVOS; ++j) {
      servos[j].setOffsetPWM(positions[i]);
    }
    delay(1000);
  }
}

#include "pca.hpp"


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);


void init_servo() {
    pwm.begin();
    pwm.setPWMFreq(60);
}


void set_servo_position(uint8_t id, uint8_t pos) {
    pwm.setPWM(id, 0, map(pos, 0, 180, SERVO_PWMMIN, SERVO_PWMMAX));
}

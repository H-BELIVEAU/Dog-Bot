#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


#define SERVO_PWMMIN 150
#define SERVO_PWMMAX 600

#define SERVO_FL_SHOULDER   0
#define SERVO_FL_ELBOW      1
#define SERVO_FL_ANKLE      2
#define SERVO_FR_SHOULDER   3
#define SERVO_FR_ELBOW      4
#define SERVO_FR_ANKLE      5
#define SERVO_BL_SHOULDER   6
#define SERVO_BL_ELBOW      7
#define SERVO_BL_ANKLE      8
#define SERVO_BR_SHOULDER   9
#define SERVO_BR_ELBOW      10
#define SERVO_BR_ANKLE      11


extern Adafruit_PWMServoDriver pwm;


bool init_servo();
void set_servo_position(uint8_t id, uint8_t pos);

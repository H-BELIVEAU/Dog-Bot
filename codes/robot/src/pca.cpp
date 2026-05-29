#include "pca.hpp"

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);
ServoState servos[16];


// --- Fonctions internes ---

bool isShoulder(uint8_t id) {
    return (id == SERVO_FL_SHOULDER || id == SERVO_FR_SHOULDER || id == SERVO_BL_SHOULDER || id == SERVO_BR_SHOULDER);
}

uint16_t angleToPWM(uint8_t id, uint8_t angle) {
    if (isShoulder(id)) return map(angle, 0, 180, SERVO_PWMMIN_MG946R, SERVO_PWMMAX_MG946R);
    return map( angle, 0, 180, SERVO_PWMMIN_MG996R, SERVO_PWMMAX_MG996R);
}

void applyPWM(uint8_t id, uint8_t angle) {
    uint16_t pwmValue = angleToPWM(id, angle);
    pwm.setPWM(id, 0, pwmValue);
}



// --- Fonctions externes ---

bool init_servo() {

    Wire.begin();

    if (!pwm.begin()) return false;

    pwm.setPWMFreq(50);
    delay(10);

    for (uint8_t i = 0; i < 16; i++) {
        servos[i].current = 90;
        servos[i].target = 90;

        servos[i].released = true;
        servos[i].lastUpdate = 0;

        pwm.setPWM(i, 0, 0);
        delay(2);
    }

    return true;
}

void release_servo(uint8_t id) {
    if (id >= 16) return;

    servos[id].released = true;
    pwm.setPWM(id, 0, 0);
}

void set_servo_target(uint8_t id, int angle) {
    if (id >= 16) return;

    angle = constrain(angle, 0, 180);

    servos[id].target = angle;
    servos[id].released = false;
}


void update_servos() {
    uint32_t now = millis();
    uint8_t movedServos = 0;

    for (uint8_t i = 0; i < 16; i++) {
        if (movedServos >= MAX_MOVING_SERVOS_PER_FRAME) return;

        ServoState& s = servos[i];

        if (s.released)                                     continue;
        if ((now - s.lastUpdate) < SERVO_UPDATE_INTERVAL)   continue;
        if (s.current == s.target)                          continue;

        int delta = s.current - s.target;

        if (s.current < s.target && delta > SERVO_STEP)     s.current += SERVO_STEP;
        else if (delta > SERVO_STEP)                        s.current -= SERVO_STEP;
        else                                                s.current = s.target;

        applyPWM(i, s.current);

        s.lastUpdate = now;
        movedServos++;
    }
}
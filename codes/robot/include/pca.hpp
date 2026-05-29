#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>


// Limites déterminées des bornes PWM
#define SERVO_PWMMIN_MG996R 120
#define SERVO_PWMMAX_MG996R 665

#define SERVO_PWMMIN_MG946R 130
#define SERVO_PWMMAX_MG946R 450


#define SERVO_RELEASED 255              // Valeur spéciale spécifiant le relachement des servos
#define SERVO_STEP 1                    // Vitesse du pas de déplacement servo
#define SERVO_UPDATE_INTERVAL 12        // Intervale de temps entre deux mises à jours
#define MAX_MOVING_SERVOS_PER_FRAME 2   // Nombre max de servos pouvant bouger à la même frame


// Connexions servo <-> canal PCA9685
#define SERVO_BL_ANKLE      0
#define SERVO_BL_ELBOW      1
#define SERVO_BL_SHOULDER   3

#define SERVO_BR_ANKLE      4
#define SERVO_BR_ELBOW      5
#define SERVO_BR_SHOULDER   6

#define SERVO_FL_ANKLE      8
#define SERVO_FL_ELBOW      9
#define SERVO_FL_SHOULDER   10

#define SERVO_FR_ANKLE      12
#define SERVO_FR_ELBOW      13
#define SERVO_FR_SHOULDER   14


extern Adafruit_PWMServoDriver pwm;


struct ServoState {
    uint8_t current;
    uint8_t target;

    bool released;

    uint32_t lastUpdate;
};

extern ServoState servos[16];


// Fonctions externes

bool init_servo();
void set_servo_target(uint8_t id, int angle);
void release_servo(uint8_t id);
void update_servos();

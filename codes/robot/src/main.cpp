#include <Arduino.h>

#include "nrf.hpp"
#include "pca.hpp"


char buffer[32];
char key[16];
char rawdata[32];


bool parsePacket(const char* input) {
    if (strncmp(input, ":::", 3) != 0) return false;

    const char* first = strchr(input + 3, ':');
    if (!first) return false;

    size_t keyLen = first - (input + 3);
    if (keyLen >= sizeof(key)) return false;

    memcpy(key, input + 3, keyLen);
    key[keyLen] = '\0';

    strncpy(rawdata, first + 1, sizeof(rawdata) - 1);
    rawdata[sizeof(rawdata) - 1] = '\0';

    return true;
}

uint8_t get_servo_id(char servo_name[3]) {
    bool front = servo_name[0] == 'F';
    bool left  = servo_name[1] == 'L';

    uint8_t joint;

    if (servo_name[2] == 'S')       joint = 0;
    else if (servo_name[2] == 'A')  joint = 1;
    else                            joint = 2;

    uint8_t servoId;

    if (front && left && joint == 0)        servoId = SERVO_FL_SHOULDER;
    else if (front && left && joint == 1)   servoId = SERVO_FL_ANKLE;
    else if (front && left)                 servoId = SERVO_FL_ELBOW;

    else if (front && joint == 0)           servoId = SERVO_FR_SHOULDER;
    else if (front && joint == 1)           servoId = SERVO_FR_ANKLE;
    else if (front)                         servoId = SERVO_FR_ELBOW;

    else if (left && joint == 0)            servoId = SERVO_BL_SHOULDER;
    else if (left && joint == 1)            servoId = SERVO_BL_ANKLE;
    else if (left)                          servoId = SERVO_BL_ELBOW;

    else if (joint == 0)                    servoId = SERVO_BR_SHOULDER;
    else if (joint == 1)                    servoId = SERVO_BR_ANKLE;
    else                                    servoId = SERVO_BR_ELBOW;

    return servoId;
}


void processSingleServo() {
    uint8_t servoId = get_servo_id(key+6);
    
    int angle = atoi(rawdata);
    set_servo_target(servoId, angle);
}

void processServoAll() {
    if (strlen(rawdata) < 36) return;

    uint8_t ids[12] = {
        SERVO_FL_SHOULDER,
        SERVO_FL_ANKLE,
        SERVO_FL_ELBOW,

        SERVO_FR_SHOULDER,
        SERVO_FR_ANKLE,
        SERVO_FR_ELBOW,

        SERVO_BL_SHOULDER,
        SERVO_BL_ANKLE,
        SERVO_BL_ELBOW,

        SERVO_BR_SHOULDER,
        SERVO_BR_ANKLE,
        SERVO_BR_ELBOW
    };

    char tmp[4];

    for (uint8_t i = 0; i < 12; i++) {

        memcpy(tmp, &rawdata[i * 3], 3);
        tmp[3] = '\0';

        int angle = atoi(tmp);

        set_servo_target(ids[i], angle);
    }
}


void processPacket() {
    if (strncmp(key, "Servo_", 6) == 0) {
        if (strncmp(key, "Servo_ALL", 9) == 0)  processServoAll();
        else                                    processSingleServo();
        return;
    }

    // Positions prédéfinies (couché, assis, debout, lever la patte)
    if (strncmp(key, "POSITION", 8) == 0) {
        if (strncmp(rawdata, "DOWN", 4) == 0) { // couché
            set_servo_target(SERVO_FL_SHOULDER, 120);
            set_servo_target(SERVO_FR_SHOULDER, 120);
            set_servo_target(SERVO_BL_SHOULDER, 120);
            set_servo_target(SERVO_BR_SHOULDER, 120);

            set_servo_target(SERVO_FL_ANKLE, 30);
            set_servo_target(SERVO_FR_ANKLE, 30);
            set_servo_target(SERVO_BL_ANKLE, 30);
            set_servo_target(SERVO_BR_ANKLE, 30);

            set_servo_target(SERVO_FL_ELBOW, 135);
            set_servo_target(SERVO_FR_ELBOW, 135);
            set_servo_target(SERVO_BL_ELBOW, 135);
            set_servo_target(SERVO_BR_ELBOW, 135);
        }

        return;
    }

    if (strncmp(key, "RELEASE", 7) == 0) {
        if (strncmp(rawdata, "ALL", 3) == 0) {
            for (uint8_t i = 0; i < 16; i++)
                release_servo(i);
            return;
        } else {
            release_servo(get_servo_id(rawdata));
            return;
        }
    }
}


// --- Fonctions Arduino ---

void setup() {

    Serial.begin(115200);
    Serial.println("\n[DOGBOT] STARTING");

    bool ok_nrf   = init_NRF();
    bool ok_servo = init_servo();

    while (!ok_nrf || !ok_servo) {
        if (!ok_nrf)    ok_nrf = init_NRF();
        if (!ok_servo)  ok_servo = init_servo();
        delay(1000);
    }

    Serial.println("[DOGBOT] READY");
    send_NRF("[DOGBOT] READY");
}

void loop() {
    if (read_NRF(buffer)) {
        if (parsePacket(buffer))
            processPacket();
    }

    update_servos();
}

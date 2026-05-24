#include <Arduino.h>

#include "nrf.hpp"
#include "pca.hpp"


char buffer[32];

char key[11];
char type[9];
char rawdata[12];

bool parsePacket(const char* input) {
    if (strncmp(input, ":::", 3) != 0) return false;

    const char* first = strchr(input+3, ':');
    if (!first) return false;

    const char* second = strchr(first + 1, ':');
    if (!second) return false;

    uint8_t keyLen = first - input - 3;
    uint8_t typeLen = second - (first + 1);

    strncpy(key, input+3, keyLen);
    key[keyLen] = '\0';

    strncpy(type, first + 1, typeLen);
    type[typeLen] = '\0';

    strncpy(rawdata, second + 1, sizeof(rawdata) - 1);
    rawdata[sizeof(rawdata) - 1] = '\0';

    return true;
}


void processPacket() {
  if (strcmp(key, "PING") == 0) send_NRF(":::PONG:STRING:OK");

  if (strncmp(key, "Servo_", 6) == 0) {
    static bool front = rawdata[0] == 'F';
    static bool left = rawdata[1] == 'L';
    static uint8_t joint = rawdata[2] == 'S'? 0 : rawdata[2] == 'A' ? 1 : 2;

         if (front && left && joint==0)  set_servo_position(SERVO_FL_SHOULDER,  atoi(rawdata));
    else if (front && left && joint==1)  set_servo_position(SERVO_FL_ANKLE,     atoi(rawdata));
    else if (front && left)              set_servo_position(SERVO_FL_ELBOW,     atoi(rawdata));
    else if (front && joint==0)          set_servo_position(SERVO_FR_SHOULDER,  atoi(rawdata));
    else if (front && joint==1)          set_servo_position(SERVO_FR_ANKLE,     atoi(rawdata));
    else if (front)                      set_servo_position(SERVO_FR_ELBOW,     atoi(rawdata));
    else if (left && joint==0)           set_servo_position(SERVO_BL_SHOULDER,  atoi(rawdata));
    else if (left && joint==1)           set_servo_position(SERVO_BL_ANKLE,     atoi(rawdata));
    else if (left)                       set_servo_position(SERVO_BL_ELBOW,     atoi(rawdata));
    else if (joint==0)                   set_servo_position(SERVO_BR_SHOULDER,  atoi(rawdata));
    else if (joint==1)                   set_servo_position(SERVO_BR_ANKLE,     atoi(rawdata));
    else                                 set_servo_position(SERVO_BR_ELBOW,     atoi(rawdata));
  }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n[DOGBOT] STARTING");

    if (!init_NRF()) Serial.println("[DOGBOT] (NRF24) INIT ERROR");

    init_servo();

    Serial.println("[DOGBOT] READY");
    send_NRF("[DOGBOT] READY");
}

void loop() {
    if (read_NRF(buffer)) {
        if (parsePacket(buffer)) processPacket();
    }
}
#include <Arduino.h>
#include "nrf.hpp"


char buffer[32];


// Lecture du serial
bool read_serial() {
    static uint8_t index = 0;

    char c;
    while (Serial.available()) {
        c = Serial.read();

        if (c == '\n') {
            buffer[index] = '\0';
            index = 0;
            return true;
        } else if (index < sizeof(buffer) - 1) buffer[index++] = c;
    }

    return false;
}



// --- Fonctions Arduino ---

void setup() {
    Serial.begin(115200);
    Serial.println("\n\n[UNO] STARTING");

    if (!init_NRF()) {
        Serial.println("[UNO] (NRF24) INIT ERROR");
        while (1);
    }

    Serial.println("[UNO] READY");
}


void loop() {
    if (read_serial()) {
        /* COMMANDES PAR DEFAUT ET ENVOI DES DONNEES */

        if (strncmp(buffer, ":::", 3) == 0) send_NRF(buffer); // Données robot
        if (strncmp(buffer, "CMD:", 4) == 0) {

            if (strncmp(buffer, "CMD:PING", 8) == 0) { // Ping
                Serial.print("[UNO] (DOG BOT) Average Ping : ");
                uint16_t ping_avg = ping_NRF(20, 50);
                if (ping_avg!=0) {
                    Serial.print(ping_avg);
                    Serial.println(" ms.");
                }
            }

        }
    }

    if (read_NRF(buffer)) Serial.println(buffer);
}
#include "nrf.hpp"



RF24 radio(PIN_CE, PIN_CS);

const byte txAddress[6] = "PC2RB"; // Canal PC vers Robot
const byte rxAddress[6] = "RB2PC"; // Canal Robot vers PC


bool init_NRF() {

    if (!radio.begin()) return false;

    radio.setPALevel(RF24_PA_MIN);
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(108);
    radio.setPayloadSize(32);
    
    radio.openWritingPipe(txAddress);
    radio.openReadingPipe(1, rxAddress);
    
    radio.startListening();

    return true;
}

bool send_NRF(const char* text) {
    radio.stopListening();
    bool ok = radio.write(text, 32);
    radio.startListening();

    return ok;
}

bool read_NRF(char* buffer) {
    if (!radio.available()) return false;

    radio.read(buffer, 32);

    return true;
}


int ping_NRF(uint8_t attempts, uint16_t delayMs) {

    float total = 0;
    uint8_t success = 0;
    char buffer[32];

    uint8_t timout_count = 0;
    for (uint8_t i = 0; i < attempts; i++) {
        unsigned long start = millis();
        send_NRF(":::PING:STRING:HELLO");

        bool received = false;

        unsigned long timeout = millis();
        while (millis() - timeout < 1000) {
            if (read_NRF(buffer)) {
                if (strncmp(buffer, ":::PONG", 7) == 0) {
                    unsigned long elapsed = millis() - start;

                    total += elapsed;
                    success++;

                    received = true;
                    break;
                }
            }
        }

        if (!received) {
            timout_count++;
            if (timout_count==5) {
                Serial.println("TIMEOUT");
                return 0;
            }
        }
        delay(delayMs);
    }

    if (success == 0) return -1;

    return total / success;
}

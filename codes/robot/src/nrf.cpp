#include "nrf.hpp"



RF24 radio(PIN_CE, PIN_CS);

const byte txAddress[6] = "RB2PC"; // Canal Robot vers PC
const byte rxAddress[6] = "PC2RB"; // Canal PC vers Robot


bool init_NRF() {
    SPI.begin(18, 19, 25, 5);

    delay(100);

    radio = RF24(PIN_CE, PIN_CS);

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

    radio.read(&buffer, sizeof(buffer));

    if (strncmp(buffer, ":::PING:", 9) == 0) send_NRF(":::PONG:STRING:OK");

    return true;
}


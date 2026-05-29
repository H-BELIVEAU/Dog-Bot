#include <SPI.h>
#include <RF24.h>


// Pins du module NRF24L01
#define PIN_CE  9
#define PIN_CS  10


extern RF24 radio;

extern const byte txAddress[6];
extern const byte rxAddress[6];


// Fonctions externes
bool init_NRF();
bool send_NRF(const char* text);
bool read_NRF(char* buffer);
int ping_NRF(uint8_t attempts = 10, uint16_t delayMs = 100);

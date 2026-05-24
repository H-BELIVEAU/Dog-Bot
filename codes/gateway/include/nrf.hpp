#include <SPI.h>
#include <RF24.h>


#define PIN_CE  9
#define PIN_CS  10


extern RF24 radio;

extern const byte txAddress[6];
extern const byte rxAddress[6];


bool init_NRF();
bool send_NRF(const char* text);
bool read_NRF(char* buffer);
int ping_NRF(uint8_t attempts = 10, uint16_t delayMs = 100);

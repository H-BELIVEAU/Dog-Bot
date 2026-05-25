#include <SPI.h>
#include <RF24.h>


#define PIN_CE  4
#define PIN_CS  5


extern RF24 radio;

extern const byte txAddress[6];
extern const byte rxAddress[6];


bool init_NRF();
bool send_NRF(const char* text);
bool read_NRF(char* buffer);

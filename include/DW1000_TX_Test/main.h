#include <Arduino.h>
#include <SPI.h>
#include <DW1000.h>

void handleSent();
void transmitter();

// ESP32_UWB pin definitions
const uint8_t PIN_SPI_SCK = 18;
const uint8_t PIN_SPI_MISO = 19;
const uint8_t PIN_SPI_MOSI = 23;
const uint8_t PIN_SPI_CS = 4; // CS in schematic shows 21 but its 4

const uint8_t PIN_RST = 27;
const uint8_t PIN_IRQ = 34;

// DEBUG packet sent status and count
boolean sent = false;
volatile boolean sentAck = false;
volatile unsigned long delaySent = 0;
int16_t sentNum = 0; // todo check int type
DW1000Time sentTime;

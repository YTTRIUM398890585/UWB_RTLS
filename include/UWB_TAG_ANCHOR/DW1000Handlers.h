#pragma once

// SPI interface required for DW1000 communication
#include <SPI.h>

// Include the C string library to convert macro defined string to char array
#include <cstring>

// DW-1000 specific library. See https://github.com/playfultechnology/arduino-dw1000
#include <DW1000Ranging.h>
#include "UWB_TAG_ANCHOR/link.h"

extern AnchorLinkedList uwb_data;

// INIT
void initDW1000(uint8_t spi_sck, uint8_t spi_miso, uint8_t spi_mosi, uint8_t dw1000_rst, uint8_t dw1000_cs, uint8_t dw1000_irq, const uint16_t antenna_delay);

// CALLBACK HANDLERS
void newRange();
void newDevice(DW1000Device* device);
void inactiveDevice(DW1000Device* device);
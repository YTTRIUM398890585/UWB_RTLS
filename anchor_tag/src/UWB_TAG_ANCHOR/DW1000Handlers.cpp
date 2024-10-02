#include "UWB_TAG_ANCHOR/DW1000Handlers.h"

AnchorLinkedList uwb_data;

void initDW1000(uint8_t spi_sck,
                uint8_t spi_miso,
                uint8_t spi_mosi,
                uint8_t dw1000_rst,
                uint8_t dw1000_cs,
                uint8_t dw1000_irq,
                const uint16_t antenna_delay)
{
	// Initialise SPI interface on specified SCK, MISO, MOSI pins
	SPI.begin(spi_sck, spi_miso, spi_mosi);

	// Start up DW1000 chip on specified RESET, CS, and IRQ pins
	DW1000Ranging.initCommunication(dw1000_rst, dw1000_cs, dw1000_irq);

	// Set antenna delay for DW1000
	// for tags is always the same, 0
	// for anchors it depends on calibration, agasint a tag with known distance and delay = 0
	// ANTENNA_DELAY is defined uniquely for each anchor in platformio.ini
	DW1000.setAntennaDelay(antenna_delay);

	// Assign callback handlers...
	// ...when distance to a known tag changes
	DW1000Ranging.attachNewRange(newRange);
	// ...when new device found on network
	DW1000Ranging.attachNewDevice(newDevice);
	// ...when previously known device has been declared inactive and removed from network
	DW1000Ranging.attachInactiveDevice(inactiveDevice);

	// convert macro defined string to char array
	// each device has a unique address defined in platformio.ini
	// 24 is 23 characters + null terminator
	char DEVICE_ADD_CHAR[24];
	strcpy(DEVICE_ADD_CHAR, DEVICE_ADDRESS);

#ifdef IS_ANCHOR
	// Start the DW-1000 as an anchor specifying pre-configured mode of operation
	// to prioritise accuracy/range/low power usage etc. Modes available are:
	// - MODE_LONGDATA_RANGE_LOWPOWER (110kb/s data rate, 16 MHz PRF and long preambles)
	// - MODE_SHORTDATA_FAST_LOWPOWER (6.8Mb/s data rate, 16 MHz PRF and short preambles)
	// - MODE_LONGDATA_FAST_LOWPOWER (6.8Mb/s data rate, 16 MHz PRF and long preambles)
	// - MODE_SHORTDATA_FAST_ACCURACY (6.8Mb/s data rate, 64 MHz PRF and short preambles)
	// - MODE_LONGDATA_FAST_ACCURACY (6.8Mb/s data rate, 64 MHz PRF and long preambles)
	// - MODE_LONGDATA_RANGE_ACCURACY (110kb/s data rate, 64 MHz PRF and long preambles)

	float anchor_coords[] = { ANCHOR_COORDS_X, ANCHOR_COORDS_Y, ANCHOR_COORDS_Z };
	DW1000Ranging.startAsAnchor(DEVICE_ADD_CHAR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false, anchor_coords);

	Serial.print(F("Anchor Coords: ("));
	Serial.print(ANCHOR_COORDS_X);
	Serial.print(", ");
	Serial.print(ANCHOR_COORDS_Y);
	Serial.print(", ");
	Serial.print(ANCHOR_COORDS_Z);
	Serial.println(")");

#elif defined(IS_TAG)
	// Start the DW-1000 as a tag (using the same mode as the anchors)
	DW1000Ranging.startAsTag(DEVICE_ADD_CHAR, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
#endif

	// For debugging, let's print the address of this device
	Serial.println(DEVICE_ADDRESS);

	// Let's calculate a "short address" from the last 2 bytes of the device address
	// We'll use a "short address" to make it easier to reference devices
	char shortAddress[6];
	byte* currentShortAddress = DW1000Ranging.getCurrentShortAddress();
	sprintf(shortAddress, "%02X%02X", currentShortAddress[1], currentShortAddress[0]);
	Serial.print(F("Short Address: "));
	Serial.println(shortAddress);
	Serial.print(F("Antenna Delay: "));
	Serial.println(DW1000.getAntennaDelay());
}

void newRange()
{
	// #ifdef DEBUG
	// 	Serial.print(F("Device range updated: "));
	// 	Serial.println(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);

	// 	Serial.print("Dev ");
	// 	Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
	// 	Serial.print("\t");
	// Serial.print(DW1000Ranging.getDistantDevice()->getRange());
	// 	Serial.print(" m\t");
	// 	Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
	// 	Serial.println(" dBm");
	// #endif

	// Serial.println(DW1000Ranging.getDistantDevice()->getRange());

// Update links
#ifdef IS_TAG
	uwb_data.update_anchor(DW1000Ranging.getDistantDevice()->getShortAddress(),
	                       DW1000Ranging.getDistantDevice()->getRange(),
	                       DW1000Ranging.getDistantDevice()->getRXPower());
#endif
}

void newDevice(DW1000Device* device)
{
	// #ifdef DEBUG
	// 	Serial.print(F("New device detected! "));
	// 	Serial.println(device->getShortAddress(), HEX);
	// #endif

	float anchor_coords[3];

	device->getCoords(anchor_coords);

#ifdef IS_TAG
	uwb_data.add_anchor(device->getShortAddress(), anchor_coords);
#endif
}

void inactiveDevice(DW1000Device* device)
{
	// #ifdef DEBUG
	// 	Serial.print(F("Device removed: "));
	// 	Serial.println(device->getShortAddress(), HEX);
	// #endif

#ifdef IS_TAG
	uwb_data.delete_anchor(device->getShortAddress());
#endif
}
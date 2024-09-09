/**
 * Ultra WideBand Real-Time Positioning System (UWBRTLS)
 *
 * Note: There are a lot of useful application notes provided by Decawave available at
 *  https://www.decawave.com/application-notes/
 * e.g. APS017 Maximising Range:
 * https://www.decawave.com/wp-content/uploads/2018/10/APS017_Max-Range-in-DW1000-Systems_v1.1.pdf
 */

// INCLUDES
#include <Arduino.h>

// SPI interface required for DW1000 communication
#include <SPI.h>
// DW-1000 specific library. See https://github.com/playfultechnology/arduino-dw1000
#include <DW1000Ranging.h>

// Include the C string library to convert macro defined string to char array
#include <cstring>

#include <ArduinoEigen.h>

#ifdef IS_TAG
#include "UWB_TAG_ANCHOR/link.h"
#endif

#include "UWB_TAG_ANCHOR/define.h"
#include "UWB_TAG_ANCHOR/main.h"

// All the functions in BasicLinearAlgebra are wrapped up inside the namespace BLA
using namespace Eigen;

void setup()
{
	// Initialise serial connection for debugging
	Serial.begin(115200);
	Serial.println(__FILE__);
	Serial.println(__DATE__);

	// Initialise SPI interface on specified SCK, MISO, MOSI pins
	SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI);

	// Start up DW1000 chip on specified RESET, CS, and IRQ pins
	DW1000Ranging.initCommunication(PIN_RST, PIN_SPI_CS, PIN_IRQ);

	// Set antenna delay for DW1000
	// for tags is always the same, 0
	// for anchors it depends on calibration, agasint a tag with known distance and delay = 0
	// ANTENNA_DELAY is defined uniquely for each anchor in platformio.ini
	DW1000.setAntennaDelay(ANTENNA_DELAY);

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
	DW1000Ranging.startAsAnchor(DEVICE_ADD_CHAR, DW1000.MODE_LONGDATA_RANGE_ACCURACY, false);

#elif defined(IS_TAG)
	// Start the DW-1000 as a tag (using the same mode as the anchors)
	DW1000Ranging.startAsTag(DEVICE_ADD_CHAR, DW1000.MODE_LONGDATA_RANGE_ACCURACY, false);
#endif

	// For debugging, let's print the address of this device
	Serial.println(DEVICE_ADDRESS);

	// Let's calculate a "short address" from the last 2 bytes of the device address
	byte* currentShortAddress = DW1000Ranging.getCurrentShortAddress();
	sprintf(shortAddress, "%02X%02X", currentShortAddress[1], currentShortAddress[0]);
	Serial.print(F("Short Address: "));
	Serial.println(shortAddress);
	Serial.print(F("Antenna Delay: "));
	Serial.println(DW1000.getAntennaDelay());

	Serial.println("Setup complete");

	// Short pause before starting main loop
	delay(500);
}

void loop()
{
	// This needs to be called on every iteration of the main program loop
	DW1000Ranging.loop();

#ifdef IS_TAG
	// // Multilateration /////////////////////////////////////

	// // TODO: hardcode anchor coords for now
	// // anchor 0
	// struct MyLink* temp = find_link(uwb_data, 0 * 256 + 1);
	// if (temp != NULL) {
	// 	temp->anchor_coords[0] = 0.0; // x
	// 	temp->anchor_coords[1] = 0.0; // y
	// 	temp->anchor_coords[2] = 0.0; // z
	// }

	// // anchor 1
	// temp = find_link(uwb_data, 0 * 256 + 2);
	// if (temp != NULL) {
	// 	temp->anchor_coords[0] = 2.0; // x
	// 	temp->anchor_coords[1] = 0.0; // y
	// 	temp->anchor_coords[2] = 0.0; // z
	// }

	// // anchor 2
	// temp = find_link(uwb_data, 0 * 256 + 3);
	// if (temp != NULL) {
	// 	temp->anchor_coords[0] = 0.0; // x
	// 	temp->anchor_coords[1] = 2.0; // y
	// 	temp->anchor_coords[2] = 0.0; // z
	// }

	// // r = (ATA)^-1 AT d
	// // A = [2(x0-xi) 2(y0-yi) 2(z0-zi)] for anchor row i = 0, 1, 2
	// // d = [di^2 -d0^2 - xi^2 + x0^2 - yi^2 + y0^2 - zi^2 + z0^2 ] for anchor row i = 0, 1, 2
	// // r = [x y z]

	// Matrix<float, Dynamic, 3> A;
	// Matrix<float, Dynamic, 1> d;
	// Matrix<float, 3, 1> r;

	// temp = uwb_data;
	// while (temp->next != NULL) {

	// }
	// return;

	// // clang-format off

	// // clang-format on

	#ifdef DEBUG
		// Print the list of known anchors
        static unsigned long last_print = 0;
        if (millis() - last_print > 100) {
            Serial.print("millis: ");
            Serial.println(millis());
            uwb_data.print_list();
            last_print = millis();
        }
	#endif

#endif
}

#ifdef IS_TAG
// TODO: Implement the function to send publish current tag coordinates to ros
#endif

// CALLBACK HANDLERS FOR DW1000Ranging
void newRange()
{
// #ifdef DEBUG
// 	Serial.print(F("Device range updated: "));
// 	Serial.println(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);

// 	Serial.print("Dev ");
// 	Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
// 	Serial.print("\t");
// 	Serial.print(DW1000Ranging.getDistantDevice()->getRange());
// 	Serial.print(" m\t");
// 	Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
// 	Serial.println(" dBm");
// #endif

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

    // TODO: somehow get the anchor to publish its own coords
    float todo[] = {0, 0, 0};

#ifdef IS_TAG
	uwb_data.add_anchor(device->getShortAddress(), todo);
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

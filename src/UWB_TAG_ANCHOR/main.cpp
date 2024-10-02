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

#include <ArduinoEigen.h>
using namespace Eigen;

#include "UWB_TAG_ANCHOR/define.h"
#include "UWB_TAG_ANCHOR/main.h"

#ifdef IS_TAG
#include "UWB_TAG_ANCHOR/utils.h"
#include "UWB_TAG_ANCHOR/multilateration.h"
#include "UWB_TAG_ANCHOR/microROS.h"
#endif

#include "UWB_TAG_ANCHOR/DW1000Handlers.h"

// TODO: Implement the function to send publish current tag coordinates to ros
// TODO: Implement publish anchor coords

void setup()
{
	// Initialise serial connection for debugging
	Serial.begin(115200);
	Serial.println(__FILE__);
	Serial.println(__DATE__);

	// Initialise UWB device
	initDW1000(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_RST, PIN_SPI_CS, PIN_IRQ, ANTENNA_DELAY);

#ifdef IS_TAG
	// Initialise microROS
	setupMicroRos();
#endif

	Serial.println("Setup complete");

	// Short pause before starting main loop
	delay(500);
}

void loop()
{
	// This needs to be called on every iteration of the main program loop
	DW1000Ranging.loop();

#ifdef IS_TAG
	Vector3f tagCoords;

	tagCoords = multilateration(uwb_data, false);

	if (millis() - last_pub > PUB_PERIOD_MS) {
		rosPublishLocation(uwb_data, tagCoords);
		last_pub = millis();
	}

#ifdef DEBUG
	// Print the list of known anchors and current tag coordinates
	if (millis() - last_print > PRINT_PERIOD_MS) {
		Serial.print("millis: ");
		Serial.println(millis());
		uwb_data.print_list();
		printVector(tagCoords, "Tag Coordinates");
		last_print = millis();
	}
#endif
#endif
}

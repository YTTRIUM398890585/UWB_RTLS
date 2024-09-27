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

	initDW1000(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_RST, PIN_SPI_CS, PIN_IRQ, ANTENNA_DELAY);

	Serial.println("Setup complete");

	// Short pause before starting main loop
	delay(500);

	//     // TODO: hardcode anchor coords for now
	// 	float todo1[] = { 0.0, 0.0, 0.0 };
	// 	uwb_data.add_anchor(0 * 256 + 1, todo1);

	// 	float todo2[] = { 7.0, 0.0, 2.0 };
	// 	uwb_data.add_anchor(0 * 256 + 2, todo2);

	// 	float todo3[] = { 0.0, 7.0, 2.0 };
	// 	uwb_data.add_anchor(0 * 256 + 3, todo3);

	// 	float todo4[] = { 7.0, 7.0, 0.0 };
	// 	uwb_data.add_anchor(0 * 256 + 4, todo4);

	// 	float todo5[] = { 3.5, 2.0, 1.0 };
	// 	uwb_data.add_anchor(0 * 256 + 5, todo5);
}

void loop()
{
	// This needs to be called on every iteration of the main program loop
	DW1000Ranging.loop();

	// // TODO: hardcode anchor coords for now
	// // anchor 1
	// uwb_data.update_anchor(0 * 256 + 1, 8.17, -10);

	// // anchor 2
	// uwb_data.update_anchor(0 * 256 + 2, 8.48, -20);

	// // anchor 3
	// uwb_data.update_anchor(0 * 256 + 3, 3.91, -30);

	// // anchor 4
	// uwb_data.update_anchor(0 * 256 + 4, 3.63, -40);

	// // anchor 4
	// uwb_data.update_anchor(0 * 256 + 5, 5.51, -50);

	// delay(1000);

#ifdef IS_TAG
	Vector3f r;

	// #ifdef DEBUG
	// 	r = multilateration(uwb_data, true);
	// #else
	// 	r = multilateration(uwb_data, false);
	// #endif

	r = multilateration(uwb_data, false);

#ifdef DEBUG
	// Print the list of known anchors and current tag coordinates
	if (millis() - last_print > 1000) {
		Serial.print("millis: ");
		Serial.println(millis());
		uwb_data.print_list();
		printVector(r, "Tag Coordinates");
		last_print = millis();
	}
#endif

#endif
}

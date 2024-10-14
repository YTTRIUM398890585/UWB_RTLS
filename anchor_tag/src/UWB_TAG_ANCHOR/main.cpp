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

void setup()
{
	// Initialise serial connection for debugging
	Serial.begin(115200);
	Serial.println(__FILE__);
	Serial.println(__DATE__);

	// Initialise UWB device
	initDW1000(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_RST, PIN_SPI_CS, PIN_IRQ, ANTENNA_DELAY);

	// create a task that will be executed in the rangingTaskCode() function,
	// with priority 1 and executed on core 0
	xTaskCreatePinnedToCore(rangingTaskCode, /* Task function. */
	                        "rangingTask",   /* name of task. */
	                        10000,           /* Stack size of task */
	                        NULL,            /* parameter of the task */
	                        1,               /* priority of the task */
	                        &rangingTask,    /* Task handle to keep track of created task */
	                        0);              /* pin task to core 0 */
	delay(500);

#ifdef IS_TAG
	// Initialise microROS
	// setupMicroRos();

	// create a task that will be executed in the multilaterationTask() function,
	// with priority 1 and executed on core 1
	xTaskCreatePinnedToCore(multilaterationTaskCode, /* Task function. */
	                        "multilaterationTask",   /* name of task. */
	                        10000,                   /* Stack size of task */
	                        NULL,                    /* parameter of the task */
	                        1,                       /* priority of the task */
	                        &multilaterationTask,    /* Task handle to keep track of created task */
	                        1);                      /* pin task to core 1 */
	delay(500);
#endif

	/* Send a notification to rangingTask(), bringing it out of the Blocked state. */
	// to kick start the rangingTask
	xTaskNotifyGive(rangingTask);

	Serial.println("Setup complete");

	// Short pause before starting main loop
	delay(500);
}

void loop() { }

void rangingTaskCode(void* pvParameters)
{
	Serial.print("rangingTask running on core ");
	Serial.println(xPortGetCoreID());

	for (;;) {
		// This needs to be called on every iteration of the main program loop
		DW1000Ranging.loop();

#ifdef IS_TAG
		// check for sufficient data to perform filtering and multilateration
		if (true) {
			// check that multilateration is not running
			// so can copy the uwb_data to mul_data
			// this should be just a poll with no waiting
			// if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY)) {
			// 	// // copy uwb_data to mul_data
            //     // mul_data.copyFrom(uwb_data);

            //     // // clear distance fields in uwb_data, only clear range not the nodes
            //     // uwb_data.clearDistance();

			// 	// Send a notification to multilaterationTask(), bringing it out of the Blocked state.
			// 	xTaskNotifyGive(multilaterationTask);
			// }

            ulTaskNotifyTake(pdTRUE, 1000);

	        Serial.print("rangingTask");

            xTaskNotifyGive(multilaterationTask);
		}
#endif
	}
}

#ifdef IS_TAG
void multilaterationTaskCode(void* pvParameters)
{
	Serial.print("multilaterationTask running on core ");
	Serial.println(xPortGetCoreID());

	for (;;) {
		// Block to wait for multilaterationTask() to notify this task to start running
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		// Perform filtering (not implemented) + multilateration on mul_data
		// Vector3f tagCoords;

		// tagCoords = multilateration(mul_data, false);

		// Publish the tag coordinates to microROS
		// if (millis() - last_pub > PUB_PERIOD_MS) {
		//     rosPublishLocation(mul_data, tagCoords);
		//     last_pub = millis();
		// }

#ifdef DEBUG
		// Print the list of known anchors and current tag coordinates
		if (millis() - last_print > PRINT_PERIOD_MS) {
			Serial.print("millis: ");
			Serial.println(millis());
			// uwb_data.print_list();
			// mul_data.print_list();
			// printVector(tagCoords, "Tag Coordinates");
			last_print = millis();
		}
#endif

		// Send a notification to rangingTask(),
		// telling it that multilateration is complete and mul_data is ready to be updated
		xTaskNotifyGive(rangingTask);

        // // Add a delay to prevent watchdog timeout
        // vTaskDelay(pdMS_TO_TICKS(100));
	}
}
#endif
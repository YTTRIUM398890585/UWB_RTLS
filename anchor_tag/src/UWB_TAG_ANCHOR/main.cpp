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

#ifdef IS_TAG
#include "UWB_TAG_ANCHOR/link.h"
#include "UWB_TAG_ANCHOR/utils.h"
#include "UWB_TAG_ANCHOR/multilateration.h"
#include "UWB_TAG_ANCHOR/microROS.h"
#endif

#include "UWB_TAG_ANCHOR/DW1000Handlers.h"

#include "UWB_TAG_ANCHOR/define.h"
#include "UWB_TAG_ANCHOR/main.h"

void setup()
{
	// Initialise serial connection for debugging
	Serial.begin(115200);
	Serial.println(__FILE__);
	Serial.println(__DATE__);

	// Create the mutex
	xMutex = xSemaphoreCreateMutex();

	// Initialise UWB device
	initDW1000(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_RST, PIN_SPI_CS, PIN_IRQ, ANTENNA_DELAY);

	// create a task that will be executed in the rangingTaskCode() function,
	// with priority 1 and executed on core 1
	xTaskCreatePinnedToCore(rangingTaskCode, /* Task function. */
	                        "rangingTask",   /* name of task. */
	                        10000,           /* Stack size of task */
	                        NULL,            /* parameter of the task */
	                        1,               /* priority of the task */
	                        &rangingTask,    /* Task handle to keep track of created task */
	                        1);              /* pin task to core 1 */
	delay(500);

#ifdef IS_TAG
	// Initialise microROS
	setupMicroRos();

	// create a task that will be executed in the multilaterationTask() function,
	// with priority 1 and executed on core 0
	xTaskCreatePinnedToCore(multilaterationTaskCode, /* Task function. */
	                        "multilaterationTask",   /* name of task. */
	                        10000,                   /* Stack size of task */
	                        NULL,                    /* parameter of the task */
	                        1,                       /* priority of the task */
	                        &multilaterationTask,    /* Task handle to keep track of created task */
	                        0);                      /* pin task to core 0 */
	delay(500);
#endif

	Serial.println("Setup complete");
}

void loop() { }

void rangingTaskCode(void* pvParameters)
{
	Serial.print("rangingTask running on core ");
	Serial.println(xPortGetCoreID());

	for (;;) {
		DW1000Ranging.loop();

#ifdef IS_TAG
		// TODO: by right freeRTOS notification is more efficient than using a flag, but i skill issue
		// Check:
		// 1) if there is sufficient data to perform multilateration
		// 2) // TODO: if ranging loop is not waiting for a range response (this should be done in
		// DW1000Ranging.loop())
		if (uwb_data.all_buffer_full() && new_data == false) {
			// Poll for mutex to access mul_data
			if (xSemaphoreTake(xMutex, 0) == pdTRUE) {
				Serial.println("rangingTask accessing shared resource");

				// Copy the uwb_data to mul_data (this clears mul_data before copying)
				mul_data.copyFrom(uwb_data);

				// Set flag for multilateration task
				new_data = true;

				// Release the mutex after accessing the shared resource
				xSemaphoreGive(xMutex);

				// Clear distance fields in uwb_data, only clear range not the nodes
				uwb_data.clearDistance();
			}
		}
#endif

		// Add a short delay to prevent tight loop
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

#ifdef IS_TAG
void multilaterationTaskCode(void* pvParameters)
{
	Serial.print("multilaterationTask running on core ");
	Serial.println(xPortGetCoreID());

	for (;;) {
		// Check if there is new data to perform multilateration (busy waiting)
		if (new_data) {
			// Wait for mutex before accessing access mul_data
			if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
				Serial.println("multilaterationTask accessing shared resource");

				// Perform filtering + multilateration on mul_data
				Vector3f tagCoords = multilateration(mul_data, false);

				// Publish the position
				rosPublishLocation(mul_data, tagCoords);

				// Reset the flag
				new_data = false;

#if defined(IS_TAG) && defined(DEBUG)
				static unsigned long last_print = 0;
				const unsigned long PRINT_PERIOD_MS = 1000;

				// Print the list of known anchors and current tag coordinates
				if (millis() - last_print > PRINT_PERIOD_MS) {
					Serial.print("millis: ");
					Serial.println(millis());
					mul_data.print_list();
					printVector(tagCoords, "Tag Coordinates");
					last_print = millis();
				}
#endif
				// Release the mutex after accessing the shared resource
				xSemaphoreGive(xMutex);
			}
		}
		// Add a short delay to prevent tight loop
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}
#endif
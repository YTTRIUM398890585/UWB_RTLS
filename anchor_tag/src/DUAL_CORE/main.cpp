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

#include "DUAL_CORE/main.h"

void setup()
{
	// Initialise serial connection for debugging
	Serial.begin(115200);
	Serial.println(__FILE__);
	Serial.println(__DATE__);

    // Create the mutex
    xMutex = xSemaphoreCreateMutex();

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

	// create a task that will be executed in the multilaterationTask() function,
	// with priority 1 and executed on core 1
	xTaskCreatePinnedToCore(multilaterationTaskCode, /* Task function. */
	                        "multilaterationTask",   /* name of task. */
	                        10000,                  /* Stack size of task */
	                        NULL,                   /* parameter of the task */
	                        1,                      /* priority of the task */
	                        &multilaterationTask,    /* Task handle to keep track of created task */
	                        1);                     /* pin task to core 1 */
	delay(500);

	Serial.println("Setup complete");

	// Short pause before starting main loop
	delay(500);
}

void loop() { }

void rangingTaskCode(void* pvParameters) { 
    Serial.print("rangingTask running on core ");
	Serial.println(xPortGetCoreID());

	for (;;) {
        // Take the mutex before accessing the shared resource
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // Access the shared resource
            Serial.println("rangingTask accessing shared resource");

            // Release the mutex after accessing the shared resource
            xSemaphoreGive(xMutex);
        }

        // Add a delay to prevent watchdog timeout
        vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void multilaterationTaskCode(void* pvParameters)
{
    Serial.print("multilaterationTask running on core ");
	Serial.println(xPortGetCoreID());

    for (;;) {
        // Take the mutex before accessing the shared resource
        if (xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
            // Access the shared resource
            Serial.println("multilaterationTask accessing shared resource");

            // vTaskDelay(pdMS_TO_TICKS(10000));


            // Release the mutex after accessing the shared resource
            xSemaphoreGive(xMutex);
        }

        // Add a delay to prevent watchdog timeout
        vTaskDelay(pdMS_TO_TICKS(100));
	}
}
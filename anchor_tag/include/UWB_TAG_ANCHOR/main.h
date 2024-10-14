#pragma once
#include <Arduino.h>

TaskHandle_t rangingTask;
void rangingTaskCode(void* pvParameters);

#ifdef IS_TAG
TaskHandle_t multilaterationTask;
void multilaterationTaskCode(void* pvParameters);

unsigned long last_pub = 0;
const unsigned long PUB_PERIOD_MS = 1000;
#endif
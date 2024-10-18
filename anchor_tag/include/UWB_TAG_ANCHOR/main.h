#pragma once
#include <Arduino.h>

SemaphoreHandle_t xMutex;

TaskHandle_t rangingTask;
void rangingTaskCode(void* pvParameters);

#ifdef IS_TAG
AnchorLinkedList mul_data;
TaskHandle_t multilaterationTask;
void multilaterationTaskCode(void* pvParameters);

bool new_data = false;

unsigned long last_pub = 0;
const unsigned long PUB_PERIOD_MS = 1000;
#endif
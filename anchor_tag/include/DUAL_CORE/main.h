#pragma once
#include <Arduino.h>

TaskHandle_t rangingTask;
TaskHandle_t multilaterationTask;

SemaphoreHandle_t xMutex;

void rangingTaskCode(void* pvParameters);
void multilaterationTaskCode(void* pvParameters);

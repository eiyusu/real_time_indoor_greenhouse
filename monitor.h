#ifndef MONITOR_H
#define MONITOR_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>


#define LDR 0
#define IRRIGATION 1


SemaphoreHandle_t mutex[2] = {xSemaphoreCreateMutex(), xSemaphoreCreateMutex()};

static uint8_t sensorReading[2] = {0, 0};


void set_value(uint8_t value, uint8_t sensor){
  xSemaphoreTake( mutex[sensor], portMAX_DELAY );
  sensorReading[sensor] = value;
  xSemaphoreGive(mutex[sensor]);
}

uint8_t get_value( uint8_t sensor){
  uint8_t aux;
  xSemaphoreTake( mutex[sensor], portMAX_DELAY );
  aux = sensorReading[sensor];
  xSemaphoreGive(mutex[sensor]);
  return aux;
}

#endif

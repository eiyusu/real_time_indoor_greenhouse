#ifndef MONITOR_H
#define MONITOR_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>


#define LDR 0
#define IRRIGATION 1

#define SIZE 2

SemaphoreHandle_t mutex[SIZE] = {xSemaphoreCreateMutex(), xSemaphoreCreateMutex()};
static uint8_t secureValues[SIZE] = {0, 0};


void set_value(uint8_t value, uint8_t variable){
  xSemaphoreTake( mutex[variable], portMAX_DELAY );
  secureValues[variable] = value;
  xSemaphoreGive(mutex[variable]);
}


uint8_t get_value( uint8_t variable){
  uint8_t aux;
  xSemaphoreTake( mutex[variable], portMAX_DELAY );
  aux = secureValues[variable];
  xSemaphoreGive(mutex[variable]);
  return aux;
}

#endif

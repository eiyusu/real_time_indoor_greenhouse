#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include "monitor.h"
#include <timers.h>
#include <queue.h>
#include "light.h"


BaseType_t qparameter = pdTRUE;

void handleInterrupt(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, &qparameter );
}

void setup() {

  

  // TimerHandle_t xTimerCreate( 
  //   const char * const pcTimerName, 
  //   TickType_t xTimerPeriodInTicks, 
  //   UBaseType_t uxAutoReload, 
  //   void * pvTimerID, 
  //   TimerCallbackFunction_t pxCallbackFunction );

    

  Serial.begin(115200);
  
  light_setup();

  Serial.println("SETUP");

  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt , RISING );

}

void loop() {}


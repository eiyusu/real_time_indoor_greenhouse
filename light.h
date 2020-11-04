#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <queue.h>

#define LIGHTPIN 5
#define DARK 0
#define LIGHT 1

TimerHandle_t light_timer;
bool enableLight = false;
bool current_light_state = DARK;
QueueHandle_t enable_disable_Q = xQueueCreate(1, sizeof(bool));
uint16_t times[2] = { pdMS_TO_TICKS(500), pdMS_TO_TICKS(2000)};


void light_turn_on(){
    digitalWrite(LIGHTPIN, HIGH);
}

void light_turn_off(){
    digitalWrite(LIGHTPIN, LOW);
}

void light_state_toggle(TimerHandle_t xTimer){
  digitalWrite(LIGHTPIN, !current_light_state);
  current_light_state = !current_light_state;
  Serial.print("changing current_light_state to: ");
  Serial.println(current_light_state ? "LIGHT" : "DARK");
  xTimerChangePeriod(xTimer, times[current_light_state], 0);
}


/*
Task que espera uma interrupção colocar um valor na fila
quando isso ocorre olha esse valor e desabilita ou habilita o timer
*/

void light_enable_disable( void *pv){
  bool L;
  for(;;){
    if ( xQueueReceive( enable_disable_Q, &L, portMAX_DELAY) == pdPASS) {
      
      if(L){
       xTimerStart( light_timer, 0 );
       Serial.println("Iniciou o timer");
      }
      else{
        xTimerStop( light_timer, 0 );
        digitalWrite(LIGHTPIN, LOW);
        Serial.println("Parou o timer");
      }
    }
  }
}


struct changeValueData {
  bool alter_state;
  uint16_t newValue;
};

void light_change_value(void *pv){
  
  struct changeValueData* data;

  for(;;){
    if( xQueueReceive( enable_disable_Q, data, portMAX_DELAY) == pdPASS ){
      times[data->alter_state] = data->newValue;
      if(current_light_state == data->alter_state){
         xTimerChangePeriod( light_timer, pdMS_TO_TICKS(data->newValue), 0 );
      }
    }
  }
}

void light_setup(){
    pinMode(LIGHTPIN, OUTPUT);
    
    light_timer = xTimerCreate(
        "lightTimer", //Nome
        pdMS_TO_TICKS(500), //Periodo
        pdTRUE, // repetir
        NULL, // ID do timer
        light_state_toggle // funcao de callback 
    ); 

    xTaskCreate(
        light_enable_disable, // Task function
        "light_start_stop", // Task name for humans
        128, 
        NULL, // Task parameter
        1, // Task priority
        NULL
    );
}

#endif
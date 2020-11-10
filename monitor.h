#ifndef MONITOR_H
#define MONITOR_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

/*Definição das posições do vetor de mutex*/
#define LDR 0
#define IRRIGATION 1
#define EXA_STATE 2
#define EXA_IRRIGATION 3

//tamanho do vetor de mutex
#define SIZE 4

SemaphoreHandle_t mutex[SIZE];
static uint8_t secureValues[SIZE];

/*
  função para inicializar o monitor e as variaveis compartilhadas.
  Chamar no setup!
*/
void monitor_start(){
  uint8_t i;
  for(i=0; i < SIZE; i++){
    mutex[i] = xSemaphoreCreateMutex();
    secureValues[i] = 0;
  }
}

//Colocar um valor numa variavel compartilhada
void set_value(uint8_t value, uint8_t variable){
  xSemaphoreTake( mutex[variable], portMAX_DELAY );
  secureValues[variable] = value;
  xSemaphoreGive(mutex[variable]);
}

//Ler algum valor de uma variael compartilhada
uint8_t get_value( uint8_t variable){
  uint8_t aux;
  xSemaphoreTake( mutex[variable], portMAX_DELAY );
  aux = secureValues[variable];
  xSemaphoreGive(mutex[variable]);
  return aux;
}

//Pegar um recurso (ex: exaustor) para usá-lo
void lock(uint8_t resource){
  xSemaphoreTake( mutex[resource], portMAX_DELAY );
}

//Devolver recurso
void unlock(uint8_t resource){
  xSemaphoreGive(mutex[resource]);
}

#endif

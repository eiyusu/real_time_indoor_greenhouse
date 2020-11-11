#ifndef EXHAUSTER_H
#define EXHAUSTER_H


#include "monitor.h"

//Variáveis do exaustor
#define led_exaustor 12

extern int t_exaustor;
extern bool exaustor_acionado;

TaskHandle_t        exaustorH;

void exaustor(void *arg){
    unsigned long int init_time, end_time, resp_time; 
    while(1){
        if(exaustor_acionado){
            init_time = millis();
            // digitalWrite no exaustor deve seguir a exclusão mútua com prioridade para a umidade (fazer no monitor)
            if(get_value(EXA_STATE)==1 && get_value(EXA_IRRIGATION)==0){
                set_value(0, EXA_STATE);
                digitalWrite(led_exaustor, LOW);
                Serial.println("Desligar Exaustor"); 
            }
            else if(get_value(EXA_STATE)==0 && get_value(EXA_IRRIGATION)==0){
                set_value(1, EXA_STATE);
                digitalWrite(led_exaustor, HIGH);
                Serial.println("Ligar Exaustor"); 
            }
            end_time = millis();
            resp_time = end_time - init_time;
            imprimir("Resposta Exaustor (ms)", resp_time);
            vTaskDelay(t_exaustor);
        }   
    }
    vTaskDelete(NULL);      //Deleta a Task atual
}

void exa_setup(){
    pinMode(led_exaustor, OUTPUT);

    //Crição da task de leitura de exaustao
    xTaskCreate(exaustor,           //Funcao
                "exaustor",         //Nome
                128,                //Pilha
                NULL,               //Parametro
                1,                  //Prioridade
                &exaustorH); 
}

#endif
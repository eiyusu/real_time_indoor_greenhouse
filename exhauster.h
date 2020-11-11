#ifndef EXHAUSTER_H
#define EXHAUSTER_H


#include "monitor.h"

#define t_exhauster 10000/portTICK_PERIOD_MS

#ifndef led_exaustor
#define led_exaustor 2
#endif

//Variáveis do exaustor
static bool exaustor_acionado = true;

void exaustor(void *arg){
    // unsigned long int init_time, end_time, resp_time; 
    
    for(;;){

    Serial.print("\n\n- TASK ");
    Serial.print(pcTaskGetName(NULL)); 
    Serial.print(", High Watermark: ");
    Serial.print(uxTaskGetStackHighWaterMark(NULL));

        if(exaustor_acionado){
            // init_time = micros();
            
            // digitalWrite no exaustor deve seguir a exclusão mútua com prioridade para a umidade
            if(get_value(EXA_STATE)==1 && get_value(EXA_IRRIGATION)==0){
                set_value(0, EXA_STATE);
                digitalWrite(led_exaustor, LOW);
                // Serial.println("Desligar Exaustor"); 
            }
            else if(get_value(EXA_STATE)==0 && get_value(EXA_IRRIGATION)==0){
                set_value(1, EXA_STATE);
                digitalWrite(led_exaustor, HIGH);
                // Serial.println("Ligar Exaustor"); 
            }
            // end_time = micros();
            // resp_time = end_time - init_time;
            // imprimir("Resposta Exaustor (us)", resp_time);
            vTaskDelay(t_exhauster);
        }   
    }
}

void exa_setup(){
    pinMode(led_exaustor, OUTPUT);

    //Crição da task de leitura de exaustao
    xTaskCreate(exaustor,           //Funcao
                "exaustor",         //Nome
                95,                //Pilha
                NULL,               //Parametro
                1,                  //Prioridade
                NULL); 
}

#endif
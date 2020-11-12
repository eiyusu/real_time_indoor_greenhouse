#ifndef EXHAUSTER_H
#define EXHAUSTER_H


#include "monitor.h"

#define t_exhauster 10000/portTICK_PERIOD_MS

#ifndef led_exaustor
#define led_exaustor 2
#endif

//Variáveis do exaustor
static bool exaustor_acionado = false;

void exaustor(void *arg){
    unsigned long e_time; 
    
    for(;;){

    // Serial.print("\n\n- TASK ");
    // Serial.print(pcTaskGetName(NULL)); 
    // Serial.print(", High Watermark: ");
    // Serial.print(uxTaskGetStackHighWaterMark(NULL));

        if(exaustor_acionado){
            e_time = micros();
            
            // digitalWrite no exaustor deve seguir a exclusão mútua com prioridade para a umidade
            if(get_value(EXA_STATE)==1 && get_value(EXA_IRRIGATION)==0){
                //lock(PIN_EXAUSTOR);
                set_value(0, EXA_STATE);
                digitalWrite(led_exaustor, LOW);
                //unlock(PIN_EXAUSTOR);
                // Serial.println("Desligar Exaustor"); 
                e_time = micros() - e_time;
                imprimir(F("Resposta Exaustor (us)"), e_time);
            }
            else if(get_value(EXA_STATE)==0){
                if(get_value(EXA_IRRIGATION)==0){
                    //lock(PIN_EXAUSTOR);
                    set_value(1, EXA_STATE);
                    digitalWrite(led_exaustor, HIGH);
                    //unlock(PIN_EXAUSTOR);
                    e_time = micros() - e_time;
            imprimir(F("Resposta Exaustor (us)"), e_time);
                }
                else if(get_value(EXA_IRRIGATION)==1){
                    set_value(0, EXA_IRRIGATION);
                }
                // Serial.println("Ligar Exaustor"); 
            }

            vTaskDelay(t_exhauster);
        }
        else{
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
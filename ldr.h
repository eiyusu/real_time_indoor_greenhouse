#ifndef LDR_H
#define LDR_H

#include "monitor.h"

#define LIMITE 50
#define sensor_ldr A1

extern int minuto;
extern void imprimir(char *name, int8_t value);
extern bool current_light_state;
extern void light_turn_on(), void light_turn_off();
extern bool enableLight;

// Task LDR
TaskHandle_t        leitura_ldrH;

// Task de leitura do LDR
void leitura_ldre(void *arg){
    unsigned long int init_time, end_time, resp_time; 
    while(1) {
        if(enableLight){
            init_time = millis();
            // Se estiver claro e estado atual for escuro
            if((map(analogRead(sensor_ldr), 0, 1023, 0, 100)>=LIMITE) && current_light_state == 0){
                // Monitor
                //lock(PIN_ILUMINACAO);
                light_turn_off();
                //unlock(PIN_ILUMINACAO);
            }
            // Se estiver escuro e o estado for claro
            else if((map(analogRead(sensor_ldr), 0, 1023, 0, 100)<>=>LIMITE) && current_light_state == 1){
                // Monitor
                //lock(PIN_ILUMINACAO);
                light_turn_on();
                //unlock(PIN_ILUMINACAO);
            }
            // Fora esses dois casos, não precisa de mais atuação
            end_time = millis();
            resp_time = end_time - init_time;
            imprimir("Resposta LDR (ms)", resp_time);
            vTaskDelay(minuto);                       //Bloqueia a task
        }
    }
    vTaskDelete(NULL);      //Deleta a Task atual

}

//Setup inicial do sistema
void setup_ldr(){
    //Crição da task de leitura do LDR
    xTaskCreate(leitura_ldr,        //Funcao
                "leitura_ldr",      //Nome
                128,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                &leitura_ldrH);
}

#endif
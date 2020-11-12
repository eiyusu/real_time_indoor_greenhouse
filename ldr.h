#ifndef LDR_H
#define LDR_H

#include "monitor.h"
#include "light.h"

#define t_ldr 2000
#define LIMITE 50
#define sensor_ldr A1

static uint8_t a_read_ldr(){
    return map(analogRead(sensor_ldr), 0, 1023, 0, 100);
} 

// Task de leitura do LDR
void leitura_ldr(void *arg){
    
    unsigned long init_time; 
    
    for(;;) {

        // Serial.print("\n\n- TASK ");
        // Serial.print(pcTaskGetName(NULL));
        // Serial.print(", High Watermark: ");
        // Serial.print(uxTaskGetStackHighWaterMark(NULL));

        if(enableLight){
            init_time = micros();
            // Se estiver claro e estado atual for escuro
            if(a_read_ldr() >= LIMITE && current_light_state == 0){
                char line_text[16];
                snprintf_P(line_text, sizeof(line_text), PSTR("LDR: change"));
                Serial.println(line_text); 
                // Monitor
                lock(PIN_ILUMINACAO);
                light_turn_off();
                unlock(PIN_ILUMINACAO);
            }
            // Se estiver escuro e o estado for claro
            else if(a_read_ldr() < LIMITE && current_light_state == 1){
                char line_text[16];
                snprintf_P(line_text, sizeof(line_text), PSTR("LDR: change"));
                Serial.println(line_text); 
                // Monitor
                lock(PIN_ILUMINACAO);
                light_turn_on();
                unlock(PIN_ILUMINACAO);
            }
            else{
                char line_text[16];
                snprintf_P(line_text, sizeof(line_text), PSTR("LDR: OK"));
                Serial.println(line_text); 
            }
            // Fora esses dois casos, não precisa de mais atuação
            init_time = micros() - init_time;
            imprimir(F("Resposta LDR (us)"), init_time);
            vTaskDelay(t_ldr);                       //Bloqueia a task
        }
        else{
            vTaskDelay(t_ldr);                       //Bloqueia a task
        }
    }
}

//Setup inicial do sistema
void ldr_setup(){
    //Crição da task de leitura do LDR
    xTaskCreate(leitura_ldr,        //Funcao
                NULL,      //Nome
                90,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                NULL);
}

#endif
#ifndef LDR_H
#define LDR_H

#include "monitor.h"
#include "light.h"

#define t_ldr 2000/portTICK_PERIOD_MS
#define LIMITE 50
#define sensor_ldr A1

static uint8_t a_read_ldr(){
    uint8_t luz;
    lock(LDR);
    luz = map(analogRead(sensor_ldr), 0, 1023, 0, 100);
    unlock(LDR);
    return luz;
} 

// Task de leitura do LDR
void leitura_ldr(void *arg){
    
    // unsigned long init_time; 
    
    for(;;) {

        // Serial.print("\n\n- TASK ");
        // Serial.print(pcTaskGetName(NULL));
        // Serial.print(", High Watermark: ");
        // Serial.print(uxTaskGetStackHighWaterMark(NULL));

        if(enableLight){
            // init_time = micros();
            // Se estiver claro e estado atual for escuro
            if(a_read_ldr() >= LIMITE && current_light_state == 0){
                
                

                Serial.print(F("\n\n\t\t------AVISO LDR------\n"));
                Serial.print(F("\t\tLEITURA:  "));
                Serial.print(a_read_ldr());
                Serial.print(F("\n\t\t!!!INCOMPATIVEL!!!"));
                Serial.print(F("\n\t\tTENTANDO DESLIGAR LUZ"));
                Serial.print(F("\n\t\t---------------------\n\n"));
                
                light_turn_off();
               
                if(menu_location==HOME_LIGHT) show_light_data();
                if(menu_location==HOME_EXHAUSTER) show_exhauster_data();  
                if(menu_location==HOME_HUMIDITY) show_humidity_data();    
               
            }
            // Se estiver escuro e o estado for claro
            else if(a_read_ldr() < LIMITE && current_light_state == 1){
                
                
                
                Serial.print(F("\n\n\t\t------AVISO LDR------\n"));
                Serial.print(F("\t\tLEITURA:  "));
                Serial.print(a_read_ldr());
                Serial.print(F("\n\t\t!!!INCOMPATIVEL!!!"));
                Serial.print(F("\n\t\tTENTANDO LIGAR LUZ"));
                Serial.print(F("\n\t\t---------------------\n\n"));
                
                light_turn_on();
               
                if(menu_location==HOME_LIGHT) show_light_data();
                if(menu_location==HOME_EXHAUSTER) show_exhauster_data();  
                if(menu_location==HOME_HUMIDITY) show_humidity_data();    


               
            }
            else{
                if(menu_location==HOME_LIGHT) Serial.print(F("\n\t\tLDR: OK!"));
            }

            // Fora esses dois casos, não precisa de mais atuação
            // init_time = micros() - init_time;
            // imprimir(F("Resposta LDR (us)"), init_time);

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
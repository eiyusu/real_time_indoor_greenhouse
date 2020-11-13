#ifndef HUMIDITY_H
#define HUMIDITY_H

#include "monitor.h"

//Variáveis de umidade
#define led_bomba 7
#define sensor_umidade A0
#define led_exaustor 6
#define t_humidity 15000/portTICK_PERIOD_MS

//Variáveis externas
static uint8_t config_umidade = 50; // apagar
static bool umidade_acionada = false, bomba_acionada = false;
static uint8_t umidade = 0;

extern void show_humidity_data();
extern uint8_t menu_location;
#ifndef HOME_HUMIDITY
#define HOME_HUMIDITY 3
#endif

// Task de leitura e tratamento da umidade
void leitura_umidade(void *arg){
    
    // unsigned long int init_time, end_time, resp_time; 
    // unsigned long u_time; 
    for(;;) {
        //Serial.print("\n\n- TASK ");
        //Serial.print(pcTaskGetName(NULL)); 
        //Serial.print(", High Watermark: ");
        //Serial.print(uxTaskGetStackHighWaterMark(NULL));
        
        //u_time = micros();
        if(umidade_acionada){                
            // Reserva sensor de leitura da umidade, faz a leitura e libera      
            lock(IRRIGATION);
            umidade = map(analogRead(sensor_umidade), 0, 1023, 0, 100);
            unlock(IRRIGATION);

            //Umidade dentro do configurado
            if(umidade >= config_umidade-5 && umidade <= config_umidade+5){ 
                if(menu_location==HOME_HUMIDITY) show_humidity_data();    
            }       
            //Liga bomba
            else if (umidade < config_umidade-5){
              
                digitalWrite(led_bomba, HIGH);  
                bomba_acionada = true;          
                
                if(menu_location==HOME_HUMIDITY) show_humidity_data();

                vTaskDelay(t_humidity);                     

                digitalWrite(led_bomba, LOW);    
                bomba_acionada = false;        
                if(menu_location==HOME_HUMIDITY) show_humidity_data();
            }       
            
            //Liga exaustor
            else{
                if(menu_location==HOME_HUMIDITY) show_humidity_data();
                
                // EXA_IRRIGATION indica que umidade vai assumir a exaustão
                set_value(2, EXA_IRRIGATION);  
                
                // Verifica se o exaustor está desligado para ligá-lo    
                if(get_value(EXA_STATE)==0){ 
                    set_value(1, EXA_STATE);
                    digitalWrite(led_exaustor, HIGH);
                }
                vTaskDelay(t_humidity);     //Bloqueia a task por um período mantendo exaustor ligado
                
                // Reserva recurso exaustor, desliga e libera. Em seguida muda variável indicando que umidade não está mais no controle
                digitalWrite(led_exaustor, LOW);
                set_value(0, EXA_STATE);
                set_value(1, EXA_IRRIGATION);
            }       
            
            //u_time = micros() - u_time;
            //imprimir(F("Resposta Umidade (us)"), u_time);
            vTaskDelay(t_humidity/2);                       //Bloqueia a task

        }
        else{
            vTaskDelay(t_humidity/2);                       //Bloqueia a task
        }
    }
}

//Setup inicial do sistema
void humidity_setup(){
    pinMode(led_bomba, OUTPUT);

    //Crição da task de leitura de umidade
    xTaskCreate(leitura_umidade,        //Funcao
                NULL,      //Nome
                100,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                NULL);
}


#endif
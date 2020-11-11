#ifndef HUMIDITY_H
#define HUMIDITY_H

#include "monitor.h"


//Variáveis de umidade
#define led_bomba 3
#define led_su 4
#define sensor_umidade A0
#define led_exaustor 2
#define t_humidity 10000/portTICK_PERIOD_MS

//Variáveis externas
static uint8_t config_umidade = 50;
static bool umidade_acionada = true;
static uint8_t umidade = 0;


// Task de leitura e tratamento da umidade
void leitura_umidade(void *arg){
    
    // unsigned long int init_time, end_time, resp_time; 
    
    for(;;) {

        Serial.print("\n\n- TASK ");
        Serial.print(pcTaskGetName(NULL)); 
        Serial.print(", High Watermark: ");
        Serial.print(uxTaskGetStackHighWaterMark(NULL));


        if(umidade_acionada){
            // init_time = millis();
            
            digitalWrite(led_su, HIGH);     
                      //Acende LED indicador de task em execução       
            // Reserva sensor de leitura da umidade, faz a leitura e libera      
            lock(IRRIGATION);
            umidade = map(analogRead(sensor_umidade), 0, 1023, 0, 100);
            unlock(IRRIGATION);
            imprimir("Umidade", umidade);       
            
            //Umidade dentro do configurado
            if(umidade >= config_umidade-5 && umidade <= config_umidade+5){ 
                Serial.println("\t\tUmidade OK");     
            }       
            
            //Liga bomba
            else if (umidade < config_umidade-5){
                Serial.println("\t\tUmidade baixa");
                // Reserva recurso BOMBA, liga a bomba e avisa que ela foi ligada, depois libera
                lock(PIN_BOMBA);
                digitalWrite(led_bomba, HIGH);            
                Serial.println("\t\tBomba ligada"); 
                unlock(PIN_BOMBA);
                vTaskDelay(t_humidity/4);                     //Bloqueia a task por uma período para retornar e desligar a bomba
                // Reserva recurso BOMBA, liga a bomba e avisa que ela foi ligada, depois libera
                lock(PIN_BOMBA);
                digitalWrite(led_bomba, LOW);            
                Serial.println("\t\tBomba desligada");
                unlock(PIN_BOMBA); 
            }       
            
            //Liga exaustor
            else{
                Serial.println("\t\tUmidade alta");
                
                // EXA_IRRIGATION indica que umidade vai assumir a exaustão
                set_value(1, EXA_IRRIGATION);  
                
                // Verifica se o exaustor está desligado para ligá-lo    
                if(get_value(EXA_STATE)==0){ 
                    set_value(1, EXA_STATE);
                    // Reserva recurso exaustor, liga e libera
                    lock(PIN_EXAUSTOR);
                    digitalWrite(led_exaustor, HIGH);
                    Serial.println("\t\t[Umidade] Exaustor ligado"); 
                    unlock(PIN_EXAUSTOR);
                }
                vTaskDelay(t_humidity);     //Bloqueia a task por um período mantento exaustor ligado
                
                // Reserva recurso exaustor, desliga e libera. Em seguisa muda variável indicando que umidade não está mais no controle
                lock(PIN_EXAUSTOR);
                digitalWrite(led_exaustor, LOW);
                Serial.println("\t\t[Umidade] Exaustor desligado"); 
                unlock(PIN_EXAUSTOR);
                set_value(0, EXA_IRRIGATION);
                // A umidade não desliga o exaustor porque porque se a task de mudar estado do exaustor chegar, ele faz a troca imediatamente 
                // ligando o exaustor logo após a umidade desligar
            }       
            // end_time = millis();
            // resp_time = end_time - init_time;
            // imprimir("Resposta Umidade (ms)", resp_time);
            
            digitalWrite(led_su, LOW);                //Desliga LED sinalizador de task 
            vTaskDelay(t_humidity);                       //Bloqueia a task
        }
    }
}

//Setup inicial do sistema
void humidity_setup(){
    pinMode(led_su, OUTPUT);
    pinMode(led_bomba, OUTPUT);

    //Crição da task de leitura de umidade
    xTaskCreate(leitura_umidade,        //Funcao
                "leitura_umidade",      //Nome
                95,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                NULL);
}


#endif
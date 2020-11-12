#ifndef HUMIDITY_H
#define HUMIDITY_H

#include "monitor.h"


//Variáveis de umidade
#define led_bomba 4
#define sensor_umidade A0
#define led_exaustor 6
#define t_humidity 10000/portTICK_PERIOD_MS

//Variáveis externas
static uint8_t config_umidade = 50; // apagar
static bool umidade_acionada = false, bomba_acionada = false;
static uint8_t umidade = 0;


// Task de leitura e tratamento da umidade
void leitura_umidade(void *arg){
    
    // unsigned long int init_time, end_time, resp_time; 
    unsigned long u_time; 
    for(;;) {

        //Serial.print("\n\n- TASK ");
        //Serial.print(pcTaskGetName(NULL)); 
        //Serial.print(", High Watermark: ");
        //Serial.print(uxTaskGetStackHighWaterMark(NULL));
        
        u_time = micros();
        if(umidade_acionada){
            // init_time = millis();
                
                      //Acende LED indicador de task em execução       
            // Reserva sensor de leitura da umidade, faz a leitura e libera      
            lock(IRRIGATION);
            umidade = map(analogRead(sensor_umidade), 0, 1023, 0, 100);
            unlock(IRRIGATION);
            // imprimir("Umidade", umidade);       
            char line_text[16];
            //Umidade dentro do configurado
            if(umidade >= config_umidade-5 && umidade <= config_umidade+5){ 
                snprintf_P(line_text, sizeof(line_text), PSTR("Umi. OK: %u%%"), umidade);
                Serial.println(line_text);  
            }       
            //Liga bomba
            else if (umidade < config_umidade-5){
                char line_text[16];
                snprintf_P(line_text, sizeof(line_text), PSTR("Umi. Baixa: %u%%"), umidade);
                Serial.println(line_text); 
                // Reserva recurso BOMBA, liga a bomba e avisa que ela foi ligada, depois libera
                lock(PIN_BOMBA);
                digitalWrite(led_bomba, HIGH);  
                bomba_acionada = true;          
                Serial.println("\t\tBomba ligada"); 
                unlock(PIN_BOMBA);
                vTaskDelay(t_humidity);                     //Bloqueia a task por uma período para retornar e desligar a bomba
                // Reserva recurso BOMBA, liga a bomba e avisa que ela foi ligada, depois libera
                lock(PIN_BOMBA);
                digitalWrite(led_bomba, LOW);    
                bomba_acionada = false;        
                Serial.println("\t\tBomba desligada");
                unlock(PIN_BOMBA); 
            }       
            
            //Liga exaustor
            else{
                char line_text[16];
                snprintf_P(line_text, sizeof(line_text), PSTR("Umi. Alta: %u%%"), umidade);
                Serial.println(line_text); 
                
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
                set_value(0, EXA_STATE);
                unlock(PIN_EXAUSTOR);
                set_value(0, EXA_IRRIGATION);
                // A umidade não desliga o exaustor porque porque se a task de mudar estado do exaustor chegar, ele faz a troca imediatamente 
                // ligando o exaustor logo após a umidade desligar
            }       
            // end_time = millis();
            // resp_time = end_time - init_time;
            // imprimir("Resposta Umidade (ms)", resp_time);
            
            u_time = micros() - u_time;
            imprimir(F("Resposta Umidade (us)"), u_time);
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
                "leitura_umidade",      //Nome
                95,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                NULL);
}


#endif
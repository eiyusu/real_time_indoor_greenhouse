#ifndef HUMIDITY_H
#define HUMIDITY_H

#include "monitor.h"


//Variáveis de umidade
#define led_bomba 8
#define led_su 7
#define sensor_umidade A0
#define led_exaustor 12

//Variáveis externas
extern int config_umidade;
extern bool umidade_acionada;
extern int umidade;
extern int minuto;
extern void imprimir(char *name, int8_t value);

TaskHandle_t        leitura_umidadeH;

// Task de leitura e tratamento da umidade
void leitura_umidade(void *arg){
    unsigned long int init_time, end_time, resp_time; 
    while(1) {
        if(umidade_acionada){
            init_time = millis();
            digitalWrite(led_su, HIGH);               //Acende LED indicador de task em execução        
            //Monitor para substituir leitura de umidade abaixo
            /*
            set_value(map(analogRead(sensor_umidade), 0, 1023, 0, 100), 1);
            umidade = get_value(1);
            */      
            umidade = analogRead(sensor_umidade);
            umidade = map(umidade, 0, 1023, 0, 100);
            imprimir("Umidade", umidade);       
            //Umidade dentro do configurado
            if(umidade >= config_umidade-5 && umidade <= config_umidade+5){ 
                Serial.println("Dentro do esperado");     
            }       
            //Liga bomba
            else if (umidade < config_umidade-5){
                digitalWrite(led_bomba, HIGH);            //Bomba ligada
                Serial.println("Ligar bomba de água"); 
                vTaskDelay(minuto/4);                     //Bomba ligada por 1/4 do ciclo
                digitalWrite(led_bomba, LOW);             //Desliga bomba
                Serial.println("Desligar bomba de água"); 
            }       
            //Liga exaustor
            else{
                set_value(1, EXA_IRRIGATION);      
                if(get_value(EXA_STATE)==0){
                    set_value(1, EXA_STATE);
                    digitalWrite(led_exaustor, HIGH);
                    Serial.println("Umidade - Ligar exaustor"); 
                }
                vTaskDelay(minuto);     //Bloqueia a task por um período mantento ele ligado
                digitalWrite(led_exaustor, LOW);
                //Serial.println("Umidade - Desligar exaustor"); 
                set_value(0, EXA_IRRIGATION);
                //A umidade não desliga o exaustor porque porque se a task de mudar estado do exaustor chegar, ele faz a troca imediatamente 
                //set_value(0, EXA_STATE);
            }       
            end_time = millis();
            resp_time = end_time - init_time;
            imprimir("Resposta Umidade (ms)", resp_time);
            digitalWrite(led_su, LOW);                //Desliga LED sinalizador de task 
            vTaskDelay(minuto);                       //Bloqueia a task
        }
    }
    vTaskDelete(NULL);      //Deleta a Task atual

}

//Setup inicial do sistema
void setup_humidity(){
    pinMode(led_su, OUTPUT);
    pinMode(led_bomba, OUTPUT);

    //Crição da task de leitura de umidade
    xTaskCreate(leitura_umidade,        //Funcao
                "leitura_umidade",      //Nome
                128,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                &leitura_umidadeH);
}


#endif
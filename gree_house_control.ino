#include "Arduino_FreeRTOS.h"
#include <math.h>
#include <timers.h>
#include <queue.h>
#include <semphr.h>
#include "task.h"

#include "monitor.h"
#include "light.h"

/*
Iuminação = 0
Umidade = 1
*/
 
// variavel para sistema de iluminação
BaseType_t qparameter = pdTRUE;
// função para testar interrupção por botao
void handleInterrupt(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, &qparameter );
}


int minuto = 10000/portTICK_PERIOD_MS;

//Variáveis de umidade
#define led_bomba 8
#define led_su 7
#define sensor_umidade A0

int config_umidade = 50;
bool umidade_acionada = false, exa_umi = false;
int umidade = 0;

//Variáveis do exaustor
#define led_exaustor 4

int t_exaustor = minuto;
bool exa_on = false, exaustor_acionado = false;

 
//Tasks
TaskHandle_t        leitura_umidadeH;
TaskHandle_t        exaustorH;
 
void imprimir(const char *name, unsigned int value) {
       Serial.print("\t\t");
       Serial.print(name);
       Serial.print(": ");
       Serial.print(value);
       Serial.print("\n");
}
 
void setup() {
    //Inicializa Serial
    Serial.begin(115200);

    //PinModes
    pinMode(led_su, OUTPUT);
    pinMode(led_bomba, OUTPUT);
    pinMode(led_exaustor, OUTPUT);
     
    //Crição da task de leitura de umidade
    xTaskCreate(leitura_umidade,        //Funcao
                "leitura_umidade",      //Nome
                128,                    //Pilha
                NULL,                   //Parametro
                1,                      //Prioridade
                &leitura_umidadeH);

    //Criação da task de controle do exaustor
    xTaskCreate(exaustor,           //Funcao
                "exaustor",         //Nome
                128,                //Pilha
                NULL,               //Parametro
                1,                  //Prioridade
                &exaustorH); 

    //setup do sistema de iluminação
    light_setup();
    attachInterrupt(digitalPinToInterrupt(2), handleInterrupt , RISING );

    Serial.println("SETUP");
}
 
void loop() {
    // Nada é feito aqui, Todas as funções são feitas em Tasks
}
 
// Task de leitura e tratamento da umidade
void leitura_umidade(void *arg){
    if(umidade_acionada){
        unsigned int init_time, end_time, resp_time; 

        while(1) {
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
              digitalWrite(led_su, LOW);              //Apaga LED indicador de task em execução       
            }

            //Liga bomba
            else if (umidade < config_umidade-5){
              digitalWrite(led_bomba, HIGH);            //Bomba utilizada apenas aqui - necessário monitor?
              vTaskDelay(minuto/4);                     //Bomba ligada por 1/4 do ciclo
              digitalWrite(led_bomba, LOW);             //Desliga bomba
              digitalWrite(led_su, LOW);                //Desliga LED sinalizador de task 
            }

            //Liga exaustor
            else{
              exa_umi = true;       // Variável temporária de controle

              if(!exa_on){
                  digitalWrite(led_exaustor, HIGH);
                  exa_on = true;
              }
              
              vTaskDelay(minuto/4);     //Bloqueia a task por um período mantento ele ligado
              exa_umi = false;
            }

            end_time = millis();
            resp_time = end_time - init_time;
            imprimir("Resposta Umidade", resp_time);
            vTaskDelay(minuto);                       //Bloqueia a task
        }
        vTaskDelete(NULL);      //Deleta a Task atual
    }
}

void exaustor(void *arg){

      if(exaustor_acionado){
          while(1){
              // digitalWrite no exaustor deve seguir a exclusão mútua com prioridade para a umidade (fazer no monitor)
              // exa_umi é um controle temporário
              if(exa_on && !exa_umi){
                  digitalWrite(led_exaustor, LOW);
                  vTaskDelay(t_exaustor);
              }
              else if(!exa_on && !exa_umi){
                  digitalWrite(led_exaustor, HIGH);
                  vTaskDelay(t_exaustor);
              }
          }
          vTaskDelete(NULL);      //Deleta a Task atual
      }
}

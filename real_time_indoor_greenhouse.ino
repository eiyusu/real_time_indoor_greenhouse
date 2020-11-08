#include "Arduino_FreeRTOS.h"
#include <math.h>
#include <timers.h>
#include <queue.h>
#include <semphr.h>
#include "task.h"

#include "monitor.h"
#include "light.h"
#include "humidity.h"
#include "exhauster.h"

#define LDR 0
#define IRRIGATION 1
#define EXA_STATE 2
#define EXA_IRRIGATION 3
 
// variavel para sistema de iluminação
BaseType_t qparameter = pdTRUE;
// função para testar interrupção por botao
void handleInterrupt(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, &qparameter );
}


int minuto = 10000/portTICK_PERIOD_MS;

//Variáveis de umidade
int config_umidade = 50;
bool umidade_acionada = true, exa_umi = false;
int umidade = 0;

//Variáveis do exaustor
int t_exaustor = minuto;
bool exa_on = false, exaustor_acionado = true;
 
void imprimir(char *name, int8_t value) {
       Serial.print("\t\t");
       Serial.print(name);
       Serial.print(": ");
       Serial.print(value);
       Serial.print("\n");
}
 
void setup() {
    //Inicializa Serial
    Serial.begin(115200);

    //set_value(0, EXA_STATE);
    //set_value(0, EXA_IRRIGATION);


    //setup do sistema de iluminação
    light_setup();

    //setup do sistema de umidade
    setup_humidity();

    //setup do sistema de exaustao 
    exa_setup();

    attachInterrupt(digitalPinToInterrupt(2), handleInterrupt , RISING );

    Serial.println("SETUP");
}
 
void loop() {
    // Nada é feito aqui, Todas as funções são feitas em Tasks
}

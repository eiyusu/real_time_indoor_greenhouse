#include "Arduino_FreeRTOS.h"
#include <math.h>
#include <timers.h>
#include <queue.h>
#include <semphr.h>
#include "task.h"


void imprimir(char *name, unsigned long value) {
       Serial.print("\t\t");
       Serial.print(name);
       Serial.print(": ");
       Serial.print(value);
       Serial.print("\n");
}
 

#include "monitor.h"
#include "light.h"
#include "humidity.h"
#include "exhauster.h"
#include "buttons.h"
// #include "ldr.h"


// função para testar interrupção por botao
void handleInterrupt(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, (BaseType_t *) pdTRUE );
}

void setup() {
    //Inicializa Serial
    Serial.begin(115200);

    //Inicializa monitor
    monitor_start();

    buttons_setup();
    //set_value(0, EXA_STATE);
    //set_value(0, EXA_IRRIGATION);


    //setup do sistema de iluminação
    light_setup();

    //setup do sistema de umidade
    setup_humidity();

    //setup do sistema de exaustao 
    exa_setup();

    // Serial.println("SETUP");
}
 
void loop() {
    // Nada é feito aqui, Todas as funções são feitas em Tasks
}

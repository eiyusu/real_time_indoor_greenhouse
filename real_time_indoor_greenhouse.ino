#include "Arduino_FreeRTOS.h"
#include <math.h>
#include <timers.h>
#include <queue.h>
#include <semphr.h>
#include "task.h"


void imprimir(const __FlashStringHelper*name, unsigned long value) {
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
#include "ldr.h"




void setup() {
    //Inicializa Serial
    Serial.begin(115200);

    //Inicializa monitor
    // monitor_start();

    // light_setup();
    buttons_setup();
    //set_value(0, EXA_STATE);
    //set_value(0, EXA_IRRIGATION);


    //setup do sistema de iluminação
    // ldr_setup();

    //setup do sistema de umidade
    // humidity_setup();

    //setup do sistema de exaustao 
    // exa_setup();

    // Serial.println("SETUP");
}
 
void loop() {
    // Nada é feito aqui, Todas as funções são feitas em Tasks
}

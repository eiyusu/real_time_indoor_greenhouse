#ifndef LIGHT_H
#define LIGHT_H

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <timers.h>
#include <queue.h>

#define LIGHTPIN 5 // pino onde ficará o rele/led
#define DARK 0 // posição no vetor para o tempo de escuro
#define LIGHT 1 // posição no vetor para o tempo de claro

//Pegar isso no monitor
unsigned long times[2] = { 5000, 5000}; // tempos em cada estado (ms)
TimerHandle_t light_timer; // timer do sistema 

static bool enableLight = false; // status se o sistema está ativo ou não
static bool current_light_state = DARK; // estado atual do sistema (CLARO OU ESCURO)
QueueHandle_t enable_disable_Q = xQueueCreate(1, sizeof(bool)); // fila de comunicação para atiação e desatiação do sistema


//fuções para ficar mais legiel quando estiver pedindo para ligar ou desligar a luz
//***Talvez tenha que adicionar ao monitor se o ldr tbm for usar isso... 
void light_turn_on(){
    digitalWrite(LIGHTPIN, HIGH);
}

void light_turn_off(){
    digitalWrite(LIGHTPIN, LOW);
}


/* 
  Alterar o estado atual para o seguinte. 
  Mudar o valor do timer para o valor do estado correspondente
*/
void light_state_toggle(TimerHandle_t xTimer){
  digitalWrite(LIGHTPIN, !current_light_state); // ***mudar para uma  função do monitor se o ldr tbm for usar
  current_light_state = !current_light_state; //alterando o estado atual
  Serial.print("changing current_light_state to: ");
  Serial.println(current_light_state ? "LIGHT" : "DARK");
  xTimerChangePeriod(xTimer, pdMS_TO_TICKS(times[current_light_state]), 0);//alterar valor do timer
}


/*
Task que espera uma interrupção colocar um valor na fila.
Quando isso ocorre ela olha esse valor e desabilita ou habilita o timer
*/
void light_enable_disable( void *pv){
  bool L; //buffer para guardar valor que vem da fila
  unsigned long l_time; 
  for(;;){

    // Serial.print("\n\n- TASK ");
    // Serial.print(pcTaskGetName(NULL)); 
    // Serial.print(", High Watermark: ");
    // Serial.print(uxTaskGetStackHighWaterMark(NULL));

    //Aguardar receber conteudo da fila. Então salvar esse conteudo na variavel L
    if ( xQueueReceive( enable_disable_Q, &L, portMAX_DELAY) == pdPASS) {
      l_time = micros();
      if(L){
       xTimerStart( light_timer, 0 );
       Serial.println("Iniciou o timer");
      }
      else{
        xTimerStop( light_timer, 0 );
        digitalWrite(LIGHTPIN, LOW); //*** mudar para monitor 
        Serial.println("Parou o timer");
      }
      l_time = micros()-l_time;
      imprimir(F("Resposta Ilumincação (us)"), l_time);
    }
  }
}

// estrutura do valor que dee ser passado para a fila de mudaça do valor dos tempos de claro e escuro
struct changeValueData {
  bool alter_state; // estado sendo alterado
  uint16_t newValue; // novo valor
};

void light_change_value(void *pv){
  
  struct changeValueData* data; //buffer para guardar valor que vem da fila

  for(;;){
    //***Tem que ser outra fila e não a mesma que ta usando para desabilitar e habilitar o timer
    if( xQueueReceive( enable_disable_Q, data, portMAX_DELAY) == pdPASS ){
      times[data->alter_state] = data->newValue; //mudando valor no vetor
      if(current_light_state == data->alter_state){
         xTimerChangePeriod( light_timer, pdMS_TO_TICKS(data->newValue), 0 ); //se o valor sendo mudado é o do estado atual, alterar tbm o timer
      }
    }
  }
}

void light_setup(){
    pinMode(LIGHTPIN, OUTPUT);
    
    light_timer = xTimerCreate(
        "lightTimer", //Nome
        pdMS_TO_TICKS(500), //Periodo
        pdTRUE, // repetir
        NULL, // ID do timer
        light_state_toggle // funcao de callback 
    ); 

    xTaskCreate(
        light_enable_disable, // implementação da task
        "light_s", // nome para debug
        95, // espaço reservado na memoria
        NULL, // parametro para ser passado para task 
        1, // prioridade da task
        NULL //handler para manipulação da task
    );
}

#endif
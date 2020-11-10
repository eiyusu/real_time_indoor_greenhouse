#ifndef BUTTON_H
#define BUTTON_H


#include <Arduino_FreeRTOS.h>
#include <queue.h>

//Definições para facilitar identificar botoes
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//pinos em que os botoes vao ser usados
#define UPPIN 13
#define DOWNPIN 12
#define LEFTPIN 11
#define RIGHTPIN 10

//pino com capacidade de interrupção
#define INTERRUPT 2

uint8_t buttons_pins[4] = {UPPIN, DOWNPIN, LEFTPIN, RIGHTPIN};//vetor com os pinos definidos

unsigned long lastFire = 0; // variavel para fazer o debouce do botao (evitar um pouco do ruido quando a gente aperta o botao)

/*
	fila para identiicar o aperto de um botao
	***talvez aumentar o tamanho dela para bufferizar apertos sequenciais 
*/
QueueHandle_t button_press = xQueueCreate(1, sizeof(uint8_t)); 
BaseType_t qparameter = pdTRUE; // parametro necessário para criar fila


//Coloca botoes no modo de identificar quem foi apertado
void configureDistinct() {
 int i = 0;
 pinMode(INTERRUPT, OUTPUT);
 digitalWrite(INTERRUPT, LOW);
 for (i = 0; i < sizeof(buttons_pins) / sizeof(uint8_t); i++) {
   pinMode(buttons_pins[i], INPUT_PULLUP);
 }
}


//Coloca botoes no modo de espera por aperto
void configureCommon() {
 int i = 0;
 pinMode(INTERRUPT, INPUT_PULLUP);
 for (i = 0; i < sizeof(buttons_pins) / sizeof(uint8_t); i++) {
   pinMode(buttons_pins[i], OUTPUT);
   digitalWrite(buttons_pins[i], LOW);
 }
}


//função que irá ser chamada para tratar a interrupção causada pelo aperto de um botao
void button_interrupt(){
	int i = 0;
	//verificação para falso click (ruido no aperto)
	if (millis() - lastFire >= 100) { 	
		lastFire = millis();

		configureDistinct(); // colocar todos os botoes na configuração para leitura 

		//ler botoes e achar aquele que foi apertado
		for (i = 0; i < sizeof(buttons_pins) / sizeof(uint8_t); i++) {
			if (!digitalRead(buttons_pins[i])) {
				//adicionar informação na fila
				xQueueSendToFrontFromISR(button_press, &i, &qparameter );
				Serial.println("oioio");
			}
		}
		configureCommon();
  }
}

//tarefa só pra testar se ta funcionando
void testTask(void *pv){
  
  int data;

  for(;;){
    if( xQueueReceive( button_press, &data, portMAX_DELAY) == pdPASS ){
		Serial.println("IN TASK");
			Serial.println(data);
		}
	}

}

//função para ser chamada no setup do arduino
void buttons_setup(){
		configureCommon();
    attachInterrupt(digitalPinToInterrupt(INTERRUPT), button_interrupt, FALLING);

		xTaskCreate(
        testTask, // Task function
        "testTask", // Task name for humans
        128, 
        NULL, // Task parameter
        1, // Task priority
        NULL
    );
}

#endif
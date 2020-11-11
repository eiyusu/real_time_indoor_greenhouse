#ifndef BUTTON_H
#define BUTTON_H


#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "light.h"
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

const uint8_t buttons_pins[4] PROGMEM  = {UPPIN, DOWNPIN, LEFTPIN, RIGHTPIN};//vetor com os pinos definidos


/*
	fila para identiicar o aperto de um botao
	***talvez aumentar o tamanho dela para bufferizar apertos sequenciais 
*/
QueueHandle_t button_press = xQueueCreate(1, sizeof(uint8_t)); 


//Coloca botoes no modo de identificar quem foi apertado
void configureDistinct() {
 uint8_t i = 0;
 pinMode(INTERRUPT, OUTPUT);
 digitalWrite(INTERRUPT, LOW);
 for (i = 0; i < 4; i++) {
   pinMode(buttons_pins[i], INPUT_PULLUP);
 }
}

//Coloca botoes no modo de espera por aperto
void configureCommon() {
 uint8_t i = 0;
 pinMode(INTERRUPT, INPUT_PULLUP);
 for (i = 0; i < 4; i++) {
   pinMode(buttons_pins[i], OUTPUT);
   digitalWrite(buttons_pins[i], LOW);
 }
}


BaseType_t qparam = pdTRUE;
//função que irá ser chamada para tratar a interrupção causada pelo aperto de um botao
	uint8_t i = 0;
void button_interrupt(){
	//verificação para falso click (ruido no aperto)

		configureDistinct(); // colocar todos os botoes na configuração para leitura 

		//ler botoes e achar aquele que foi apertado
		for (i = 0; i < 4; i++) {
			if (!digitalRead(buttons_pins[i])) {
				//adicionar informação na fila
				xQueueSendToFrontFromISR(button_press, &i, &qparam );
				Serial.println(i);
				Serial.println("Interrupção --> Lendo botoes");
			}
		}
		configureCommon();
  }



#define HOME_LIGHT 0
#define CONFIG_LIGHT_TIME 1
#define HOME_HUMIDITY 2
#define CONFIG_HUMIDITY_TIME 3
#define HOME_EXHAUSTER 4

static uint8_t menu_location = HOME_LIGHT;



BaseType_t qpa = pdTRUE;
void show_light_data(){
	char line_text[16];
	// lcd.setCursor(1,0);
	snprintf_P(line_text, sizeof(line_text), PSTR("Enable: %s"), enableLight? PSTR("ON") : F("OFF"));
	Serial.println(line_text);
	
	// lcd.setCursor(0,1);
	snprintf_P(line_text, sizeof(line_text), PSTR("State: %s"), current_light_state ? F("LIGHT") : F("DARK"));
	Serial.println(line_text);

}

void enable_disable_light(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, &qpa );
	show_light_data();
}

void show_humidity_data(){
	char line_text[16];
	// lcd.setCursor(1,0);
	snprintf_P(line_text, sizeof(line_text), PSTR("Enable: %s"), current_light_state ?  F("ON") : F("OFF"));
	Serial.println(line_text);
	
	// lcd.setCursor(0,1);
	snprintf_P(line_text, sizeof(line_text), PSTR("H: %u/% B: %s"), (uint8_t)30,current_light_state ?  F("ON") : F("OFF"));
	Serial.println(line_text);

}

void navigateUmi(){
	menu_location = HOME_HUMIDITY;
	show_humidity_data();
}
void navigateLight(){
	menu_location = HOME_LIGHT;
	show_light_data();
}

void navigateConfigLight(){
	char line_text[16];
	menu_location = CONFIG_LIGHT_TIME;
	snprintf_P(line_text, sizeof(line_text), PSTR("L: %s m"), times[LIGHT]);
	Serial.println(line_text);
	
	// lcd.setCursor(0,1);
	snprintf_P(line_text, sizeof(line_text), PSTR("D: %s m"), times[DARK]);
	Serial.println(line_text);
}

void button_router(void *pv){
  
	
  uint8_t pressed_button;
	static void (*actions[4]) ();

  for(;;){
    if( xQueueReceive( button_press, &pressed_button, portMAX_DELAY) == pdPASS ){
		Serial.println("BOTAOO");
		switch (menu_location){
			case HOME_LIGHT:

				actions[UP] = navigateUmi;
				// actions[DOWN] = ni;
				actions[LEFT] = enable_disable_light;
				actions[RIGHT] = navigateConfigLight;

				actions[pressed_button]();
				break;
			
			// case CONFIG_LIGHT_TIME:
			// 	actions[UP] = na;
			// 	actions[DOWN] = ni;
			// 	actions[LEFT] = ne;
			// 	actions[RIGHT] = nu;
			// 	actions[pressed_button]();
			// 	break;
			
			// case HOME_HUMIDITY:
			// 	actions[UP] = na;
			// 	actions[DOWN] = ni;
			// 	actions[LEFT] = ne;
			// 	actions[RIGHT] = nu;
			// 	actions[pressed_button]();
			// 	break;

			default:
				break;
		}


	}

}
}
//função para ser chamada no setup do arduino

void testTask(void *pv){
  
  int data;

  for(;;){
    if( xQueueReceive( button_press, &data, portMAX_DELAY) == pdPASS ){
		Serial.println("IN TASK");
			Serial.println(data);
		}
	}

}


void buttons_setup(){
		configureCommon();
    attachInterrupt(digitalPinToInterrupt(INTERRUPT), button_interrupt, FALLING);
		xTaskCreate(
        testTask, // Task function
        "b_router", // Task name for humans
        128	, 
        NULL, // Task parameter
        1, // Task priority
        NULL
    );

		show_light_data();
}

#endif
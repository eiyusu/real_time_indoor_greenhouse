#ifndef BUTTON_H
#define BUTTON_H


#include <Arduino_FreeRTOS.h>
#include <queue.h>
#include "monitor.h"
#include "light.h"
#include "humidity.h"
#include "exhauster.h"

#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//pinos em que os botoes vao ser usados
#define UPPIN (const uint8_t) 13
#define DOWNPIN (const uint8_t) 12
#define LEFTPIN (const uint8_t) 11
#define RIGHTPIN (const uint8_t) 10

//pino com capacidade de interrupção
#define INTERRUPT 2

// Locais de menu
#define HOME_LIGHT 0
#define CONFIG_LIGHT_TIME 1
#define CONFIG_DARK_TIME 2
#define HOME_HUMIDITY 3
#define CONFIG_HUMIDITY 4
#define HOME_EXHAUSTER 5

static uint8_t menu_location = HOME_LIGHT;

const uint8_t buttons_pins[4]  = {UPPIN, DOWNPIN, LEFTPIN, RIGHTPIN};
unsigned long lastFire = 0; //Evitar leituras múltiplas 
QueueHandle_t button_press = xQueueCreate(1, sizeof(uint8_t));
BaseType_t qparameter = pdTRUE;


void configureDistinct() {
 pinMode(INTERRUPT, OUTPUT);
 digitalWrite(INTERRUPT, LOW);
 for (uint8_t i = 0; i < 4; i++) {
   pinMode(buttons_pins[i], INPUT_PULLUP);
 }
}

void configureCommon() {
 pinMode(INTERRUPT, INPUT_PULLUP);
 for (uint8_t i = 0; i < 4; i++) {
   pinMode(buttons_pins[i], OUTPUT);
   digitalWrite(buttons_pins[i], LOW);
 }
}

//função que irá ser chamada para tratar a interrupção causada pelo aperto de um botao
void button_interrupt(){
  uint8_t i = 0;
  //verificação para falso click 
  if (millis() - lastFire >= 30) {  
    lastFire = millis();

    configureDistinct(); // colocar todos os botoes na configuração para leitura 

    //ler botoes e achar aquele que foi apertado
    for (i = 0; i < 4; i++) {
      if (!digitalRead(buttons_pins[i])) {
        //adicionar informação na fila
        xQueueSendToFrontFromISR(button_press, &i, &qparameter );
        
      }
    }
    configureCommon();
  }
}

// Funções de iluminação
void show_light_data(){
  const char ON[] PROGMEM= "ON";
  const char OFF[] PROGMEM= "OFF";
  const char LI[] PROGMEM= "LUZ";
  const char DA[] PROGMEM= "ESCURO";

  
  
  Serial.print(F("\n\n\t\t-----ILUMINACAO-----\n"));
  Serial.print(F("\t\tATIVADO: "));
  Serial.print(enableLight? ON : OFF);
  Serial.print(F("\n\t\tESTADO ATUAL: "));
  Serial.print(current_light_state ? LI  : DA);
  Serial.print(F("\n\t\t--------------------\n\n"));

}

void enable_disable_light(){
  enableLight = !enableLight;
  xQueueSendToFrontFromISR(enable_disable_Q, &enableLight, &qparameter );
  show_light_data();
}

void navigateLight(){
  menu_location = HOME_LIGHT;
  show_light_data();
}

void navigateConfigLight(){
	if(!enableLight){
		menu_location = CONFIG_LIGHT_TIME;

    
    
    Serial.print(F("\n\n\t\t----CONFIG>TEMPO----\n"));
    Serial.print(F("\t\tLUZ: "));
    Serial.print(times[LIGHT]/60000);
    Serial.print(F(" min"));
    Serial.print(F("\n\t\t---------------------\n\n"));
	}
	else{
    
    
		Serial.print(F("\n\n\t\t----CONFIG>TEMPO----\n"));
    Serial.print(F("\t\tPROIBIDO-->SIS. ACIONADO"));
    Serial.print(F("\n\t\t---------------------\n\n"));
	}
}

void navigateConfigDark(){
  menu_location = CONFIG_DARK_TIME; 
 
  
  Serial.print(F("\n\n\t\t----CONFIG>TEMPO----\n"));
  Serial.print(F("\t\tESCURO: "));
  Serial.print(times[DARK]/60000);
  Serial.print(F(" min"));
  Serial.print(F("\n\t\t---------------------\n\n"));
}

void addLightTime(){
  // Tempos em millisegundo adicionados, cada aperto soma 30min
  times[LIGHT]/60000>=43200000/60000 ? NULL : times[LIGHT]+=60000;
  navigateConfigLight();
}

void subLightTime(){
  // Tempos em millisegundo, cada aperto subtrai 30min
  times[LIGHT]/60000<= 1 ? NULL : times[LIGHT]-=60000;
  navigateConfigLight();
}

void addDarkTime(){
  // Tempos em millisegundo adicionados, cada aperto soma 30min
  times[DARK]/60000>=43200000/60000 ? NULL : times[DARK]+=60000; //1800000
  navigateConfigDark();
}

void subDarkTime(){
  // Tempos em millisegundo, cada aperto subtrai 30min
  times[DARK]/60000<=1 ? NULL : times[DARK]-=60000;
  navigateConfigDark();
}


// Funções de umidade
void show_humidity_data(){
  const char ON[] PROGMEM= "ON";
  const char OFF[] PROGMEM= "OFF";
  
  
  Serial.print(F("\n\n\t\t-------UMIDADE-------\n"));
  Serial.print(F("\t\tATIVADO: "));
  Serial.print(umidade_acionada? ON : OFF);
  Serial.print(F("\n\t\tHUMIDADE LIDA: "));
  Serial.print(umidade);
  Serial.print(F("%"));
  Serial.print(F("\n\t\tESTADO DA BOMBA: "));
  Serial.print(bomba_acionada ?  ON : OFF);
  Serial.print(F("\n\t\t---------------------\n\n"));

}

void navigateUmi(){
  menu_location = HOME_HUMIDITY;
  show_humidity_data();
}

// Ativa/desativa umidade
void enable_disable_humidity(){
  umidade_acionada = !umidade_acionada;

  if(bomba_acionada){
    bomba_acionada = !bomba_acionada;
    digitalWrite(led_bomba, LOW);
  }
  if(get_value(EXA_IRRIGATION)==1){
    //lock(PIN_EXAUSTOR);
    digitalWrite(led_exaustor, LOW);
    set_value(0, EXA_STATE);
    set_value(0, EXA_IRRIGATION);
    //unlock(PIN_EXAUSTOR);
  }
  show_humidity_data();
}

void show_config_h_data(){
  
  Serial.print(F("\n\n\t\t---CONFIG>UMIDADE----\n"));
  Serial.print(F("\t\tVALOR DESEJADO: "));
  Serial.print(config_umidade);
  Serial.print(F("%"));
  Serial.print(F("\n\t\t---------------------\n\n"));
}

void navigateConfigHumidity(){
  if(!umidade_acionada){
  	menu_location = CONFIG_HUMIDITY;
  	show_config_h_data();
  }
  else{
    
  	Serial.print(F("\n\n\t\t----CONFIG>UMIDADE---\n"));
    Serial.print(F("\t\tPROIBIDO-->SIS. ACIONADO"));
    Serial.print(F("\n\t\t---------------------\n\n"));
  }
}

void addHumidity(){
  config_umidade>=100 ? NULL : config_umidade+=1;
  show_config_h_data();
}

void subHumidity(){
  config_umidade<=0 ? NULL : config_umidade-=1;
    show_config_h_data();
}

// Funções de exaustão
void show_exhauster_data(){
  const char ON[] PROGMEM= "ON";
  const char OFF[] PROGMEM= "OFF";
  
  Serial.print(F("\n\n\t\t-------EXAUSTAO------\n"));
  Serial.print(F("\t\tATIVADO: "));
  Serial.print(exaustor_acionado? ON : OFF);
  Serial.print(F("\n\t\tESTADO VENTILADOR: "));
  Serial.print(get_value(EXA_STATE) ?  ON : OFF);
  Serial.print(F("\n\t\t---------------------\n\n"));
}

void navigateExa(){
  menu_location = HOME_EXHAUSTER;
  show_exhauster_data();
}

void enable_disable_exa(){
  exaustor_acionado = !exaustor_acionado;
  if(get_value(EXA_STATE)==1){
    set_value(0, EXA_STATE);
    digitalWrite(led_exaustor, LOW);
  }
  show_exhauster_data();
}



void button_router(void *pv){
  
  uint8_t pressed_button;
  static void (*actions[4]) ();
  // unsigned long b_time;

  for(;;){
    if( xQueueReceive( button_press, &pressed_button, portMAX_DELAY) == pdPASS ){
      // b_time = micros();
      if(menu_location==HOME_LIGHT){
        actions[UP] = navigateUmi; // navegar umidade 
        actions[DOWN] = navigateExa; //navegar para exaustao
        actions[LEFT] = enable_disable_light; //ativar e desativar light 
        actions[RIGHT] = navigateConfigLight; // naegar para config luz 

        actions[pressed_button]();
      }
      else if(menu_location==CONFIG_LIGHT_TIME){
        actions[UP]  = addLightTime; // add light time
        actions[DOWN]  = subLightTime; // sub dark time
        actions[LEFT]  = navigateConfigDark; // ir pra config dark time
        if(pressed_button!=RIGHT)
          actions[pressed_button]();
      }
      else if(menu_location==CONFIG_DARK_TIME){
        actions[UP] = addDarkTime; // add dark time
        actions[DOWN] = subDarkTime; // sub dark time
        actions[LEFT] = navigateLight; // voltar para tela de luz
        
        if(pressed_button!=RIGHT)
          actions[pressed_button]();
      }
      else if(menu_location==HOME_HUMIDITY){
        actions[UP] = navigateExa; // navegar exatusao
        actions[DOWN] = navigateLight; // navegar para luz 
        actions[LEFT] = enable_disable_humidity;// habiiltear e desabilitar 
        actions[RIGHT] = navigateConfigHumidity; // ir para configurar umi OK
        actions[pressed_button]();
      }
      else if(menu_location==CONFIG_HUMIDITY){
        actions[UP] = addHumidity; // navegar exatusao
        actions[DOWN] = subHumidity; // navegar para luz 
        actions[LEFT] = navigateUmi;// habiiltear e desabilitar 
        if(pressed_button!=RIGHT)
          actions[pressed_button]();
      }
      else if(menu_location==HOME_EXHAUSTER){
        actions[UP] = navigateLight; // add umidade OK
        actions[DOWN] = navigateUmi; // sub umidade OK
        actions[LEFT] = enable_disable_exa; // habiiltear e desabilitar OK
        
        if(pressed_button!=RIGHT)
          actions[pressed_button]();
      }
      // b_time = micros() - b_time;
      // imprimir(F("Resposta Menu (us)"), b_time);
  }

}
}
//função para ser chamada no setup do arduino
void buttons_setup(){
    configureCommon();
    attachInterrupt(digitalPinToInterrupt(INTERRUPT), button_interrupt, FALLING);

    xTaskCreate(
        button_router, // Task function
        NULL, // Task name for humans
        128, 
        NULL, // Task parameter
        2, // Task priority
        NULL
    );

    show_light_data();
}

#endif
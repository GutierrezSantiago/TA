#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <splash.h>
#include "config.h"


// potenciometro
const int potePin = PIN_ADC; 
volatile int estadoPote;

const int ledPin1 = PIN_LED_R; //rojo
const int relayPin = 5; //relay con led verde

// led azul y vars para logica boton
const int botonPin = PIN_ENC_PUSH;


int estadoLed = LOW; // Estado del LED 
int estadoBoton; // Estado del pulsador
int ultimoEstado = HIGH; // Estado anterior del pulsador   
unsigned long ultimoRebote = 0; // Último tiempo de rebote del pulsador
int delayRebote = 50; // Tiempo de rebote del pulsador

//ESTADISTICOS
volatile int counter = 0;
volatile float porcPote = 0;

//DISPLAY

//Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void displayInit(){
  display.begin(0x3C, true); // SSD1306
  //display.begin(0x3C, true); // Address 0x3C default SH110X
  display.clearDisplay();
  display.setTextSize(1);
  //display.setTextColor(SH110X_WHITE);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0,0);

  display.println(F("Hola Mundo!"));
  display.println(F("Datos de Compilacion:"));
  display.printf("Fecha %s\n",__DATE__);
  display.printf("Hora: %s\n",__TIME__);

  display.display();
}

void setup() {
  Serial.begin(BAUDRATE);
  delay(1000);
  
  // potenciometro, led y relay
  pinMode(potePin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(relayPin, OUTPUT);
  analogReadResolution(10);
  analogWriteResolution(10);

  //led azul
  pinMode(botonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  
  // display
  delay(1000);
  Serial.println(F("Iniciando Display..."));
  displayInit();  
  Serial.println(F("Display Iniciado"));
  delay(5000);

}

void loop() {
  // valor potenciometro y cambio intensidad led rojo
  estadoPote = analogRead(potePin);
  analogWrite(ledPin1, estadoPote);

  // led verde 
  if (estadoPote>513){
    digitalWrite(relayPin, HIGH);
  } else {
    digitalWrite(relayPin, LOW);
  }

  //LOGICA LED AZUL Y BOTON
  int lectura = digitalRead(botonPin);

  // Si el estado del pulsador ha cambiado, registrar el tiempo de rebote
  if (lectura != ultimoEstado) {
    ultimoRebote = millis();
  }

  // Si ha pasado suficiente tiempo desde el último rebote, actualizar el estado del pulsador
  if ((millis() - ultimoRebote) > delayRebote) {
    // Si el estado del pulsador ha cambiado, actualizar el estado del LED
    if (lectura != estadoBoton) {
      estadoBoton = lectura;

      // Si el pulsador ha sido presionado, cambiar el estado del LED
      if (estadoBoton == LOW) {
        estadoLed = !estadoLed;
        counter = counter + 1;
        Serial.println("Boton presionado");
        digitalWrite(LED_BUILTIN, estadoLed);
      }
    }
  }
  // Guardar el último estado del pulsador
  ultimoEstado = lectura;

  porcPote = ((float)estadoPote/1024)*100;

  // display
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Pulsador: %d acciones\n",counter);
  display.printf("Led A: ");
  if(estadoLed){
    display.printf("Encendido\n");
  }else{
    display.printf("Apagado\n");
  }
  display.printf("Led R: %d", (int)porcPote);
  display.print("% de potencia\n");
  display.printf("Led V: ");
  if(estadoPote>512){
    display.printf("Encendido\n");
  }else{
    display.printf("Apagado\n");
  }
  display.display();
  delay(20); // this speeds up the simulation
}
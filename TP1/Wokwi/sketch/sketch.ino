// potenciometro
const int potePin = 4; 
volatile int estadoPote;

const int ledPin1 = 25; //rojo
const int relayPin = 2; //relay con led verde

// led azul y vars para logica boton
const int botonPin = 12;
const int ledPin2 = 5; // azul

volatile int estadoBotonAnterior;
volatile int estadoBotonActual;
volatile int estadoLed = LOW;

//ESTADISTICOS
volatile int counter = 0;
volatile float porcPote = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  Serial.begin(115200);
  pinMode(potePin, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(botonPin, INPUT);
  pinMode(ledPin2, OUTPUT);
  estadoBotonActual = digitalRead(botonPin);
  digitalWrite(ledPin2, estadoLed);
  analogReadResolution(10);
  analogWriteResolution(10);

}

void loop() {
  // valor potenciometro y cambio intensidad led rojo
  estadoPote = analogRead(potePin);
  analogWrite(ledPin1, estadoPote);

  // led verde 
  if (estadoPote>512){
    digitalWrite(2, HIGH);
    Serial.println("Encendido V"); // esto claramente se puede mejorar con una variable que guarde y se imprima todo abajo
  } else {
    digitalWrite(2, LOW);
    Serial.println("Apagado V");
  }

  //LOGICA LED AZUL Y BOTON (LO QUE NO ESTOY SEGURO ES SI HAY QUE USAR INTERRUPCIONES PARA ESTO)
  // mentira estoy casi seguro que si son interrupciones, voy a esperar que el profe suba lo suyo
  estadoBotonAnterior = estadoBotonActual;
  estadoBotonActual = digitalRead(botonPin);
  delay(10);
  if(estadoBotonActual == LOW && estadoBotonAnterior == HIGH){
    estadoLed = !estadoLed;
    counter = counter + 1;
  }
  digitalWrite(ledPin2, estadoLed);
  

  Serial.println(estadoPote);
  //IMPRIMO ESTADISTICOS (se soluciona lo de led verde arriba, podria mejorarse)
  porcPote = ((float)estadoPote/1024)*100;
  
  if(estadoLed){
    Serial.println("Apagado A");
  }else{
    Serial.println("Encendido A");
  }
  Serial.println(porcPote);
  Serial.println(counter);
  Serial.println("-------------------------");//divisor para pruebas
  delay(10); // this speeds up the simulation
}

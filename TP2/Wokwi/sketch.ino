// Dependencias
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Configuración de pines y dispositivos
// DISPLAY
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

// DHT 22
#define DHTTYPE DHT22
#define DHTPIN 33
DHT dht(DHTPIN, DHTTYPE);

// POTENCIOMETRO, LED
const int PIN_ADC = 32;
volatile int estadoPote;
const int PIN_LED_V = 23;
volatile int potenciaLed;

// CREDENCIALES WIFI Y TELEGRAM BOT
//const char *ssid = "";
//const char *password = "";

const String botToken = "";
const unsigned long botMTBS = 1000; //mean time between scan messages (Tiempo medio entre scans)
unsigned long botLastScan;
volatile int banderaModoMensaje; // Bandera para saber si se esta en modo mensaje para el display

WiFiClientSecure secured_client;
UniversalTelegramBot bot(botToken, secured_client);

void setup() {
  // PINES
  pinMode(PIN_ADC, INPUT);
  analogReadResolution(10);
  analogWriteResolution(10);
  pinMode(PIN_LED_V, OUTPUT);
  potenciaLed = 0;
  pinMode(LED_BUILTIN, OUTPUT);

  // DISPLAY
  display.begin(0x3c, true); //Dirección por defecto: 0x3C
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  banderaModoMensaje = 0;

  // WIFI
  dht.begin();
  Serial.begin(115200);
  Serial.print("Connecting to WiFi");
  WiFi.begin("Wokwi-GUEST", "", 6);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED) {
  delay(100);
  Serial.print(".");
  }
  Serial.println(" Connected!");
  }

// Funciones de ayuda
String floatToString( float n, int l, int d, boolean z)
{
  char c[l+1];
  String s;
  dtostrf(n,l,d,c);
  s=String(c);
  if(z){
    s.replace(" ","0");
  }
  return s;
}

void handleNewMessages(int numNewMessages)
{
Serial.print ("handleNewMessages ");
Serial.println(numNewMessages);
for (int i = 0; i < numNewMessages; i++)
{
String chat_id = bot.messages[i].chat_id;
String text = bot.messages[i].text;
String from_name = bot.messages[i].from_name;
if (from_name == "")
from_name = "Guest";
if (banderaModoMensaje == 0)
{
  if (text == "/humedad")
  {
    float h = dht.readHumidity();
    String hstr = floatToString(h,2,2,false);
    String mensaje = "Humedad: " + hstr + "%.";
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/temperatura")
  {
    float t = dht.readTemperature();
    String tstr = floatToString(t,2,2,false);
    String mensaje = "Temperatura: " + tstr + " grados.";
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/dht") 
  {
    float t = dht.readTemperature();
    String tstr = floatToString(t,2,2,false);
    float h = dht.readHumidity();
    String hstr = floatToString(h,2,2,false);
    String mensaje = "Humedad: " + hstr + "%.\nTemperatura: " + tstr + " grados.";
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/potenciometro") 
  {
    estadoPote = analogRead(PIN_ADC);
    String estadoPoteString = floatToString(estadoPote, 0, 0, false);
    String mensaje = "El potenciómetro presenta el siguiente valor: " + estadoPoteString;
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/ledAzulOn") 
  {
    digitalWrite(LED_BUILTIN, HIGH);
    bot.sendMessage(chat_id, "Led integrado encendido.");
  }
  if (text == "/ledAzulOff") 
  {
    digitalWrite(LED_BUILTIN, LOW);
    bot.sendMessage(chat_id, "Led integrado apagado.");
  }
  if (text == "/ledVerdeOn") 
  {
    potenciaLed = 1024;
    analogWrite(PIN_LED_V, potenciaLed);
    bot.sendMessage(chat_id, "Led verde encendido.");
  }
  if (text == "/ledVerdeOff") 
  {
    potenciaLed = 0;
    analogWrite(PIN_LED_V, potenciaLed);
    bot.sendMessage(chat_id, "Led verde apagado.");
  }
  if (text == "/ledVerdeMas") 
  {
    Serial.println(potenciaLed);
    potenciaLed = potenciaLed + 256;
    Serial.println(potenciaLed);
    if (potenciaLed > 1023) 
    {
      potenciaLed = 1024;
    }
    Serial.println(potenciaLed);
    analogWrite(PIN_LED_V, potenciaLed);
    String mensaje = "Potencia del led verde incrementada en un 25%.";
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/ledVerdeMenos") 
  {
    Serial.println(potenciaLed);
    potenciaLed = potenciaLed - 256;
    Serial.println(potenciaLed);
    if (potenciaLed < 0) 
    {
      potenciaLed = 0;
    }
    Serial.println(potenciaLed);
    analogWrite(PIN_LED_V, potenciaLed);
    String mensaje = "Potencia del led verde decrementada en un 25%.";
    
    bot.sendMessage(chat_id, mensaje);
  }
  if (text == "/mensaje") 
  {
    banderaModoMensaje = 1;
    bot.sendMessage(chat_id, "Escriba el mensaje a mostrar por el display.\nSi desea salir del modo mensaje sin enviar nada o limpiar el display el comando es:\n/salir\n\nLuego de enviar el próximo mensaje se saldra del modo mensaje automaticamente.");
  }


  if (text == "/start")
  {
  String welcome = "Bienvenido al Bot de Telegram del Grupo 9 de TA 4K2.\n" ;//+ from_name + "

  welcome += "Los comandos disponibles son los siguientes:\n\n";
  welcome += "/temperatura\n";
  welcome += "/humedad\n";
  welcome += "/dht\n";
  welcome += "/potenciometro\n";
  welcome += "/ledAzulOn\n";
  welcome += "/ledAzulOff\n";
  welcome += "/ledVerdeOn\n";
  welcome += "/ledVerdeOff\n";
  welcome += "/ledVerdeMas\n"; // Aumenta 25% potencia
  welcome += "/ledVerdeMenos\n"; // Disminuye 25% potencia
  welcome += "/mensaje\n";
  //welcome += "/help\n"; -> Hacemos un help para explicar los comandos?
  bot.sendMessage(chat_id, welcome, "Markdown");
  }
} else 
{
  display.clearDisplay();
  display.setCursor(0,0);
  if(text != "/salir")
  {
    display.print(text);
  }
  display.display();

  banderaModoMensaje = 0;
}

}
}

void loop() {
  if (millis() - botLastScan > botMTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      Serial.println("Respuesta obtenida");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    botLastScan = millis();
  }
}

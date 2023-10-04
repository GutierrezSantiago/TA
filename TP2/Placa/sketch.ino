// Dependencias
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"

// DISPLAY
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, -1);

// DHT 22
#define DHTTYPE DHT22
#define DHTPIN 33
DHT dht(DHTPIN, DHTTYPE);

// Bot Telegram
WiFiClientSecure secured_client;
UniversalTelegramBot bot(TOKEN, secured_client);

// LED
volatile int ledPower = 0;

// Potenciometro
volatile int potentiometerValue = 0;

// Bot Telegram
unsigned long botLastScan;
volatile bool isMessageMode = false;

void setup() {
  // PIN
  pinMode(POT_PIN, INPUT);
  analogReadResolution(10);
  analogWriteResolution(10);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // DISPLAY
  display.begin(0x3c, true);  //Dirección por defecto: 0x3C
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  // Serial Monitor
  Serial.begin(115200);

  // DHT
  dht.begin();

  // Telegram Bot
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);

  // Connect to Wi-Fi
  // WiFi.begin(SSID, PASS);
  WiFi.begin("Wokwi-GUEST", "", 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connection successful!");
}

void loop() {
  if (millis() - botLastScan > MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    botLastScan = millis();
  }
}

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chatId = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    if (isMessageMode) {
      display.clearDisplay();
      display.setCursor(0, 0);
      if (text != CMD_EXIT) display.print(text);
      display.display();

      String message = "Mensaje mostrado en el display: " + text;
      bot.sendMessage(chatId, message);

      isMessageMode = false;
      continue;
    }

    if (text == CMD_READ_HUMIDITY) {
      float h = dht.readHumidity();
      String hstr = floatToString(h, 2, 2, false);
      String message = "Humedad: " + hstr + "%.";
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_READ_TEMPERATURE) {
      float t = dht.readTemperature();
      String tstr = floatToString(t, 2, 2, false);
      String message = "Temperatura: " + tstr + "°C.";
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_READ_DHT) {
      float t = dht.readTemperature();
      String tstr = floatToString(t, 2, 2, false);
      float h = dht.readHumidity();
      String hstr = floatToString(h, 2, 2, false);
      String message = "Humedad: " + hstr + "%.\nTemperatura: " + tstr + "°C.";
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_READ_POTENTIOMETER) {
      potentiometerValue = analogRead(POT_PIN);
      String potentiometerValueString = floatToString(potentiometerValue, 0, 0, false);
      String message = "Valor del potenciómentro: " + potentiometerValueString;
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_TURN_ON_BLUE_LED) {
      digitalWrite(LED_BUILTIN, HIGH);
      bot.sendMessage(chatId, "LED azul encendido.");
      continue;
    }

    if (text == CMD_TURN_OFF_BLUE_LED) {
      digitalWrite(LED_BUILTIN, LOW);
      bot.sendMessage(chatId, "LED azul apagado.");
      continue;
    }

    if (text == CMD_TURN_ON_GREEN_LED) {
      ledPower = 1024;
      analogWrite(GREEN_LED_PIN, ledPower);
      bot.sendMessage(chatId, "LED verde encendido.");
      continue;
    }

    if (text == CMD_TURN_OFF_GREEN_LED) {
      ledPower = 0;
      analogWrite(GREEN_LED_PIN, ledPower);
      bot.sendMessage(chatId, "LED verde apagado.");
      continue;
    }

    if (text == CMD_INCREASE_POWER_GREEN_LED) {
      ledPower = ledPower + 256;

      if (ledPower > 1023) {
        ledPower = 1023;
      }

      analogWrite(GREEN_LED_PIN, ledPower);

      String message = "Potencia del LED verde incrementada en un 25%.";
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_DECREASE_POWER_GREEN_LED) {
      ledPower = ledPower - 256;

      if (ledPower < 0) {
        ledPower = 0;
      }

      analogWrite(GREEN_LED_PIN, ledPower);

      String message = "Potencia del LED verde decrementada en un 25%.";
      bot.sendMessage(chatId, message);
      continue;
    }

    if (text == CMD_PRINT_MESSAGE) {
      isMessageMode = true;
      bot.sendMessage(chatId, "Escriba el mensaje a mostrar en el display (\/exit para salir del modo mensaje y limpiar el display).");
      continue;
    }
  }
}

String floatToString(float n, int l, int d, boolean z) {
  char c[l + 1];
  String s;
  dtostrf(n, l, d, c);
  s = String(c);

  if (z) {
    s.replace(" ", "0");
  }

  return s;
}
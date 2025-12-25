#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "secrets.h"  // berisi WIFI_SSID, WIFI_PASSWORD, BOT_TOKEN, CHAT_ID (tidak di-push ke Git)
const int ledPins[] = {13, 12, 14};
const int buttonPin = 15;
const int ledCount = sizeof(ledPins) / sizeof(ledPins[0]);
bool lastButtonState = HIGH; // awalnya tidak ditekan (INPUT_PULLUP)
const int ledPin = LED_BUILTIN;
int ledStatus = 0;
const unsigned long BOT_MTBS = 1000; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime; // last time messages' scan has been done



void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++)
  {
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
        if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "/LampuMerahMenyala")
    {
      digitalWrite(ledPins[0], HIGH); // Menyalakan LED (HIGH adalah level tegangan)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Lampu Merah Menyala", "");
    }

    if (text == "/LampuMerahMati")
    {
      ledStatus = 0;
      digitalWrite(ledPins[0], LOW); // Mematikan LED (LOW adalah level tegangan)
      bot.sendMessage(chat_id, "Lampu Merah Mati", "");
    }

    if (text == "/LampuKuningMenyala")
    {
      digitalWrite(ledPins[1], HIGH); // Menyalakan LED (HIGH adalah level tegangan)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Lampu Kuning Menyala", "");
    }

    if (text == "/LampuKuningMati")
    {
      ledStatus = 0;
      digitalWrite(ledPins[1], LOW); // Mematikan LED (LOW adalah level tegangan)
      bot.sendMessage(chat_id, "Lampu Kuning Mati", "");
    }

        if (text == "/LampuHijauMenyala")
    {
      digitalWrite(ledPins[2], HIGH); // Menyalakan LED (HIGH adalah level tegangan)
      ledStatus = 1;
      bot.sendMessage(chat_id, "Lampu Hijau Menyala", "");
    }

    if (text == "/LampuHijauMati")
    {
      ledStatus = 0;
      digitalWrite(ledPins[2], LOW); // Mematikan LED (LOW adalah level tegangan)
      bot.sendMessage(chat_id, "Lampu Hijau Mati", "");
    }


    if (text == "/status")
    {
      // Baca status ketiga lampu
      bool merah  = digitalRead(ledPins[0]) == HIGH;
      bool kuning = digitalRead(ledPins[1]) == HIGH;
      bool hijau  = digitalRead(ledPins[2]) == HIGH;

      if (merah && kuning && hijau)
      {
        bot.sendMessage(chat_id, "Lampu Hijau, Kuning, dan Merah Menyala", "");
      }
      else if (merah && kuning && !hijau)
      {
        bot.sendMessage(chat_id, "Lampu Kuning dan Merah Menyala", "");
      }
      else if (!merah && kuning && hijau)
      {
        bot.sendMessage(chat_id, "Lampu Hijau dan Kuning Menyala", "");
      }
      else if (merah && !kuning && !hijau)
      {
        bot.sendMessage(chat_id, "Lampu Merah Menyala", "");
      }
      else if (!merah && kuning && !hijau)
      {
        bot.sendMessage(chat_id, "Lampu Kuning Menyala", "");
      }
      else if (!merah && !kuning && hijau)
      {
        bot.sendMessage(chat_id, "Lampu Hijau Menyala", "");
      }
      else
      {
        bot.sendMessage(chat_id, "Semua Lampu Mati", "");
      }
    }

    if (text == "/start")
    {
      String welcome = "Selamat Datang Di Bot Telegram Menyalakan Lampu Automatis, " + from_name + ".\n";
      welcome += "Ini adalah contoh Bot LED Berkedip.\n\n";
      welcome += "/LampuMerahMenyala : untuk menyalakan LED\n";
      welcome += "/LampuMerahMati : untuk mematikan LED\n";
      welcome += "/LampuKuningMenyala : untuk menyalakan LED\n";
      welcome += "/LampuKuningMati : untuk mematikan LED\n";
      welcome += "/LampuHijauMenyala : untuk menyalakan LED\n";
      welcome += "/LampuHijauMati : untuk mematikan LED\n";
      welcome += "/status : Mengembalikan status LED saat ini\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}



void setup() {

Serial.begin(115200);
Serial.println("Setup dimulai...");
  for (int i = 0; i < ledCount; i++) {

    pinMode(ledPins[i], OUTPUT);
  }
  pinMode(buttonPin, INPUT_PULLUP);

  // attempt to connect to Wifi network:
  Serial.print(WIFI_SSID);
  Serial.println("");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}



void loop() {
    if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <DHT.h>

#include "Settings.h"
#include "Relay.h"
#include "Switch.h"
#include "RGB.h"
#include "TempHum.h"
#include "WebServer.h"

/* ПИНЫ */
#define PIN_SWITCH1 12  // D6
#define PIN_SWITCH2 14  // D5
#define PIN_SWITCH3 13  // D7

#define PIN_RELAY1  15  // D8
#define PIN_RELAY2  16  // D0

#define PIN_RGB_R   0   // D3
#define PIN_RGB_G   4   // D2
#define PIN_RGB_B   5   // D1

#define PIN_DHT     2   // D4
/* \ПИНЫ */

/* Константы */
#define DHTTYPE DHT22           // датчик DHT 22, если DHT11 - поменять
#define INTERVAL_GET_DATA 2000  // интервала измерений, мс

#define INTERVAL_SET_RGB 10     // интервала измерений, мс
/* \Константы */

unsigned long dhtTime;    // предыдущее время измерения
DHT dht(PIN_DHT, DHTTYPE);
TempHum tempHum;      // сюда будем вести запись температуры и влажности

_Relay Relay1(PIN_RELAY1);
_Relay Relay2(PIN_RELAY2);

unsigned long rgbTime;    // предыдущее время измерения
_RGB RGBstrip(PIN_RGB_R, PIN_RGB_G, PIN_RGB_B);

Settings settings;  // Создание настроек
AsyncWebServer server(80);

void setup() {
  delay(3000);

  analogWriteFreq(500);
  analogWriteRange(255);

  dht.begin();          // запуск DHT

  Serial.begin(115200);
  Serial.println();

  _Switch Switch1(PIN_SWITCH1, SwitchFuncLight1);   // param1 - pin, param2 - функция прерывания
  _Switch Switch2(PIN_SWITCH2, SwitchFuncLight2);
  _Switch Switch3(PIN_SWITCH3, SwitchFuncRGB);

  settings.Init();    // Чтение настроек

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(settings.WIFI_STA_SSID, settings.WIFI_STA_PASS);

  unsigned long connection_timer = millis();

  Serial.println("Подключение к точки доступа WiFi...");
  while ((WiFi.status() != WL_CONNECTED) && (millis() - connection_timer < 10000)) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Соединение с точкой доступа WiFi установлено!\n\tIP: ");
    Serial.println(WiFi.localIP());
    settings.WIFI_STA_IP = WiFi.localIP().toString();
    settings.WIFI_STA_STATUS = true;
  } else {
    Serial.println("Ошибка! Соединение с точкой доступа WiFi не установлено.");
    settings.WIFI_STA_STATUS = false;

    // Удаляем предыдущие конфигурации WIFI сети
    WiFi.disconnect(); // обрываем WIFI соединения
    WiFi.mode(WIFI_OFF); // отключаем WIFI
    delay(500);

    // присваиваем статичесий IP адрес
    WiFi.mode(WIFI_AP); // режим клиента
  }

  Serial.println("Настройка точки доступа...");

  IPAddress local_IP(192,168,4,1);
  IPAddress gateway(192,168,4,1);
  IPAddress subnet(255,255,255,0);

  Serial.print("Установка настроек точки доступа ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Готово" : "Ошибка!");

  Serial.print("Установка точки доступа ... ");
  Serial.println(WiFi.softAP(settings.WIFI_AP_SSID, settings.WIFI_AP_PASS) ? "Готово" : "Ошибка!");

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("IP адрес точки доступа: ");
  Serial.println(myIP);
  settings.WIFI_AP_IP = myIP.toString();

  //Serial.print("MAC: ");
  //Serial.println(WiFi.macAddress());

  settings.MAC_ADDRESS = getMacAddress();
  Serial.print("MAC: ");
  Serial.println(settings.MAC_ADDRESS);

  if (!MDNS.begin(settings.WIFI_STA_MDNS, WiFi.localIP())) {
    Serial.println("Ошибка! Служба mDNS не запущена.");
    settings.MDNS_STATUS = false;
  } else {
    Serial.println("Служба mDNS успешно запущена.");
    settings.MDNS_STATUS = true;
  }

  initServer();



  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";

      SPIFFS.end();
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");

    if (ArduinoOTA.getCommand() == U_FS) {
      if(!SPIFFS.begin()){
        Serial.println("Ошибка открытия SPIFFS!");
        return;
      }
    }
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Служба OTA запущена.");

  server.begin();
  settings.HTTP_STATUS = true;
  Serial.println("HTTP сервер запущен.");

  MDNS.addService("http", "tcp", 80);

  dhtTime = millis();
  rgbTime = dhtTime;
}

void loop() {
  MDNS.update();

  ArduinoOTA.handle();

  if (millis() - dhtTime > INTERVAL_GET_DATA) {
    tempHum.temp = dht.readTemperature();
    tempHum.hum  = dht.readHumidity();
    dhtTime = millis();
  }

  if (millis() - rgbTime > INTERVAL_SET_RGB) {
    byte tempR = RGBstrip.Rstate;
    byte tempG = RGBstrip.Gstate;
    byte tempB = RGBstrip.Bstate;

    if (!RGBstrip.getState()) {
      tempR = 0;
      tempG = 0;
      tempB = 0;
    }

    /*  КОД ALEXGYVER --- пока не работает
    int deltaR = tempR - RGBstrip.nowR;
  	int deltaG = tempG - RGBstrip.nowG;
  	int deltaB = tempB - RGBstrip.nowB;

  	// ищем наибольшее изменение по модулю
  	int deltaMax = 0;
  	if (abs(deltaR) > deltaMax) deltaMax = abs(deltaR);
  	if (abs(deltaG) > deltaMax) deltaMax = abs(deltaG);
  	if (abs(deltaB) > deltaMax) deltaMax = abs(deltaB);

  	// Защита от деления на 0. Завершаем работу
  	if (deltaMax != 0) {
      // Шаг изменения цвета
    	float stepR = (float)deltaR / deltaMax;
    	float stepG = (float)deltaG / deltaMax;
    	float stepB = (float)deltaB / deltaMax;

    	// Дробные величины для плавности, начальное значение = текущему у светодиода
    	float thisR = RGBstrip.nowR, thisG = RGBstrip.nowG, thisB = RGBstrip.nowB;

      if (deltaMax != 0) {
    	   // Основной цикл изменения яркости
        thisR += stepR;
        thisG += stepG;
        thisB += stepB;

        RGBstrip.nowR = thisR;
        RGBstrip.nowG = thisG;
        RGBstrip.nowB = thisB;

        analogWrite(PIN_RGB_R, RGBstrip.nowR);
        analogWrite(PIN_RGB_G, RGBstrip.nowG);
        analogWrite(PIN_RGB_B, RGBstrip.nowB);
      }
    }
    */


    if (RGBstrip.nowR != tempR) {
      if (tempR > RGBstrip.nowR) {
        RGBstrip.nowR++;
      } else {
        RGBstrip.nowR--;
      }

      analogWrite(PIN_RGB_R, RGBstrip.nowR);
    }

    if (RGBstrip.nowG != tempG) {
      if (tempG > RGBstrip.nowG) {
        RGBstrip.nowG++;
      } else {
        RGBstrip.nowG--;
      }

      analogWrite(PIN_RGB_G, RGBstrip.nowG);
    }

    if (RGBstrip.nowB != tempB) {
      if (tempB > RGBstrip.nowB) {
        RGBstrip.nowB++;
      } else {
        RGBstrip.nowB--;
      }

      analogWrite(PIN_RGB_B, RGBstrip.nowB);
    }

    rgbTime = millis();
  }
}

ICACHE_RAM_ATTR void SwitchFuncLight1() {
  Relay1.changeState();
}

ICACHE_RAM_ATTR void SwitchFuncLight2() {
  Relay2.changeState();
}

ICACHE_RAM_ATTR void SwitchFuncRGB() {
  RGBstrip.TurnOnOff(PIN_SWITCH3);
}

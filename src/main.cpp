#include <Arduino.h>
#include "PCF8574.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include "timerMinim.h"
#include <ArduinoOTA.h>
#include <Tasks\TimingTask.h>


#define I2C_RELAYS_ADR 0x24



//SDA = 4;
//SCL = 5;
PCF8574 pcf8574( I2C_RELAYS_ADR, 4, 5);

/*
  Задачи, выполняемые по времени
*/
//Полив в 08:00
TimingTask Task0 = TimingTask(480, 1);

//Полив в 20:00
TimingTask Task1 = TimingTask(1160, 1);


const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 9000;
const int   daylightOffset_sec = 9000;

/*
  Вывод текущего времени в Serial
*/
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

/*
  Получение времени
*/
uint GetCurrentMinutes()
{
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  return timeinfo.tm_hour * 60 + timeinfo.tm_min;
}

/*
  Подключение к домашнему Wifi
*/
void WIFI_STA_INIT()
{
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_STA);
    delay(100);
    WiFi.begin("HUAWEI-3", "77741124404");

    //Конфигурация параметров сети модуля
    //WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println(WiFi.localIP());
}

/*
  Инициализация выходных реле
*/
void PCF8574_Init()
{
  pcf8574.begin();

  pcf8574.pinMode(P0, OUTPUT);
  pcf8574.digitalWrite(P0, HIGH);

  pcf8574.pinMode(P1, OUTPUT);
  pcf8574.digitalWrite(P1, HIGH);

  pcf8574.pinMode(P2, OUTPUT);
  pcf8574.digitalWrite(P2, HIGH);

  pcf8574.pinMode(P3, OUTPUT);
  pcf8574.digitalWrite(P3, HIGH);

  pcf8574.pinMode(P4, OUTPUT);
  pcf8574.digitalWrite(P4, HIGH);

  pcf8574.pinMode(P5, OUTPUT);
  pcf8574.digitalWrite(P5, HIGH);

  pcf8574.pinMode(P6, OUTPUT);
  pcf8574.digitalWrite(P6, HIGH);

  pcf8574.pinMode(P7, OUTPUT);
  pcf8574.digitalWrite(P7, HIGH);
}

/*
  Инициализация прошивки по воздуху
*/
void OTA_Init()
{
  ArduinoOTA.begin();

  ArduinoOTA.onStart([]()
  {
    //LCD.clear();
    //LCD.setCursor(0, 0);
    //LCD.print("UPLOAD FIRMWARE ");
    //LCD.backlight();
    //LCD.setCursor(0,0);
    //LCD.print("Start");  //  "Начало OTA-апдейта"
  });


  ArduinoOTA.onEnd([]()
  {
    //LCD.clear();
    //LCD.setCursor(0, 0);
    //LCD.print("UPLOAD SUCCES ");
    //LCD.setCursor(0, 1);
    //LCD.print("SUCCESSFULLY");
    //delay(2000);
    //LCD.print("\nEnd");  //  "Завершение OTA-апдейта"
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    //LCD.setCursor(0, 1);
    //LCD.printf("PROGRESS: %u%%", (progress / (total / 100)));
  });
}

/*
  Инициализация 
*/
void setup() 
{
  Serial.begin(115200);
  WIFI_STA_INIT();
  OTA_Init();

  PCF8574_Init();
  Serial.println(WiFi.localIP());

  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // UDP-клиент на указанном порту
  //udp.begin(udp_port);
}

bool Relay0 = false;

/*
  Бесконечный цикл
*/
void loop() 
{
    bool wifi_connected = (WiFi.status() == WL_CONNECTED);
    
    //Проверяем обновление по воздуху
    //если есть раздача сети или контроллер подключен к сети
    if(wifi_connected == true)
    {
        //Проверка обновления прошивки по воздуху
        ArduinoOTA.handle();
    }

    uint currenttime = GetCurrentMinutes();

    //Relay0 =  Task0.Update(currenttime);
    Relay0 |= Task1.Update(currenttime);

    //Переносим переменнные в регистр
    pcf8574.digitalWrite(P7, !Relay0);
    printLocalTime();
    delay(500);
}

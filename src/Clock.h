#ifndef _Clock_h
#define _Clock_h

#define udp_port 2390

#define DEFAULT_NTP_SERVER "ru.pool.ntp.org" // NTP сервер по умолчанию "time.nist.gov"

#define UDP_PACKET_MAX_SIZE 1024

uint16_t SYNC_TIME_PERIOD = 60;          // Период синхронизации в минутах

timerMinim ntpSyncTimer(1000 * 60 * SYNC_TIME_PERIOD);  // Таймер синхронизации времени с NTP-сервером

// ---- Синхронизация часов с сервером NTP
bool wifi_connected = false;
IPAddress timeServerIP;
#define NTP_PACKET_SIZE 48               // NTP время в первых 48 байтах сообщения
byte packetBuffer[NTP_PACKET_SIZE];      // буффер для хранения входящих и исходящих пакетов

int8_t timeZoneOffset = 5;               // смещение часового пояса от UTC
long ntp_t = 0;                          // Время, прошедшее с запроса данных с NTP-сервера (таймаут)
byte ntp_cnt = 0;                        // Счетчик попыток получить данные от сервера
bool init_time = false;                  // Флаг false - время не инициализировано; true - время инициализировано
bool refresh_time = true;                // Флаг true - пришло время выполнить синхронизацию часов с сервером NTP
bool useNtp = false;                     // Использовать синхронизацию времени с NTP-сервером
char ntpServerName[31] = "";             // Используемый сервер NTP

char incomeBuffer[UDP_PACKET_MAX_SIZE];        // Буфер для приема строки команды из wifi udp сокета. Также буфер используется для отправки строк в смартфон



// ---- Подключение к сети
WiFiUDP udp;





String clockCurrentText() 
{
  
  int hrs = hour();
  int mins = minute();

  String sHrs = "0" + String(hrs);  
  String sMin = "0" + String(mins);
  if (sHrs.length() > 2) sHrs = sHrs.substring(1);
  if (sMin.length() > 2) sMin = sMin.substring(1);
  return sHrs + ":" + sMin;
}

/*
  Отправка пакета на ip адрес
*/
void sendNTPpacket(IPAddress& address) 
{
  Serial.print(F("Отправка NTP пакета на сервер "));
  Serial.println(ntpServerName);
  // set all bytes in the buffer to 0
  // memset(packetBuffer, 0, NTP_PACKET_SIZE);
  for (byte i=0; i<NTP_PACKET_SIZE; i++) packetBuffer[i] = 0;
  
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); // NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

void getNTP() 
{
  if (!wifi_connected)
  {
    return;
  } 

  WiFi.hostByName(ntpServerName, timeServerIP);
  IPAddress ip;
  ip.fromString(F("0.0.0.0"));

  if (timeServerIP==ip) 
  {
    timeServerIP.fromString(F("192.36.143.130"));  // Один из ru.pool.ntp.org // 195.3.254.2
  }

  //printNtpServerName();
  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  ntp_t = millis();  
}

void parseNTP() 
{
    Serial.println(F("Разбор пакета NTP"));
    ntp_t = 0; 
    ntp_cnt = 0; 
    init_time = true; 
    refresh_time = false;
    unsigned long highWord = word(incomeBuffer[40], incomeBuffer[41]);
    unsigned long lowWord = word(incomeBuffer[42], incomeBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    unsigned long seventyYears = 2208988800UL ;
    time_t t = secsSince1900 - seventyYears + (timeZoneOffset) * 3600;
    Serial.print(F("Секунд с 1970: "));
    Serial.println(t);
    setTime(t);
}

void update(bool wifi_connected)
{
  if (wifi_connected) 
    {
      if (ntp_t > 0 && millis() - ntp_t > 5000) 
      {
        Serial.println(F("Таймаут NTP запроса!"));
        ntp_t = 0;
        ntp_cnt++;

        if (init_time && ntp_cnt >= 10) 
        {
          Serial.println(F("Не удалось установить соединение с NTP сервером."));  
          refresh_time = false;
        }
      }

      bool timeToSync = ntpSyncTimer.isReady();

      if (timeToSync) 
      { 
        ntp_cnt = 0; 
        refresh_time = true; 
      }

      if (timeToSync || (refresh_time && ntp_t == 0 && (ntp_cnt < 10 || !init_time))) 
      {
        getNTP();

        if (ntp_cnt >= 10) 
        {
          if (init_time) 
          {
            udp.flush();
          } 
          else 
          {
            //ESP.restart();
            ntp_cnt = 0;
            WIFI_STA_INIT();
          }
        }        
      }
    }

}

#endif
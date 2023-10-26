#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266mDNS.h>
#else
    #include <WiFi.h>
    #include <ESPmDNS.h>
#endif

//#include <WiFiUdp.h>

//Клиент и сервер
WiFiServer wifiServer = WiFiServer(5000);

WiFiClient client;

//WiFiUDP udp;

uint8_t ByteArray[16];
uint8_t RECIVED_BYTES_LENGTCH;



void StartWifiServer()
{
    wifiServer.begin();

    //udp.begin(6003);
}

uint8_t ReadWifiServer()
{
    
    client = wifiServer.available();

    if(client.available() > 0)
    {
        RECIVED_BYTES_LENGTCH = 0;

        while(client.available() > 0)
        {
            ByteArray[RECIVED_BYTES_LENGTCH] = client.read();
            RECIVED_BYTES_LENGTCH++;
        }

        Serial.println(ByteArray[0], HEX);
    }
    
   return RECIVED_BYTES_LENGTCH;

}
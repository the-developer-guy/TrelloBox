#include <WiFi.h>
#include <M5EPD.h>
#include <WiFiMulti.h>
#include "credentials.h"
#include "trello_process.h"
#include "m5control.h"
#include "wirelesscontrol.h"

#define uS_TO_S_FACTOR 1000000ULL    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP (60 * 60 * 12) /* Time ESP32 will go to sleep (in seconds) */
#define WIFI_CONNECT_TIMEOUT_MILLIS (10000)

M5EPD_Canvas canvas(&M5.EPD);
WiFiMulti wiFiMulti;
String payload;

void setup() 
{
  m5Setup(M5, canvas, TIME_TO_SLEEP * uS_TO_S_FACTOR);
  if(wifiConnect(WiFi, wiFiMulti, ssid, password, WIFI_CONNECT_TIMEOUT_MILLIS) == false)
  {
    Serial.println("Couldn't connect to WiFi in time!");
    esp_deep_sleep_start();
  }
  
  if(getHttpData(trelloApiLink, payload, rootCACertificate) == true)
  {
    processJson(payload);
    const char* task = getNext();
    int line = 0;

    while ((task != NULL) && (line < 12)) 
    {
      canvas.drawString(task, 10, line * 40 + 10);
      line++;
      task = getNext();
    }
    M5.EPD.Clear(true);
    canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    delay(1000);
  }
  
  esp_deep_sleep_start();
}

void loop()
{}

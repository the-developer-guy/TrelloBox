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
JsonProcessor jsonProcessor;

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
    if(jsonProcessor.setJson(payload) == true)
    {
      const char* task = jsonProcessor.getNext();

      while ((task != NULL) && (jsonProcessor.getLine() < 12)) 
      {
        canvas.drawString(task, 10, jsonProcessor.getLine() * 40);
        task = jsonProcessor.getNext();
      }
      M5.EPD.Clear(true);
      canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
    }
    else
    {
      Serial.println("Error processing the response JSON!");
    }
    delay(1000);
  }
  
  esp_deep_sleep_start();
}

void loop()
{}

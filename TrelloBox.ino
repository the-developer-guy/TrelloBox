#include <WiFi.h>
#include <WebServer.h>
#include <M5EPD.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
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
  
  WiFiClientSecure* client = new WiFiClientSecure;
  if (client) {
    client->setCACert(rootCACertificate);
    {
      HTTPClient https;
      Serial.print("[HTTPS] begin...\n");
      if (https.begin(*client, trelloApiLink)) {
        Serial.print("[HTTPS] GET...\n");
        int httpCode = https.GET();
        if (httpCode > 0) {
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
            Serial.println(payload);
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
  processJson(payload);
  const char* task = getNext();
  int line = 0;

  while (task != NULL) {
    canvas.drawString(task, 10, line * 40 + 10);
    line++;
    task = getNext();
  }
  M5.EPD.Clear(true);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
  delay(1000);
  esp_deep_sleep_start();
}
void loop() {
}

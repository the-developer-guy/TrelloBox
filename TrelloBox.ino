#include <WiFi.h>
#include <WebServer.h>
#include <M5EPD.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "credentials.h"
#include "trello_process.h"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  (60*60*12)        /* Time ESP32 will go to sleep (in seconds) */

void m5Setup(void);
void m5Print(String s);
void setClock();

M5EPD_Canvas canvas(&M5.EPD);
WiFiMulti WiFiMulti;
String payload;

void setup() {
  m5Setup();
  Serial.println("ESP started");

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.print("Waiting for WiFi to connect...");
  while ((WiFiMulti.run() != WL_CONNECTED)) {
    Serial.print(".");
  }
  Serial.println(" connected");

  setClock();  
 
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    client -> setCACert(rootCACertificate);
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
  canvas.clear();
  while(task != NULL)
  {
    canvas.drawString(task, 45, line * 40 + 10);
    line++;
    task = getNext();
  }
  M5.EPD.Clear(true);
  canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
  delay(5000);
  esp_deep_sleep_start();
  
}
void loop() {
  delay(1);
}

void m5Setup(void)
{
  M5.begin(false,false,true,false,true);
  M5.EPD.SetRotation(0);
  M5.EPD.Clear(true);
  M5.RTC.begin();
  canvas.createCanvas(960, 540);
  canvas.setTextSize(3);
  canvas.drawString("Updating...", 400, 250);
  canvas.pushCanvas(0,0,UPDATE_MODE_DU4);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}
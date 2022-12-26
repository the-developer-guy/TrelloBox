#include <WiFi.h>
#include <WebServer.h>
#include <M5EPD.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include "credentials.h"
#include "trello_process.h"

#define uS_TO_S_FACTOR 1000000ULL    /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP (60 * 60 * 12) /* Time ESP32 will go to sleep (in seconds) */

void m5Setup(void);
void m5Print(String s);

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

void m5Setup(void) {
  M5.begin(false, true, true, false, true);
  M5.EPD.SetRotation(0);
  M5.EPD.Clear(true);
  M5.RTC.begin();
  canvas.loadFont("/ArialBlack.ttf", SD);
  canvas.createCanvas(960, 540);
  canvas.createRender(32, 1024);
  canvas.setTextSize(32);
  canvas.setTextColor(15);
  canvas.drawString("Updating...", 400, 250);
  canvas.pushCanvas(0, 0, UPDATE_MODE_DU4);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  gpio_hold_en((gpio_num_t)M5EPD_MAIN_PWR_PIN);
  gpio_deep_sleep_hold_en();
  canvas.clear();
}

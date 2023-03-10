#pragma once

#include <WiFi.h>
#include <WiFiMulti.h>
#include <Arduino.h>

bool wifiConnect(WiFiClass& wifi, WiFiMulti& wifiMulti, const char* ssid, const char* password, int timeout);
bool getHttpData(String& url, String& response, const char* rootCertificate);

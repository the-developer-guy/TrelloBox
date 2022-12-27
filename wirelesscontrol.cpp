#include "wirelesscontrol.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WebServer.h>

bool wifiConnect(WiFiClass& wifi, WiFiMulti& wifiMulti, const char* ssid, const char* password, int timeout)
{
  wifi.mode(WIFI_STA);
  wifiMulti.addAP(ssid, password);

  uint32_t timeoutTimestamp = millis() + timeout;
  bool wifiConnected = wifiMulti.run() == WL_CONNECTED;
  while ((wifiConnected == false) && (millis() <= timeoutTimestamp)) 
  {
    wifiConnected = wifiMulti.run() == WL_CONNECTED;
  }
  return wifiConnected;
}

bool getHttpData(String& url, String& response, const char* rootCertificate)
{
  WiFiClientSecure* client = new WiFiClientSecure;
  bool getSuccessful = false;
  if (client) 
  {
    client->setCACert(rootCertificate);
    {
      HTTPClient https;
      if (https.begin(*client, url)) 
      {
        int httpCode = https.GET();
        if (httpCode > 0) 
        {
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) 
          {
            response = https.getString();
            getSuccessful = true;
          }
        }
        https.end();
      } 
    }
    delete client;
  }
  return getSuccessful;
}
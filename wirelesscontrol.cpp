#include "wirelesscontrol.h"


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

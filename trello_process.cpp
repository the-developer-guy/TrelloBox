#include "trello_process.h"
#include <ArduinoJson.h>

StaticJsonDocument<50000> doc;
int counter = 0;

void processJson(String str)
{
  DeserializationError error = deserializeJson(doc, str);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    return;
  }
  counter = 0;
}

const char* getNext(void)
{
  const char* value = doc[counter]["name"];
  counter++;
  return value;
}

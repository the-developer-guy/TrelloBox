#pragma once

#include <ArduinoJson.h>

class JsonProcessor
{
  private:
    StaticJsonDocument<50000> doc;
    int counter = 0;
  public:
    JsonProcessor();
    bool setJson(String str);
    const char* getNext(void);
    int getLine(void){ return counter; }
};

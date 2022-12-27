#include "trello_process.h"

JsonProcessor::JsonProcessor()
{
  counter = 0;
}

bool JsonProcessor::setJson(String str)
{
  DeserializationError error = deserializeJson(doc, str);
  if (error) 
  {
    counter = -1;
    return false;
  }
  counter = 0;
  return true;
}

const char* JsonProcessor::getNext(void)
{
  if(counter == -1)
  {
    return NULL;
  }
  const char* value = doc[counter]["name"];
  counter++;
  return value;
}

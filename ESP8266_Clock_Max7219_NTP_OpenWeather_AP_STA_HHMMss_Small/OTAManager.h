#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <Arduino.h>
#include <ESP8266mDNS.h>

class OTAManager {
public:
  OTAManager();
  
  void begin();
  void handle();
  
private:
  void setupOTAHandlers();
};

#endif

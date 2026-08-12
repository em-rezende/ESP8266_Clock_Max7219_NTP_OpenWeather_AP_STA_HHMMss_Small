#ifndef OTAWEBINTERFACE_H
#define OTAWEBINTERFACE_H

#include "Arduino.h"

class OTAWebInterface {
public:
  OTAWebInterface();
  
  String getOTAPageHTML();
  String getUpdateSuccessHTML();
  String getUpdateErrorHTML(const String& error);
  
private:
  String generateHeader(String title);
  String generateCSS();
};

#endif
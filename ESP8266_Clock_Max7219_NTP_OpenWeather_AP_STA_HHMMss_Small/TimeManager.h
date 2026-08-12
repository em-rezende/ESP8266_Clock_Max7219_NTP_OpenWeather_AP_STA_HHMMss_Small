#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "Arduino.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// Array com nomes dos dias abreviados
extern const char* dayNames[];
extern const char* monthNames[];

class TimeManager {
public:
  TimeManager();
  
  void begin();
  void update();
  bool forceUpdate();
  String formatTime();
  void updateInfoString();
  String timestampToLocalTime(unsigned long timestamp);
  
  // Variáveis públicas
  int h, m, s;      // Hora atual
  int d, mo, ye, w; // Data atual
  bool dots;
  bool isPM;
  
  String infoString;
  
  bool ntpSynced;
  unsigned long ntpLastCheck;
  
private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;
  
  void updateTimeFromNTP();
};

extern TimeManager timeManager;

#endif
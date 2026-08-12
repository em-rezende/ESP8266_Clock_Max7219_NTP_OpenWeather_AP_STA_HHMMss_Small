#ifndef WEATHERMANAGER_H
#define WEATHERMANAGER_H

#include "Arduino.h"
#include <ESP8266WiFi.h>

class WeatherManager {
public:
  WeatherManager();
  
  void updateWeatherData();
  String getWindDir(int degrees);
  
  // Variáveis públicas
  float currentTemp;
  float feelsLike;
  float tempMin;
  float tempMax;
  float humidity;
  float pressure;
  float windSpeed;
  int windDeg;
  String weatherDescription;
  int clouds;
  int visibility;
  String cityName;
  unsigned long sunriseTime;
  unsigned long sunsetTime;
  
  unsigned long lastWeatherUpdate;
  
private:
  WiFiClient client;
  void parseWeatherData(String jsonData);
};

#endif

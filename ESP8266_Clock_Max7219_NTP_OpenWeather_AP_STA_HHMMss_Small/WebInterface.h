#ifndef WEBINTERFACE_H
#define WEBINTERFACE_H

#include <Arduino.h>

// Forward declarations
class ConfigManager;
class TimeManager;
class WeatherManager;
class DisplayManager;

class WebInterface {
public:
  WebInterface();
  
  // Setters
  void setConfigManager(ConfigManager* mgr);
  void setTimeManager(TimeManager* mgr);
  void setWeatherManager(WeatherManager* mgr);
  void setDisplayManager(DisplayManager* mgr);
  
  // HTML page generators
  String getMainPageHTML();
  String getAdvancedConfigPageHTML();  // NOVA FUNÇÃO
  String getSuccessPageHTML(const String& message = "");
  
  // Helper functions
  String formatTimeForWeb();
  String escapeHTML(String input);
  
private:
  // Member variables
  ConfigManager* config;
  TimeManager* time;
  WeatherManager* weather;
  DisplayManager* display;
  
  // Private helper function
  bool isWeatherAvailable();
};

#endif
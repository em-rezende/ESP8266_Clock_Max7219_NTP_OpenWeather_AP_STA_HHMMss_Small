#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include <ESP8266WebServer.h>

// Forward declarations
class WebInterface;
class ConfigManager;
class TimeManager;
class WeatherManager;
class DisplayManager;

class WebServerManager {
public:
  WebServerManager();
  
  void begin();
  void handleClient();
  
  // Getter para o servidor
  ESP8266WebServer* getServer();
  
  // Setters
  void setWebInterface(WebInterface* interface);
  void setConfigManager(ConfigManager* mgr);
  void setTimeManager(TimeManager* mgr);
  void setWeatherManager(WeatherManager* mgr);
  void setDisplayManager(DisplayManager* mgr);
  
  // Handlers de rotas (REMOVA handleConfig)
  void handleRoot();
  void handleData();
  void handleRedirectToAdvanced();
  void handleAdvancedConfig();
  void handleAdvancedUpdate();
  void handleOTA();
  void handleRestart();
  void handleNotFound();
  
private:
  ESP8266WebServer server;
  
  // Ponteiros para os gerenciadores
  WebInterface* webInterface;
  ConfigManager* config;
  TimeManager* time;
  WeatherManager* weather;
  DisplayManager* display;
  
  // Método privado
  void sendJSONData();
};

#endif
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include "Arduino.h"
#include <EEPROM.h>
#include <WiFiManager.h>

// Definições
static const uint8_t CLOCK_DSTMODE_NONE = 0;
static const uint8_t CLOCK_DSTMODE_EU = 1;
static const uint8_t CLOCK_DSTMODE_US = 2;
static const uint32_t SETTINGS_MAGIC = 0x4B315749;
static const int EEPROM_ADDR = 0;

// Estrutura de configurações
struct Settings {
  // Configurações de horário
  float utcOffset;
  bool is12H;
  bool observeDST;
  uint8_t dstMode;
  float dstExtraHours;

  // Configurações de clima
  bool weatherEnabled;
  char weatherAPIKey[33];
  char weatherLat[12];
  char weatherLon[13];
  char weatherLang[4];
  int weatherUpdateInterval;
  
  // Configurações de exibição
  int clockDisplayDuration;

  uint32_t magic;
};

class ConfigManager {
public:
  ConfigManager();
  ~ConfigManager();
  
  void loadSettings();
  void saveSettings();
  void loadDefaultSettings();
  void applySettingsFromEEPROM();
  void updateWiFiManagerBuffers();
  void updateEEPROMStructure();
  
  // Getters
  float getUtcOffset() const { return utcOffset; }
  bool getIs12HFormat() const { return is12HFormat; }
  bool getWeatherEnabled() const { return weatherEnabled; }
  String getWeatherAPIKey() const { return weatherAPIKey; }
  String getWeatherLat() const { return weatherLat; }
  String getWeatherLon() const { return weatherLon; }
  String getWeatherLang() const { return weatherLang; }
  int getClockDisplayDuration() const { return clockDisplayDuration; }
  int getWeatherUpdateInterval() const { return weatherUpdateInterval; }
  
  // Variáveis
  float utcOffset;
  bool is12HFormat;
  bool observeDST;
  uint8_t dstMode;
  float dstExtraHours;
  
  bool weatherEnabled;
  String weatherAPIKey;
  String weatherLat;
  String weatherLon;
  String weatherLang;
  int weatherUpdateInterval;
  
  int clockDisplayDuration;
  
  void setupWiFiManagerParameters(WiFiManager& wifiManager);
  void saveConfigCallback();

private:
  Settings settings;
  void initializeBuffers();
  
  // Buffers para WiFiManager
  char bufUtc[10];
  char buf12h[6];
  char bufDst[6];
  char bufDstMode[10];
  char bufDstExtra[10];
  char bufWeatherEnabled[6];
  char bufWeatherAPIKey[33];
  char bufWeatherLat[12];
  char bufWeatherLon[13];
  char bufWeatherLang[4];
  char bufWeatherUpdateInterval[4];
  char bufClockDisplayDuration[4];
  
  // Ponteiros para parâmetros (alocados dinamicamente)
  WiFiManagerParameter* pUtc;
  WiFiManagerParameter* p12h;
  WiFiManagerParameter* pDst;
  WiFiManagerParameter* pDstMode;
  WiFiManagerParameter* pDstExtra;
  WiFiManagerParameter* pWeatherEnabled;
  WiFiManagerParameter* pWeatherAPIKey;
  WiFiManagerParameter* pWeatherLat;
  WiFiManagerParameter* pWeatherLon;
  WiFiManagerParameter* pWeatherLang;
  WiFiManagerParameter* pWeatherUpdateInterval;
  WiFiManagerParameter* pClockDisplayDuration;
};

#endif

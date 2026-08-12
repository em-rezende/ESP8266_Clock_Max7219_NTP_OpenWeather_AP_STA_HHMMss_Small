#include "ConfigManager.h"
#include "Arduino.h"

ConfigManager::ConfigManager() 
  : pUtc(nullptr), p12h(nullptr), pDst(nullptr), pDstMode(nullptr), 
    pDstExtra(nullptr), pWeatherEnabled(nullptr), pWeatherAPIKey(nullptr),
    pWeatherLat(nullptr), pWeatherLon(nullptr), pWeatherLang(nullptr),
    pWeatherUpdateInterval(nullptr), pClockDisplayDuration(nullptr) {
  
  // Inicializar buffers
  initializeBuffers();
}

ConfigManager::~ConfigManager() {
  // Liberar memória dos parâmetros
  delete pUtc;
  delete p12h;
  delete pDst;
  delete pDstMode;
  delete pDstExtra;
  delete pWeatherEnabled;
  delete pWeatherAPIKey;
  delete pWeatherLat;
  delete pWeatherLon;
  delete pWeatherLang;
  delete pWeatherUpdateInterval;
  delete pClockDisplayDuration;
}

void ConfigManager::initializeBuffers() {
  // Inicializar buffers com valores padrão
  strcpy(bufUtc, "-3.0");
  strcpy(buf12h, "false");
  strcpy(bufDst, "false");
  strcpy(bufDstMode, "NONE");
  strcpy(bufDstExtra, "1.0");
  strcpy(bufWeatherEnabled, "true");
  strcpy(bufWeatherAPIKey, "");
  strcpy(bufWeatherLat, "");
  strcpy(bufWeatherLon, "");
  strcpy(bufWeatherLang, "pt");
  strcpy(bufWeatherUpdateInterval, "30");
  strcpy(bufClockDisplayDuration, "30");
}

void ConfigManager::loadDefaultSettings() {
  settings.utcOffset = -3.0;
  settings.is12H = true;
  settings.observeDST = false;
  settings.dstMode = CLOCK_DSTMODE_US;
  settings.dstExtraHours = 1.0;

  settings.weatherEnabled = false;
  strcpy(settings.weatherAPIKey, "");
  strcpy(settings.weatherLat, "");
  strcpy(settings.weatherLon, "");
  strcpy(settings.weatherLang, "pt");
  settings.weatherUpdateInterval = 30;

  settings.clockDisplayDuration = 30;
  settings.magic = SETTINGS_MAGIC;
}

void ConfigManager::loadSettings() {
  EEPROM.begin(sizeof(Settings));
  EEPROM.get(EEPROM_ADDR, settings);

  if (settings.magic != SETTINGS_MAGIC) {
    loadDefaultSettings();
    EEPROM.put(EEPROM_ADDR, settings);
    EEPROM.commit();
  }
}

void ConfigManager::saveSettings() {
  settings.magic = SETTINGS_MAGIC;
  EEPROM.put(EEPROM_ADDR, settings);
  EEPROM.commit();
}

void ConfigManager::applySettingsFromEEPROM() {
  utcOffset = settings.utcOffset;
  is12HFormat = settings.is12H;
  observeDST = settings.observeDST;
  dstMode = settings.dstMode;
  dstExtraHours = settings.dstExtraHours;

  weatherEnabled = settings.weatherEnabled;
  weatherAPIKey = String(settings.weatherAPIKey);
  weatherLat = String(settings.weatherLat);
  weatherLon = String(settings.weatherLon);
  weatherLang = "&lang=" + String(settings.weatherLang);
  weatherUpdateInterval = settings.weatherUpdateInterval;

  // Limpar espaços ao carregar
  weatherAPIKey.trim();
  weatherLat.trim();
  weatherLon.trim();

  clockDisplayDuration = settings.clockDisplayDuration;

  if (clockDisplayDuration < 10) {
    clockDisplayDuration = 10;
  }
}

void ConfigManager::updateWiFiManagerBuffers() {
  dtostrf(utcOffset, 1, 1, bufUtc);
  strcpy(buf12h, is12HFormat ? "true" : "false");
  strcpy(bufDst, observeDST ? "true" : "false");

  if (dstMode == CLOCK_DSTMODE_US) strcpy(bufDstMode, "AUTO_US");
  else if (dstMode == CLOCK_DSTMODE_EU) strcpy(bufDstMode, "AUTO_EU");
  else strcpy(bufDstMode, "NONE");

  dtostrf(dstExtraHours, 1, 1, bufDstExtra);
  strcpy(bufWeatherEnabled, weatherEnabled ? "true" : "false");
  strncpy(bufWeatherAPIKey, weatherAPIKey.c_str(), sizeof(bufWeatherAPIKey) - 1);
  strncpy(bufWeatherLat, weatherLat.c_str(), sizeof(bufWeatherLat) - 1);
  strncpy(bufWeatherLon, weatherLon.c_str(), sizeof(bufWeatherLon) - 1);
  strncpy(bufWeatherLang, settings.weatherLang, sizeof(bufWeatherLang) - 1);
  itoa(weatherUpdateInterval, bufWeatherUpdateInterval, 10);
  itoa(clockDisplayDuration, bufClockDisplayDuration, 10);
}

void ConfigManager::updateEEPROMStructure() {
  settings.utcOffset = utcOffset;
  settings.is12H = is12HFormat;
  settings.observeDST = observeDST;
  settings.dstMode = dstMode;
  settings.dstExtraHours = dstExtraHours;

  settings.weatherEnabled = weatherEnabled;
  
  weatherAPIKey.trim();
  memset(settings.weatherAPIKey, 0, sizeof(settings.weatherAPIKey));
  strncpy(settings.weatherAPIKey, weatherAPIKey.c_str(), sizeof(settings.weatherAPIKey) - 1);
  settings.weatherAPIKey[sizeof(settings.weatherAPIKey) - 1] = '\0';
  
  weatherLat.trim();
  memset(settings.weatherLat, 0, sizeof(settings.weatherLat));
  strncpy(settings.weatherLat, weatherLat.c_str(), sizeof(settings.weatherLat) - 1);
  settings.weatherLat[sizeof(settings.weatherLat) - 1] = '\0';
  
  weatherLon.trim();
  memset(settings.weatherLon, 0, sizeof(settings.weatherLon));
  strncpy(settings.weatherLon, weatherLon.c_str(), sizeof(settings.weatherLon) - 1);
  settings.weatherLon[sizeof(settings.weatherLon) - 1] = '\0';
  
  String lang = weatherLang;
  if (lang.startsWith("&lang=")) {
    lang = lang.substring(6);
  }
  lang.trim();
  memset(settings.weatherLang, 0, sizeof(settings.weatherLang));
  strncpy(settings.weatherLang, lang.c_str(), sizeof(settings.weatherLang) - 1);
  settings.weatherLang[sizeof(settings.weatherLang) - 1] = '\0';
  
  settings.weatherUpdateInterval = weatherUpdateInterval;
  settings.clockDisplayDuration = clockDisplayDuration;
  
  if (settings.clockDisplayDuration < 10) {
    settings.clockDisplayDuration = 10;
  }
}

void ConfigManager::setupWiFiManagerParameters(WiFiManager& wifiManager) {
  // Criar parâmetros se não existirem
  if (!pUtc) {
    pUtc = new WiFiManagerParameter("utc", "UTC Offset (ex: -3.0)", bufUtc, sizeof(bufUtc));
    p12h = new WiFiManagerParameter("12h", "Formato 12h (true/false)", buf12h, sizeof(buf12h));
    pDst = new WiFiManagerParameter("dst", "Horário de verão (true/false)", bufDst, sizeof(bufDst));
    pDstMode = new WiFiManagerParameter("dstmode", "Modo DST: AUTO_US/AUTO_EU/NONE", bufDstMode, sizeof(bufDstMode));
    pDstExtra = new WiFiManagerParameter("dstextra", "Horas extra DST (1.0)", bufDstExtra, sizeof(bufDstExtra));
    pWeatherEnabled = new WiFiManagerParameter("weatherenabled", "Clima ativo (true/false)", bufWeatherEnabled, sizeof(bufWeatherEnabled));
    pWeatherAPIKey = new WiFiManagerParameter("weatherapikey", "API Key OpenWeatherMap", bufWeatherAPIKey, sizeof(bufWeatherAPIKey));
    pWeatherLat = new WiFiManagerParameter("weatherlat", "Latitude (ex: -19.9167)", bufWeatherLat, sizeof(bufWeatherLat));
    pWeatherLon = new WiFiManagerParameter("weatherlon", "Longitude (ex: -43.9345)", bufWeatherLon, sizeof(bufWeatherLon));
    pWeatherLang = new WiFiManagerParameter("weatherlang", "Idioma: pt, en, es, de, fr, it", bufWeatherLang, sizeof(bufWeatherLang));
    pWeatherUpdateInterval = new WiFiManagerParameter("weatherupdate", "Atualizar clima a cada (minutos)", bufWeatherUpdateInterval, sizeof(bufWeatherUpdateInterval));
    pClockDisplayDuration = new WiFiManagerParameter("clockduration", "Mostrar relógio por (segundos)", bufClockDisplayDuration, sizeof(bufClockDisplayDuration));
  }
  
  // Atualizar buffers
  updateWiFiManagerBuffers();
  
  // Adicionar parâmetros ao WiFiManager
  wifiManager.addParameter(pUtc);
  wifiManager.addParameter(p12h);
  wifiManager.addParameter(pDst);
  wifiManager.addParameter(pDstMode);
  wifiManager.addParameter(pDstExtra);
  wifiManager.addParameter(pWeatherEnabled);
  wifiManager.addParameter(pWeatherAPIKey);
  wifiManager.addParameter(pWeatherLat);
  wifiManager.addParameter(pWeatherLon);
  wifiManager.addParameter(pWeatherLang);
  wifiManager.addParameter(pWeatherUpdateInterval);
  wifiManager.addParameter(pClockDisplayDuration);
}

void ConfigManager::saveConfigCallback() {
  if (!pUtc) return; // Parâmetros não inicializados
  
  utcOffset = atof(pUtc->getValue());
  is12HFormat = String(p12h->getValue()) == "true";
  observeDST = String(pDst->getValue()) == "true";
  dstExtraHours = atof(pDstExtra->getValue());

  String mode = String(pDstMode->getValue());
  mode.toUpperCase();
  if (mode == "AUTO_US") dstMode = CLOCK_DSTMODE_US;
  else if (mode == "AUTO_EU") dstMode = CLOCK_DSTMODE_EU;
  else dstMode = CLOCK_DSTMODE_NONE;

  weatherEnabled = String(pWeatherEnabled->getValue()) == "true";
  weatherAPIKey = String(pWeatherAPIKey->getValue());
  weatherLat = String(pWeatherLat->getValue());
  weatherLon = String(pWeatherLon->getValue());
  weatherLang = "&lang=" + String(pWeatherLang->getValue());
  weatherUpdateInterval = atoi(pWeatherUpdateInterval->getValue());

  weatherAPIKey.trim();
  weatherLat.trim();
  weatherLon.trim();
  
  clockDisplayDuration = atoi(pClockDisplayDuration->getValue());

  if (clockDisplayDuration < 10) {
    clockDisplayDuration = 10;
  }

  updateEEPROMStructure();
  saveSettings();
}

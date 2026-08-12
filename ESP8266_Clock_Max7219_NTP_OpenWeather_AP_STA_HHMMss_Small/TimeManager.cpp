#include "TimeManager.h"
#include "ConfigManager.h"
#include "WeatherManager.h"

extern ConfigManager configManager;
extern WeatherManager weatherManager;  // <-- DECLARE a instância externa
extern const char* dayNames[];
extern const char* monthNames[];

TimeManager::TimeManager() 
  : timeClient(ntpUDP, "pool.ntp.org", 0) {
  h = 12;
  m = 0;
  s = 0;
  d = 31;
  mo = 12;
  ye = 2025;
  w = 3;
  dots = false;
  isPM = false;
  ntpSynced = false;
  ntpLastCheck = 0;
  infoString = "";
}

void TimeManager::begin() {
  timeClient.begin();
  timeClient.setUpdateInterval(3600000);
}

void TimeManager::update() {
  if (!ntpSynced && millis() - ntpLastCheck > 5000) {
    ntpLastCheck = millis();
    if (timeClient.forceUpdate()) {
      updateTimeFromNTP();
      ntpSynced = true;
    }
  }
}

bool TimeManager::forceUpdate() {
  if (timeClient.forceUpdate()) {
    updateTimeFromNTP();
    ntpSynced = true;
    return true;
  }
  return false;
}

void TimeManager::updateTimeFromNTP() {
  unsigned long epoch = timeClient.getEpochTime();
  
  // Aplicar offset UTC
  epoch += (long)(configManager.getUtcOffset() * 3600);
  
  time_t rawTime = (time_t)epoch;
  struct tm* timeinfo = gmtime(&rawTime);
  
  h = timeinfo->tm_hour;
  m = timeinfo->tm_min;
  s = timeinfo->tm_sec;
  
  d = timeinfo->tm_mday;
  mo = timeinfo->tm_mon + 1;
  ye = timeinfo->tm_year + 1900;
  w = timeinfo->tm_wday;
}

String TimeManager::formatTime() {
  String timeStr = "";
  if (h < 10) timeStr += "0";
  timeStr += String(h);
  timeStr += ":";
  if (m < 10) timeStr += "0";
  timeStr += String(m);
  timeStr += ":";
  if (s < 10) timeStr += "0";
  timeStr += String(s);
  return timeStr;
}

void TimeManager::updateInfoString() {
  // Data básica - Formato: "Sab, 7 fev 2026"
  infoString = String(dayNames[w]) + ", " + String(d) + " " + 
               monthNames[mo - 1] + " " + String(ye);
  
  // Adicionar dados climáticos se disponíveis
  // IMPORTANTE: Acessar a instância global weatherManager
  if (configManager.getWeatherEnabled() && 
      configManager.getWeatherAPIKey().length() > 0) {
    
    // Usar a instância global diretamente
    if (weatherManager.cityName.length() > 0) {
      
      // FORMATO: " - Temp:20.5C Umid:94% Press:1017hPa Vento:7.4km/h Dir:S Nublado"
      infoString += " - ";
      
      // Temperatura
      infoString += "Temp: " + String(weatherManager.currentTemp, 1) + "C ";
      
      // Umidade
      infoString += "Umid: " + String(weatherManager.humidity, 0) + "% ";
      
      // Pressão
      infoString += "Press: " + String(weatherManager.pressure, 0) + "hPa ";
      
      // Vento (se houver)
      if (weatherManager.windSpeed > 0.1) {
        infoString += "Vento: " + String(weatherManager.windSpeed * 3.6, 1) + "km/h ";
        
        // Direção do vento
        String windDir = weatherManager.getWindDir(weatherManager.windDeg);
        if (windDir.length() > 0) {
          infoString += "Dir: " + windDir + " ";
        }
      } else {
        infoString += "Vento: calmo ";
      }
      
      // Descrição do tempo (se disponível)
      if (weatherManager.weatherDescription.length() > 0) {
        infoString += weatherManager.weatherDescription;
      }
    }
  }
  
  Serial.print("📝 InfoString atualizada: ");
  Serial.println(infoString);
}


String TimeManager::timestampToLocalTime(unsigned long timestamp) {
  if (timestamp == 0) return "--:--";
  
  // Ajustar para fuso horário local
  timestamp += (long)(configManager.getUtcOffset() * 3600);
  
  // Converter para horas e minutos
  time_t timeValue = timestamp;
  struct tm* timeinfo = gmtime(&timeValue);
  
  char buffer[6];
  sprintf(buffer, "%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
  return String(buffer);
}
#include "WeatherManager.h"
#include "ConfigManager.h"
#include <ArduinoJson.h> // Usar a biblioteca v.5.13.5

extern ConfigManager configManager;

WeatherManager::WeatherManager() {
  currentTemp = 0.0;
  feelsLike = 0.0;
  tempMin = 0.0;
  tempMax = 0.0;
  humidity = 0.0;
  pressure = 0.0;
  windSpeed = 0.0;
  windDeg = 0;
  weatherDescription = "";
  clouds = 0;
  visibility = 0;
  cityName = "";
  sunriseTime = 0;
  sunsetTime = 0;
  lastWeatherUpdate = 0;
}

void WeatherManager::updateWeatherData() {
  if (!configManager.getWeatherEnabled() || 
      configManager.getWeatherAPIKey().length() == 0 ||
      configManager.getWeatherLat().length() == 0 ||
      configManager.getWeatherLon().length() == 0) {
    Serial.println("⚠️  Clima: Configuração incompleta");
    return;
  }

  Serial.print("🌤️  Conectando à API OpenWeatherMap... ");
  
  if (client.connect("api.openweathermap.org", 80)) {
    Serial.println("OK");
    
    String url = "/data/2.5/weather?";
    url += "lat=" + configManager.getWeatherLat();
    url += "&lon=" + configManager.getWeatherLon();
    url += "&units=metric";
    url += "&appid=" + configManager.getWeatherAPIKey();
    
    // Idioma (se disponível)
    String lang = "pt"; // Padrão português
    url += "&lang=" + lang;

    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("❌ Timeout da API");
        client.stop();
        return;
      }
    }

    String response = "";
    while (client.available()) {
      response += client.readStringUntil('\r');
    }
    
    client.stop();

    int jsonStart = response.indexOf('{');
    if (jsonStart == -1) {
      Serial.println("❌ Resposta não contém JSON");
      return;
    }
    
    String jsonData = response.substring(jsonStart);
    parseWeatherData(jsonData);
    
  } else {
    Serial.println("❌ Falha na conexão");
  }
}

void WeatherManager::parseWeatherData(String jsonData) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonData);
  
  if (!root.success()) {
    Serial.println("❌ Falha ao analisar JSON");
    return;
  }

  // Parse dos dados
  if (root["main"].success()) {
    currentTemp = root["main"]["temp"];
    feelsLike = root["main"]["feels_like"];
    tempMin = root["main"]["temp_min"];
    tempMax = root["main"]["temp_max"];
    humidity = root["main"]["humidity"];
    pressure = root["main"]["pressure"];
  }
  
  if (root["name"].success()) {
    cityName = root["name"].as<String>();
  }
  
  if (root["visibility"].success()) {
    visibility = root["visibility"];
  }
  
  if (root["clouds"].success() && root["clouds"]["all"].success()) {
    clouds = root["clouds"]["all"];
  }
  
  if (root["wind"].success()) {
    windSpeed = root["wind"]["speed"];
    windDeg = root["wind"]["deg"];
  }
  
  if (root["sys"].success()) {
    sunriseTime = root["sys"]["sunrise"];
    sunsetTime = root["sys"]["sunset"];
  }
  
  if (root["weather"].success() && root["weather"].is<JsonArray>()) {
    JsonArray& weatherArray = root["weather"];
    if (weatherArray.size() > 0) {
      weatherDescription = weatherArray[0]["description"].as<String>();
      // Capitalizar primeira letra
      if (weatherDescription.length() > 0) {
        weatherDescription.toLowerCase();
        weatherDescription.setCharAt(0, toupper(weatherDescription.charAt(0)));
      }
    }
  }

  lastWeatherUpdate = millis();
  
  Serial.println("✅ Dados climáticos atualizados:");
  Serial.print("   Cidade: "); Serial.println(cityName);
  Serial.print("   Temperatura: "); Serial.print(currentTemp, 1); Serial.println("°C");
  Serial.print("   Condições: "); Serial.println(weatherDescription);
  Serial.print("   Umidade: "); Serial.print(humidity, 0); Serial.println("%");
  Serial.print("   Pressão: "); Serial.print(pressure, 0); Serial.println(" hPa");
  Serial.print("   Vento: "); Serial.print(windSpeed * 3.6, 1); Serial.println(" km/h");
}

String WeatherManager::getWindDir(int degrees) {
  if (windSpeed <= 0.1) return "calmo";
  
  degrees = degrees % 360;
  
  if (degrees >= 337.5 || degrees < 22.5) return "N";
  if (degrees >= 22.5 && degrees < 67.5) return "NE";
  if (degrees >= 67.5 && degrees < 112.5) return "L";
  if (degrees >= 112.5 && degrees < 157.5) return "SE";
  if (degrees >= 157.5 && degrees < 202.5) return "S";
  if (degrees >= 202.5 && degrees < 247.5) return "SO";
  if (degrees >= 247.5 && degrees < 292.5) return "O";
  if (degrees >= 292.5 && degrees < 337.5) return "NO";
  
  return "";
}

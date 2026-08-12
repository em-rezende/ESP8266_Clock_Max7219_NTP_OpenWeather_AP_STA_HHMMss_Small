#include "WebInterface.h"
#include "ConfigManager.h"
#include "TimeManager.h"
#include "WeatherManager.h"
#include "DisplayManager.h"
#include <ESP8266WiFi.h>

// ================== CONSTRUCTOR & SETTERS ==================

WebInterface::WebInterface() {
  config = nullptr;
  time = nullptr;
  weather = nullptr;
  display = nullptr;
}

void WebInterface::setConfigManager(ConfigManager* mgr) { 
  config = mgr; 
}

void WebInterface::setTimeManager(TimeManager* mgr) { 
  time = mgr; 
}

void WebInterface::setWeatherManager(WeatherManager* mgr) { 
  weather = mgr; 
}

void WebInterface::setDisplayManager(DisplayManager* mgr) { 
  display = mgr; 
}

// ================== PRIVATE HELPER ==================

bool WebInterface::isWeatherAvailable() {
  if (!config) return false;
  if (!weather) return false;
  
  return (config->getWeatherEnabled() && 
          config->getWeatherAPIKey().length() > 0);
}

// ================== HELPER FUNCTIONS ==================

String WebInterface::formatTimeForWeb() {
  if (!time) return "--:--:--";
  
  String timeStr = "";
  if (time->h < 10) timeStr += "0";
  timeStr += String(time->h);
  timeStr += ":";
  if (time->m < 10) timeStr += "0";
  timeStr += String(time->m);
  timeStr += ":";
  if (time->s < 10) timeStr += "0";
  timeStr += String(time->s);
  return timeStr;
}

String WebInterface::escapeHTML(String input) {
  input.replace("&", "&amp;");
  input.replace("<", "&lt;");
  input.replace(">", "&gt;");
  input.replace("\"", "&quot;");
  input.replace("'", "&#39;");
  return input;
}

// ================== HTML PAGES ==================

String WebInterface::getMainPageHTML() {
  String html;
  html.reserve(8000);
  
  html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Relógio LED Matrix ESP8266 - Monitor</title>";
  
  html += "<style>";
  html += "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; margin: 20px; background: #f0f0f0; color: #333; }";
  html += ".container { max-width: 900px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1, h2, h3 { color: #2c3e50; margin-bottom: 15px; }";
  html += ".back-link { display: inline-flex; align-items: center; color: #3498db; text-decoration: none; font-weight: bold; margin-bottom: 20px; }";
  html += ".back-link:hover { text-decoration: underline; }";
  html += ".config-section { margin-bottom: 25px; padding: 15px; background: #ecf0f1; border-radius: 5px; }";
  
  /* Display Virtual Responsivo */
  html += ".display-container { margin: 15px 0; }";
  html += ".grid-wrapper { overflow-x: auto; margin: 10px 0; }";
  html += ".grid { display: grid; grid-template-columns: repeat(32, 1fr); grid-template-rows: repeat(8, 1fr); gap: 1px; background: #111; padding: 8px; border-radius: 4px; min-width: 320px; }";
  html += ".pixel { aspect-ratio: 1/1; min-width: 8px; border-radius: 1px; transition: background 0.2s; }";
  html += ".pixel-on { background: #ff4444; box-shadow: 0 0 3px #ff4444; }";
  html += ".pixel-off { background: #222; border: 1px solid #444; }";
  html += ".clock { font-size: 2em; font-family: monospace; margin: 10px 0; text-align: center; color: #2c3e50; font-weight: bold; letter-spacing: 2px; }";
  html += ".info { background: #fff3cd; padding: 10px; border-radius: 5px; margin: 10px 0; }";
  
  html += "table { width: 100%; border-collapse: collapse; margin: 10px 0; }";
  html += "th { background: #3498db; color: white; text-align: left; padding: 10px; }";
  html += "td { padding: 8px 10px; border-bottom: 1px solid #ddd; }";
  html += "tr:nth-child(even) { background: #f9f9f9; }";
  
  html += ".btn { background: #3498db; color: white; border: none; padding: 8px 15px; border-radius: 5px; cursor: pointer; font-size: 0.95em; margin: 5px; }";
  html += ".btn:hover { background: #2980b9; }";
  html += ".btn-success { background: #27ae60; }";
  html += ".btn-success:hover { background: #219653; }";
  
  html += ".quick-links { display: flex; flex-wrap: wrap; gap: 10px; margin: 15px 0; }";
  html += ".link-item { flex: 1; min-width: 150px; background: #ecf0f1; padding: 10px; border-radius: 4px; text-align: center; }";
  html += ".link-item a { color: #3498db; text-decoration: none; font-weight: bold; }";
  html += ".link-item a:hover { text-decoration: underline; }";
  
  html += ".weather-section { background: #e8f4f8; border-left: 4px solid #2ecc71; }";
  html += ".weather-param { margin: 8px 0; padding: 8px; background: white; border-radius: 4px; }";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 15px; margin: 10px; }";
  html += "  .grid { min-width: 280px; }";
  html += "  .pixel { min-width: 6px; }";
  html += "  .clock { font-size: 1.8em; }";
  html += "  .quick-links { flex-direction: column; }";
  html += "  table { font-size: 0.9em; }";
  html += "}";
  html += "</style>";
  
  html += "<script>";
  html += "let autoRefresh = true;";
  html += "function updateTime() {";
  html += "  fetch('/data')";
  html += "    .then(r => r.json())";
  html += "    .then(data => {";
  html += "      if(data.time) document.getElementById('currentTime').textContent = data.time;";
  html += "      if(data.infoString) document.getElementById('infoString').textContent = data.infoString;";
  html += "      if(data.displayMode) document.getElementById('displayMode').textContent = data.displayMode;";
  html += "      if(data.brightness) document.getElementById('brightness').textContent = data.brightness;";
  html += "      if(data.ip) document.getElementById('ipAddress').textContent = data.ip;";
  html += "      if(data.wifiStatus) {";
  html += "        let statusEl = document.getElementById('wifiStatus');";
  html += "        statusEl.textContent = data.wifiStatus;";
  html += "        statusEl.className = 'status ' + (data.wifiStatus === 'Conectado' ? 'connected' : 'disconnected');";
  html += "      }";
  html += "      if(data.displayData) updateDisplay(data.displayData);";
  html += "      if(data.weather) updateWeather(data.weather);";
  html += "    })";
  html += "    .catch(err => console.error('Erro:', err));";
  html += "}";
  
  html += "function updateDisplay(displayData) {";
  html += "  const grid = document.getElementById('displayGrid');";
  html += "  if(!grid) return;";
  html += "  grid.innerHTML = '';";
  html += "  for(let row = 0; row < 8; row++) {";
  html += "    for(let col = 0; col < 32; col++) {";
  html += "      const pixel = document.createElement('div');";
  html += "      const byteValue = displayData[col] || 0;";
  html += "      const isOn = (byteValue >> row) & 1;";
  html += "      pixel.className = 'pixel ' + (isOn ? 'pixel-on' : 'pixel-off');";
  html += "      pixel.title = 'Col:' + col + ' Linha:' + row + ' Byte:0x' + byteValue.toString(16);";
  html += "      grid.appendChild(pixel);";
  html += "    }";
  html += "  }";
  html += "}";

  html += "function updateWeather(weather) {";
  html += "  if(!weather) return;";
  html += "  const elements = ['city','temp','feelsLike','humidity','pressure','windSpeed','description'];";
  html += "  elements.forEach(id => {";
  html += "    if(weather[id] && document.getElementById('weather_' + id)) {";
  html += "      document.getElementById('weather_' + id).textContent = weather[id];";
  html += "    }";
  html += "  });";
  html += "}";
  
  html += "function toggleAutoRefresh() {";
  html += "  autoRefresh = !autoRefresh;";
  html += "  const btn = document.getElementById('autoRefreshBtn');";
  html += "  if(btn) btn.textContent = autoRefresh ? 'Desativar Auto-Refresh' : 'Ativar Auto-Refresh';";
  html += "  if(autoRefresh) startAutoRefresh();";
  html += "}";
  
  html += "function startAutoRefresh() {";
  html += "  if(autoRefresh) {";
  html += "    updateTime();";
  html += "    setTimeout(startAutoRefresh, 1000);";
  html += "  }";
  html += "}";
  
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  updateTime();";
  html += "  startAutoRefresh();";
  html += "});";
  html += "</script>";
  html += "</head>";
  
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>📱 Relógio LED Matrix ESP8266 - Monitor Web</h1>";
  
  // Seção 1: Display Virtual
  html += "<div class='section'>";
  html += "<h2>🖥️ Display Virtual 8x32</h2>";
  html += "<div class='display-container'>";
  html += "<div class='grid-wrapper'>";
  html += "<div id='displayGrid' class='grid'></div>";
  html += "</div>";
  html += "</div>";
  html += "<div class='clock' id='currentTime'>";
  html += formatTimeForWeb();
  html += "</div>";
  html += "<div class='info' id='infoString'>";
  if (time) html += escapeHTML(time->infoString);
  html += "</div>";
  html += "</div>";
  
  // Seção 2: Status do Sistema
  html += "<div class='section'>";
  html += "<h2>📊 Status do Sistema</h2>";
  html += "<table>";
  html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
  html += "<tr><td>Modo de Exibição</td><td id='displayMode'>Carregando...</td></tr>";
  html += "<tr><td>Nível de Brilho</td><td id='brightness'>Carregando...</td></tr>";
  html += "<tr><td>Endereço IP</td><td id='ipAddress'>" + WiFi.localIP().toString() + "</td></tr>";
  html += "<tr><td>Status WiFi</td><td><span id='wifiStatus' class='status ";
  html += (WiFi.status() == WL_CONNECTED ? "connected" : "disconnected");
  html += "'>";
  html += (WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado");
  html += "</span></td></tr>";
  html += "</table>";
  html += "</div>";
  
  // Seção 3: Configurações Atuais
  html += "<div class='section'>";
  html += "<h2>⚙️ Configurações Atuais</h2>";
  html += "<table>";
  html += "<tr><th>Configuração</th><th>Valor</th></tr>";
  if (config) {
    html += "<tr><td>Fuso Horário (UTC)</td><td>" + String(config->getUtcOffset(), 1) + "</td></tr>";
    html += "<tr><td>Formato 12h</td><td>" + String(config->getIs12HFormat() ? "Sim" : "Não") + "</td></tr>";
    html += "<tr><td>Duração do Relógio</td><td>" + String(config->getClockDisplayDuration()) + " segundos</td></tr>";
    html += "<tr><td>Clima Ativado</td><td>" + String(config->getWeatherEnabled() ? "Sim" : "Não") + "</td></tr>";
    html += "<tr><td>Intervalo Atualização Clima</td><td>" + String(config->getWeatherUpdateInterval()) + " minutos</td></tr>";
    html += "<tr><td>API Key</td><td>" + String(config->getWeatherAPIKey().length() > 0 ? "Configurada" : "Não configurada") + "</td></tr>";
    html += "<tr><td>Coordenadas</td><td>Lat: " + config->getWeatherLat() + " Lon: " + config->getWeatherLon() + "</td></tr>";
  }
  html += "</table>";
  html += "</div>";
  
  // Seção 4: Dados Meteorológicos (se disponível)
  if (isWeatherAvailable()) {
    html += "<div class='section weather-section'>";
    html += "<h2>🌤️ Dados Meteorológicos Detalhados</h2>";
    
    // Informações Básicas
    html += "<div style='margin-bottom: 15px;'>";
    html += "<h3 style='background: linear-gradient(to right, #3498db, #2ecc71); color: white; padding: 8px 12px; border-radius: 5px;'>📍 Informações Básicas</h3>";
    html += "<table style='width: 100%; margin-bottom: 15px;'>";
    html += "<tr><td style='width: 40%; padding: 8px;'><strong>Cidade:</strong></td><td style='padding: 8px;' id='weather_city'>";
    html += weather->cityName.length() > 0 ? escapeHTML(weather->cityName) : "Não disponível";
    html += "</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Coordenadas:</strong></td><td style='padding: 8px;'>";
    html += "Lat: " + config->getWeatherLat() + " Lon: " + config->getWeatherLon();
    html += "</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Hora Local:</strong></td><td style='padding: 8px;'>";
    html += formatTimeForWeb();
    html += "</td></tr>";
    html += "</table>";
    html += "</div>";
    
    // Temperatura
    html += "<div style='margin-bottom: 15px;'>";
    html += "<h3 style='background: linear-gradient(to right, #3498db, #2ecc71); color: white; padding: 8px 12px; border-radius: 5px;'>🌡️ Temperatura</h3>";
    html += "<table style='width: 100%; margin-bottom: 15px;'>";
    html += "<tr><td style='width: 40%; padding: 8px;'><strong>Atual:</strong></td><td style='padding: 8px;' id='weather_temp'>";
    html += String(weather->currentTemp, 1) + "°C</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Sensação Térmica:</strong></td><td style='padding: 8px;' id='weather_feelsLike'>";
    html += String(weather->feelsLike, 1) + "°C</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Mínima:</strong></td><td style='padding: 8px;'>";
    html += String(weather->tempMin, 1) + "°C</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Máxima:</strong></td><td style='padding: 8px;'>";
    html += String(weather->tempMax, 1) + "°C</td></tr>";
    html += "</table>";
    html += "</div>";
    
    // Outros Parâmetros
    html += "<div style='margin-bottom: 15px;'>";
    html += "<h3 style='background: linear-gradient(to right, #3498db, #2ecc71); color: white; padding: 8px 12px; border-radius: 5px;'>📊 Outros Parâmetros</h3>";
    html += "<table style='width: 100%; margin-bottom: 15px;'>";
    html += "<tr><td style='width: 40%; padding: 8px;'><strong>Pressão Atmosférica:</strong></td><td style='padding: 8px;' id='weather_pressure'>";
    html += String(weather->pressure, 0) + " hPa</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Umidade:</strong></td><td style='padding: 8px;' id='weather_humidity'>";
    html += String(weather->humidity, 0) + "%</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Visibilidade:</strong></td><td style='padding: 8px;'>";
    if (weather->visibility >= 10000) {
      html += "10+ km";
    } else if (weather->visibility > 0) {
      html += String(weather->visibility / 1000.0, 1) + " km";
    } else {
      html += "Não disponível";
    }
    html += "</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Nuvens:</strong></td><td style='padding: 8px;'>";
    html += String(weather->clouds) + "%</td></tr>";
    html += "</table>";
    html += "</div>";
    
    // Vento
    html += "<div style='margin-bottom: 15px;'>";
    html += "<h3 style='background: linear-gradient(to right, #3498db, #2ecc71); color: white; padding: 8px 12px; border-radius: 5px;'>💨 Vento</h3>";
    html += "<table style='width: 100%; margin-bottom: 15px;'>";
    html += "<tr><td style='width: 40%; padding: 8px;'><strong>Velocidade:</strong></td><td style='padding: 8px;' id='weather_windSpeed'>";
    html += String(weather->windSpeed * 3.6, 1) + " km/h</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Direção:</strong></td><td style='padding: 8px;'>";
    String windDirection = weather->getWindDir(weather->windDeg);
    if (weather->windSpeed > 0.1) {
      html += windDirection + " (" + String(weather->windDeg) + "°)";
    } else {
      html += "Calmo";
    }
    html += "</td></tr>";
    html += "</table>";
    html += "</div>";
    
    // Informações Astronômicas
    html += "<div style='margin-bottom: 15px;'>";
    html += "<h3 style='background: linear-gradient(to right, #3498db, #2ecc71); color: white; padding: 8px 12px; border-radius: 5px;'>🌅 Informações Astronômicas</h3>";
    html += "<table style='width: 100%; margin-bottom: 15px;'>";
    html += "<tr><td style='width: 40%; padding: 8px;'><strong>Nascer do Sol:</strong></td><td style='padding: 8px;'>";
    html += time->timestampToLocalTime(weather->sunriseTime);
    html += "</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Pôr do Sol:</strong></td><td style='padding: 8px;'>";
    html += time->timestampToLocalTime(weather->sunsetTime);
    html += "</td></tr>";
    html += "<tr><td style='padding: 8px;'><strong>Condições:</strong></td><td style='padding: 8px;' id='weather_description'>";
    html += escapeHTML(weather->weatherDescription);
    html += "</td></tr>";
    html += "</table>";
    html += "</div>";
    
    // Última atualização
    html += "<div class='info' style='margin-top: 15px;'>";
    html += "<p><strong>⏱️ Última Atualização:</strong> ";
    html += String((millis() - weather->lastWeatherUpdate) / 60000, 0);
    html += " minutos atrás</p>";
    html += "</div>";
    
    html += "</div>";
  }
  
  // Seção 5: Links da API OpenWeatherMap (se disponível)
  if (isWeatherAvailable()) {
    html += "<div class='section'>";
    html += "<h2>🌐 Links da API OpenWeatherMap</h2>";
    
    String apiUrl = "https://api.openweathermap.org/data/2.5/weather?";
    apiUrl += "lat=" + config->getWeatherLat() + "&";
    apiUrl += "lon=" + config->getWeatherLon() + "&";
    apiUrl += "units=metric&";
    apiUrl += "appid=" + config->getWeatherAPIKey();
    
    html += "<p><strong>API Completa:</strong> ";
    html += "<a href='" + apiUrl + "' target='_blank' rel='noopener'>Abrir API</a></p>";
    
    html += "<p><strong>Visualizador JSON:</strong> ";
    html += "<a href='https://jsonviewer.stack.hu/' target='_blank' rel='noopener'>";
    html += "JSON Viewer Online</a> (cole a URL abaixo)</p>";
    
    html += "<div class='info' style='margin-top:10px;'>";
    html += "<strong>URL para teste:</strong><br>";
    html += "<input type='text' value='" + apiUrl + "' ";
    html += "style='width:100%;padding:5px;margin:5px 0;' ";
    html += "onclick='this.select()' readonly>";
    html += "<small>Clique para selecionar e copiar (Ctrl+C)</small>";
    html += "</div>";
    
    html += "</div>";
  }
  
  // Seção 6: Botões de Controle
  html += "<div class='section' style='text-align:center;'>";
  html += "<button class='btn' onclick='updateTime()'>🔄 Atualizar Agora</button>";
  html += "<button id='autoRefreshBtn' class='btn' onclick='toggleAutoRefresh()'>Desativar Auto-Refresh</button>";
  html += "<a href='/config' class='btn btn-success'>⚙️ Configurações</a>";
  html += "<a href='/ota' class='btn'>🔄 OTA</a>";
  html += "</div>";
  
  // Seção 7: Links Rápidos
  html += "<div class='section'>";
  html += "<h3>🌐 Acesso Rápido</h3>";
  html += "<div class='quick-links'>";
  html += "<div class='link-item'><strong>📊 Monitor</strong><br><a href='/'>/</a></div>";
  html += "<div class='link-item'><strong>⚙️ Configurações</strong><br><a href='/config'>/config</a></div>";
  html += "<div class='link-item'><strong>📡 Dados JSON</strong><br><a href='/data' target='_blank'>/data</a></div>";
  html += "<div class='link-item'><strong>🔄 Atualização OTA</strong><br><a href='/ota'>/ota</a></div>";
  html += "</div>";
  html += "</div>";
  
  // Seção 8: Rodapé
  html += "<div class='section' style='text-align:center;'>";
  html += "<p><small><strong>ESP8266 LED Matrix Clock v2.0</strong><br>";
  if (time) {
    html += String(dayNames[time->w]) + ", " + String(time->d) + " " + monthNames[time->mo-1] + " " + String(time->ye);
  }
  html += "</small></p>";
  html += "</div>";
  
  html += "</div>"; // container
  html += "</body>";
  html += "</html>";
  
  return html;
}

// NO WebInterface.cpp - Substituir a função getAdvancedConfigPageHTML()
String WebInterface::getAdvancedConfigPageHTML() {
  String html;
  html.reserve(6000);
  
  html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Configurações Avançadas - Relógio LED</title>";
  
  html += "<style>";
  html += "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; margin: 20px; background: #f0f0f0; color: #333; }";
  html += ".container { max-width: 900px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";
  html += "h1, h2, h3 { color: #2c3e50; margin-bottom: 15px; }";
  html += ".back-link { display: inline-flex; align-items: center; color: #3498db; text-decoration: none; font-weight: bold; margin-bottom: 20px; }";
  html += ".back-link:hover { text-decoration: underline; }";
  html += ".config-section { margin-bottom: 25px; padding: 15px; background: #ecf0f1; border-radius: 5px; }";
  
  html += ".form-group { margin-bottom: 15px; }";
  html += ".form-group label { display: block; margin-bottom: 5px; font-weight: 600; color: #2c3e50; }";
  html += ".form-group input, .form-group select { width: 100%; padding: 8px 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 14px; }";
  html += ".form-note { font-size: 0.85em; color: #666; margin-top: 5px; }";
  
  html += ".btn { background: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; font-size: 1em; text-decoration: none; display: inline-block; }";
  html += ".btn:hover { background: #2980b9; }";
  html += ".btn-save { background: #27ae60; }";
  html += ".btn-save:hover { background: #219653; }";
  html += ".btn-cancel { background: #6c757d; }";
  html += ".btn-cancel:hover { background: #5a6268; }";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 15px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; text-align: center; }";
  html += "}";
  html += "</style>";
  html += "</head>";
  
  html += "<body>";
  html += "<div class='container'>";
  html += "<a href='/' class='back-link'>← Voltar ao Monitor Principal</a>";
  html += "<h1>⚙️ Configurações Avançadas</h1>";
  
  html += "<form action='/saveadvancedconfig' method='POST'>";
  
  // Seção: Horário
  html += "<div class='config-section'>";
  html += "<h2>🕒 Configurações de Horário</h2>";
  
  html += "<div class='form-group'>";
  html += "<label for='utc'>Fuso Horário (UTC):</label>";
  html += "<input type='text' id='utc' name='utc' value='" + String(config->getUtcOffset(), 1) + "'>";
  html += "<div class='form-note'>Exemplo: -3.0 para Brasil, +1.0 para Europa</div>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='12h'>Formato 12h:</label>";
  html += "<select id='12h' name='12h'>";
  html += "<option value='true'" + String(config->getIs12HFormat() ? " selected" : "") + ">Sim</option>";
  html += "<option value='false'" + String(!config->getIs12HFormat() ? " selected" : "") + ">Não</option>";
  html += "</select>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='dst'>Horário de Verão (DST):</label>";
  html += "<select id='dst' name='dst'>";
  html += "<option value='true'" + String(config->observeDST ? " selected" : "") + ">Sim</option>";
  html += "<option value='false'" + String(!config->observeDST ? " selected" : "") + ">Não</option>";
  html += "</select>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='dstmode'>Modo DST:</label>";
  html += "<select id='dstmode' name='dstmode'>";
  html += "<option value='NONE'" + String(config->dstMode == 0 ? " selected" : "") + ">Nenhum</option>";
  html += "<option value='AUTO_US'" + String(config->dstMode == 2 ? " selected" : "") + ">EUA (AUTO_US)</option>";
  html += "<option value='AUTO_EU'" + String(config->dstMode == 1 ? " selected" : "") + ">Europa (AUTO_EU)</option>";
  html += "</select>";
  html += "<div class='form-note'>Selecionar automático para região</div>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='dstextra'>Horas Extra DST:</label>";
  html += "<input type='text' id='dstextra' name='dstextra' value='" + String(config->dstExtraHours, 1) + "'>";
  html += "<div class='form-note'>Normalmente 1.0 hora</div>";
  html += "</div>";
  html += "</div>"; // Fim da seção horário
  
  // Seção: Clima
  html += "<div class='config-section'>";
  html += "<h2>🌤️ Configurações de Clima</h2>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherenabled'>Clima Ativado:</label>";
  html += "<select id='weatherenabled' name='weatherenabled'>";
  html += "<option value='true'" + String(config->getWeatherEnabled() ? " selected" : "") + ">Sim</option>";
  html += "<option value='false'" + String(!config->getWeatherEnabled() ? " selected" : "") + ">Não</option>";
  html += "</select>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherapikey'>API Key OpenWeatherMap:</label>";
  html += "<input type='text' id='weatherapikey' name='weatherapikey' value='" + escapeHTML(config->getWeatherAPIKey()) + "'>";
  html += "<div class='form-note'>Obtenha em: openweathermap.org/api</div>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherlat'>Latitude:</label>";
  html += "<input type='text' id='weatherlat' name='weatherlat' value='" + escapeHTML(config->getWeatherLat()) + "'>";
  html += "<div class='form-note'>Exemplo: -19.9167</div>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherlon'>Longitude:</label>";
  html += "<input type='text' id='weatherlon' name='weatherlon' value='" + escapeHTML(config->getWeatherLon()) + "'>";
  html += "<div class='form-note'>Exemplo: -43.9345</div>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherlang'>Idioma:</label>";
  html += "<select id='weatherlang' name='weatherlang'>";
  html += "<option value='pt'" + String(config->getWeatherLang() == "&lang=pt" ? " selected" : "") + ">Português</option>";
  html += "<option value='en'" + String(config->getWeatherLang() == "&lang=en" ? " selected" : "") + ">Inglês</option>";
  html += "<option value='es'" + String(config->getWeatherLang() == "&lang=es" ? " selected" : "") + ">Espanhol</option>";
  html += "<option value='fr'" + String(config->getWeatherLang() == "&lang=fr" ? " selected" : "") + ">Francês</option>";
  html += "</select>";
  html += "</div>";
  
  html += "<div class='form-group'>";
  html += "<label for='weatherupdate'>Atualizar a cada (minutos):</label>";
  html += "<input type='number' id='weatherupdate' name='weatherupdate' value='" + String(config->getWeatherUpdateInterval()) + "' min='5' max='1440'>";
  html += "<div class='form-note'>Mínimo: 5, Máximo: 1440 (24h)</div>";
  html += "</div>";
  html += "</div>"; // Fim da seção clima
  
  // Seção: Exibição
  html += "<div class='config-section'>";
  html += "<h2>🖥️ Configurações de Exibição</h2>";
  
  html += "<div class='form-group'>";
  html += "<label for='clockduration'>Mostrar relógio por (segundos):</label>";
  html += "<input type='number' id='clockduration' name='clockduration' value='" + String(config->getClockDisplayDuration()) + "' min='10' max='300'>";
  html += "<div class='form-note'>Mínimo: 10, Máximo: 300</div>";
  html += "</div>";
  html += "</div>"; // Fim da seção exibição
  
  // Botões de ação
  html += "<div style='display: flex; gap: 10px; margin-top: 30px;'>";
  html += "<button type='submit' class='btn btn-save'>💾 Salvar Configurações</button>";
  html += "<a href='/' class='btn btn-cancel'>❌ Cancelar</a>";
  html += "</div>";
  
  html += "</form>";
  
  // Aviso de reinício
  html += "<div style='margin-top: 20px; padding: 15px; background: #fff3cd; border-radius: 5px; border-left: 4px solid #ffc107;'>";
  html += "<p><strong>⚠️ Atenção:</strong> Após salvar as configurações, o relógio será reiniciado automaticamente.</p>";
  html += "<p>O processo leva alguns segundos. Mantenha-se na mesma página.</p>";
  html += "</div>";
  
  html += "</div>"; // container
  html += "</body>";
  html += "</html>";
  
  return html;
}

String WebInterface::getSuccessPageHTML(const String& message) {
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='3;url=/'></head>";
  html += "<body><h1>✅ Sucesso!</h1>";
  if (message.length() > 0) {
    html += "<p>" + escapeHTML(message) + "</p>";
  }
  html += "<p>Redirecionando...</p>";
  html += "</body></html>";
  return html;
}
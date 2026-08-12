#include "WebServerManager.h"
#include "WebInterface.h"
#include "ConfigManager.h"
#include "TimeManager.h"
#include "WeatherManager.h"
#include "DisplayManager.h"
#include "GlobalDefines.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

extern DisplayMode currentMode;
extern unsigned long clockStartTime;

// ================== CONSTRUCTOR ==================

WebServerManager::WebServerManager() : server(80) {
  webInterface = nullptr;
  config = nullptr;
  time = nullptr;
  weather = nullptr;
  display = nullptr;
}

// ================== GETTERS/SETTERS ==================

ESP8266WebServer* WebServerManager::getServer() { 
  return &server; 
}

void WebServerManager::setWebInterface(WebInterface* interface) { 
  webInterface = interface; 
}

void WebServerManager::setConfigManager(ConfigManager* mgr) { 
  config = mgr; 
}

void WebServerManager::setTimeManager(TimeManager* mgr) { 
  time = mgr; 
}

void WebServerManager::setWeatherManager(WeatherManager* mgr) { 
  weather = mgr; 
}

void WebServerManager::setDisplayManager(DisplayManager* mgr) { 
  display = mgr; 
}

// ================== FUNÇÕES PRINCIPAIS ==================

void WebServerManager::begin() {
  // Configurar rotas principais
  server.on("/", std::bind(&WebServerManager::handleRoot, this));
  server.on("/data", std::bind(&WebServerManager::handleData, this));
  
  // Redirecionar /config para /advancedconfig
  server.on("/config", std::bind(&WebServerManager::handleRedirectToAdvanced, this));
  
  server.on("/advancedconfig", std::bind(&WebServerManager::handleAdvancedConfig, this));
  server.on("/saveadvancedconfig", HTTP_POST, std::bind(&WebServerManager::handleAdvancedUpdate, this));
  server.on("/restart", HTTP_POST, std::bind(&WebServerManager::handleRestart, this));
  server.on("/ota", std::bind(&WebServerManager::handleOTA, this));
  server.onNotFound(std::bind(&WebServerManager::handleNotFound, this));
  
  server.begin();
  Serial.println("✅ Servidor HTTP iniciado na porta 80");
}

void WebServerManager::handleClient() {
  server.handleClient();
}

// ================== NOVAS FUNÇÕES ADICIONADAS ==================

void WebServerManager::handleAdvancedConfig() {
  Serial.println("📡 Requisição recebida: /advancedconfig");
  if (webInterface) {
    server.send(200, "text/html", webInterface->getAdvancedConfigPageHTML());
  } else {
    server.send(500, "text/plain", "Web Interface não configurada");
  }
}

void WebServerManager::handleRedirectToAdvanced() {
  Serial.println("📡 Redirecionando /config para /advancedconfig");
  
  // Redirecionamento HTTP 302
  server.sendHeader("Location", "/advancedconfig");
  server.send(302, "text/plain", "Redirecionando para Configurações Avançadas...");
  
  // Ou mostrar uma mensagem amigável antes de redirecionar
  /*
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='2;url=/advancedconfig'>";
  html += "<title>Redirecionando...</title>";
  html += "</head><body>";
  html += "<h1>Redirecionando para Configurações Avançadas</h1>";
  html += "<p>Aguarde 2 segundos ou <a href='/advancedconfig'>clique aqui</a>.</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
  */
}

void WebServerManager::handleAdvancedUpdate() {
  Serial.println("📡 Requisição recebida: /saveadvancedconfig (POST)");
  
  if (config && server.args() > 0) {
    // Atualizar configurações de horário
    if (server.hasArg("utc")) config->utcOffset = server.arg("utc").toFloat();
    if (server.hasArg("12h")) config->is12HFormat = server.arg("12h") == "true";
    if (server.hasArg("dst")) config->observeDST = server.arg("dst") == "true";
    if (server.hasArg("dstmode")) {
      String mode = server.arg("dstmode");
      if (mode == "AUTO_US") config->dstMode = 2;
      else if (mode == "AUTO_EU") config->dstMode = 1;
      else config->dstMode = 0;
    }
    if (server.hasArg("dstextra")) config->dstExtraHours = server.arg("dstextra").toFloat();
    
    // Atualizar configurações de clima
    if (server.hasArg("weatherenabled")) config->weatherEnabled = server.arg("weatherenabled") == "true";
    if (server.hasArg("weatherapikey")) config->weatherAPIKey = server.arg("weatherapikey");
    if (server.hasArg("weatherlat")) config->weatherLat = server.arg("weatherlat");
    if (server.hasArg("weatherlon")) config->weatherLon = server.arg("weatherlon");
    if (server.hasArg("weatherlang")) config->weatherLang = "&lang=" + server.arg("weatherlang");
    if (server.hasArg("weatherupdate")) config->weatherUpdateInterval = server.arg("weatherupdate").toInt();
    
    // Atualizar configurações de exibição
    if (server.hasArg("clockduration")) config->clockDisplayDuration = server.arg("clockduration").toInt();
    
    // Validar valores
    if (config->clockDisplayDuration < 10) config->clockDisplayDuration = 10;
    if (config->clockDisplayDuration > 300) config->clockDisplayDuration = 300;
    if (config->weatherUpdateInterval < 5) config->weatherUpdateInterval = 5;
    if (config->weatherUpdateInterval > 1440) config->weatherUpdateInterval = 1440;
    
    // Salvar na EEPROM
    config->updateEEPROMStructure();
    config->saveSettings();
    
    // Enviar resposta mais elaborada
    String html = "<!DOCTYPE html>";
    html += "<html lang='pt-BR'>";
    html += "<head>";
    html += "<meta charset='UTF-8'>";
    html += "<title>Configurações Salvas - Relógio LED</title>";
    html += "<style>";
    html += "body { font-family: Arial, sans-serif; margin: 40px; background: #f0f0f0; }";
    html += ".container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }";
    html += ".success-icon { font-size: 60px; color: #27ae60; margin: 20px 0; }";
    html += ".btn { background: #3498db; color: white; padding: 10px 20px; border-radius: 5px; text-decoration: none; display: inline-block; margin: 10px; }";
    html += "</style>";
    html += "<meta http-equiv='refresh' content='5;url=/'></head>";
    html += "<body>";
    html += "<div class='container'>";
    html += "<div class='success-icon'>✅</div>";
    html += "<h1>Configurações Salvas com Sucesso!</h1>";
    html += "<p>As configurações foram salvas na memória EEPROM.</p>";
    html += "<p>O relógio será reiniciado em <strong>5 segundos</strong> para aplicar as mudanças.</p>";
    html += "<p>Você será redirecionado automaticamente para a página principal.</p>";
    html += "<div style='margin-top: 30px;'>";
    html += "<a href='/' class='btn'>🏠 Ir para a Página Principal</a>";
    html += "</div>";
    html += "</div>";
    html += "</body>";
    html += "</html>";
    
    server.send(200, "text/html", html);
    
    // Reiniciar após delay
    delay(3000);
    ESP.restart();
    
  } else {
    server.send(400, "text/plain", "Parâmetros inválidos");
  }
}

void WebServerManager::handleRestart() {
  Serial.println("🔄 Reinício solicitado via web");
  
  String html = "<!DOCTYPE html>";
  html += "<html><head><meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='3;url=/'></head>";
  html += "<body><h1>🔄 Reiniciando...</h1>";
  html += "<p>O relógio será reiniciado em 3 segundos.</p>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
  
  delay(1000);
  ESP.restart();
}

void WebServerManager::handleOTA() {
  // Serial.println("📡 Requisição recebida: /ota");
  
  String html;
  html.reserve(5000);
  
  html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Atualização OTA - Relógio LED</title>";
  
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
  html += ".btn-warning { background: #ffc107; color: #212529; }";
  html += ".btn-warning:hover { background: #e0a800; }";
  
  html += ".current-value { background: #fff3cd; padding: 10px; border-radius: 5px; margin: 10px 0; font-family: monospace; }";
  html += ".status-box { background: #d4edda; padding: 15px; border-radius: 5px; margin: 15px 0; border-left: 4px solid #28a745; }";
  html += ".warning-box { background: #fff3cd; border: 1px solid #ffeaa7; color: #856404; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  html += ".step-box { background: #e8f4f8; border-left: 4px solid #17a2b8; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  html += ".credential-box { background: #d4edda; border-left: 4px solid #28a745; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  
  html += "table { width: 100%; border-collapse: collapse; margin: 10px 0; }";
  html += "th { background: #3498db; color: white; text-align: left; padding: 10px; }";
  html += "td { padding: 8px 10px; border-bottom: 1px solid #ddd; }";
  html += "tr:nth-child(even) { background: #f9f9f9; }";
  
  html += "ol { margin-left: 20px; margin-top: 10px; }";
  html += "li { margin-bottom: 10px; }";
  
  html += ".quick-links { display: flex; flex-wrap: wrap; gap: 10px; margin: 15px 0; }";
  html += ".link-item { flex: 1; min-width: 150px; background: #ecf0f1; padding: 10px; border-radius: 5px; text-align: center; }";
  html += ".link-item a { color: #3498db; text-decoration: none; font-weight: bold; }";
  html += ".link-item a:hover { text-decoration: underline; }";
  
  html += ".form-group input[type='file'] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 16px; }";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 15px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; text-align: center; }";
  html += "  .quick-links { flex-direction: column; }";
  html += "  table { font-size: 0.9em; }";
  html += "  .link-item { min-width: auto; }";
  html += "}";
  html += "</style>";
  
  html += "<script>";
  html += "function startUpdate() {";
  html += "  const confirmed = confirm('⚠️ ATENÇÃO:\\n\\n' +";
  html += "    '• Não desconecte a alimentação durante a atualização\\n' +";
  html += "    '• O relógio reiniciará automaticamente após a atualização\\n' +";
  html += "    '• O processo pode levar 1-2 minutos\\n\\n' +";
  html += "    'Deseja continuar para a página de upload?');";
  html += "  if (confirmed) {";
  html += "    window.location.href = '/update';";
  html += "  }";
  html += "}";
  
  html += "function copyToClipboard(text) {";
  html += "  navigator.clipboard.writeText(text).then(function() {";
  html += "    alert('URL copiada para a área de transferência!');";
  html += "  });";
  html += "}";
  
  html += "function updateTime() {";
  html += "  const now = new Date();";
  html += "  const hours = now.getHours().toString().padStart(2, '0');";
  html += "  const minutes = now.getMinutes().toString().padStart(2, '0');";
  html += "  const seconds = now.getSeconds().toString().padStart(2, '0');";
  html += "  document.getElementById('currentTime').textContent = hours + ':' + minutes + ':' + seconds;";
  html += "}";
  
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  updateTime();";
  html += "  setInterval(updateTime, 1000);";
  html += "});";
  html += "</script>";
  html += "</head>";
  
  html += "<body>";
  html += "<div class='container'>";
  html += "<a href='/' class='back-link'>← Voltar ao Monitor Principal</a>";
  html += "<h1>🔄 Atualização OTA - Relógio LED</h1>";
  
  // Seção 1: Status do sistema
  html += "<div class='config-section'>";
  html += "<h2>📊 Status do Sistema</h2>";
  
  html += "<div class='current-value'>";
  html += "<strong>Hora do Servidor:</strong> <span id='currentTime'>--:--:--</span>";
  html += "</div>";
  
  html += "<table>";
  html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
  html += "<tr><td>IP do Relógio</td><td>" + WiFi.localIP().toString() + "</td></tr>";
  html += "<tr><td>SSID WiFi</td><td>" + WiFi.SSID() + "</td></tr>";
  html += "<tr><td>Sinal WiFi</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  html += "<tr><td>Status</td><td><span style='color: #28a745; font-weight: bold;'>● Online</span></td></tr>";
  html += "</table>";
  html += "</div>";
  
  // Seção 2: Aviso importante
  html += "<div class='config-section'>";
  html += "<h2>⚠️ AVISO IMPORTANTE</h2>";
  html += "<div class='warning-box'>";
  html += "<p><strong>LEIA COM ATENÇÃO ANTES DE PROCEDER:</strong></p>";
  html += "<ol style='margin-top: 10px;'>";
  html += "<li><strong>NÃO</strong> desconecte a alimentação durante a atualização</li>";
  html += "<li>Certifique-se de que o arquivo .bin é compatível com o hardware</li>";
  html += "<li>A atualização pode levar 1-2 minutos para completar</li>";
  html += "<li>O relógio reiniciará automaticamente após a atualização</li>";
  html += "<li>Mantenha-se na mesma rede WiFi durante o processo</li>";
  html += "</ol>";
  html += "</div>";
  html += "</div>";
  
  // Seção 3: Botão para upload
  html += "<div class='config-section'>";
  html += "<h2>📤 Upload do Firmware</h2>";
  
  html += "<div class='status-box'>";
  html += "<p><strong>Pronto para atualizar:</strong> Clique no botão abaixo para acessar a página de upload do firmware.</p>";
  html += "<p>Você será solicitado a fazer login com as credenciais:</p>";
  html += "<div style='margin: 10px 0; padding: 10px; background: white; border-radius: 4px;'>";
  html += "<strong>Usuário:</strong> admin<br>";
  html += "<strong>Senha:</strong> admin123";
  html += "</div>";
  html += "</div>";
  
  html += "<div style='text-align: center; margin-top: 20px;'>";
  html += "<button onclick='startUpdate()' class='btn btn-save'>";
  html += "🔧 Acessar Página de Upload";
  html += "</button>";
  html += "</div>";
  html += "</div>";
  
  // Seção 4: Instruções passo a passo
  html += "<div class='config-section'>";
  html += "<h2>📋 Instruções Passo a Passo</h2>";
  html += "<div class='step-box'>";
  html += "<ol>";
  html += "<li><strong>Preparação do Firmware:</strong>";
  html += "<ul style='margin-top: 5px; margin-bottom: 10px; padding-left: 20px;'>";
  html += "<li>Abra o código no Arduino IDE</li>";
  html += "<li>Vá em <strong>Sketch → Exportar Binário Compilado</strong></li>";
  html += "<li>O arquivo será salvo com extensão <strong>.bin</strong></li>";
  html += "</ul>";
  html += "</li>";
  html += "<li><strong>Acesso ao Upload:</strong> Clique no botão <strong>'Acessar Página de Upload'</strong> acima</li>";
  html += "<li><strong>Autenticação:</strong> Faça login com as credenciais fornecidas</li>";
  html += "<li><strong>Upload:</strong> Selecione o arquivo .bin e clique em <strong>Upload</strong></li>";
  html += "<li><strong>Aguarde:</strong> Acompanhe o progresso até a conclusão</li>";
  html += "</ol>";
  html += "</div>";
  html += "</div>";
  
  // Seção 5: Para Arduino IDE (OTA via rede)
  html += "<div class='config-section'>";
  html += "<h2>🔧 Para Arduino IDE</h2>";
  
  String otaUrl = "http://" + WiFi.localIP().toString() + ":8266";
  html += "<p><strong>URL para Arduino IDE:</strong></p>";
  html += "<div class='current-value'>";
  html += otaUrl;
  html += "</div>";
  html += "<p><small>No Arduino IDE: <strong>Sketch → Upload por Rede</strong> e cole a URL acima</small></p>";
  
  html += "<p><strong>Configurações:</strong></p>";
  html += "<table>";
  html += "<tr><th>Configuração</th><th>Valor</th></tr>";
  html += "<tr><td>Host/Porta</td><td>" + WiFi.localIP().toString() + ":8266</td></tr>";
  html += "<tr><td>Usuário</td><td>admin</td></tr>";
  html += "<tr><td>Senha</td><td>admin123</td></tr>";
  html += "</table>";
  
  html += "<button onclick=\"copyToClipboard('" + otaUrl + "')\" class='btn' style='margin-top: 10px;'>";
  html += "📋 Copiar URL";
  html += "</button>";
  html += "</div>";
  
  // Seção 6: Links Rápidos
  html += "<div class='config-section'>";
  html += "<h3>🌐 Acesso Rápido</h3>";
  html += "<div class='quick-links'>";
  html += "<div class='link-item'><strong>📊 Monitor</strong><br><a href='/'>/</a></div>";
 // html += "<div class='link-item'><strong>⚙️ Configurações</strong><br><a href='/config'>/config</a></div>";
  html += "<div class='link-item'><strong>📡 Dados JSON</strong><br><a href='/data'>/data</a></div>";
  html += "<div class='link-item'><strong>🔄 OTA Update</strong><br><a href='/ota'>/ota</a></div>";
  html += "</div>";
  html += "</div>";
  
  // Seção 7: Informações técnicas
  html += "<div class='config-section'>";
  html += "<h3>🔧 Informações Técnicas</h3>";
  html += "<div style='background: #f8f9fa; padding: 15px; border-radius: 5px; font-size: 0.9em; color: #666;'>";
  html += "<p><strong>Hardware:</strong> ESP8266 NodeMCU</p>";
  html += "<p><strong>Display:</strong> Matriz LED 8x32 MAX7219</p>";
  html += "<p><strong>Versão Atual:</strong> ESP8266 LED Matrix Clock v2.0</p>";
  html += "<p><strong>Protocolo:</strong> HTTP Update Server</p>";
  html += "</div>";
  html += "</div>";
  
  // Rodapé
  html += "<div style='text-align: center; margin-top: 30px; color: #666; font-size: 0.9em;'>";
  html += "<p><strong>ESP8266 LED Matrix Clock - Sistema OTA</strong></p>";
  html += "<p>Atualização de firmware via rede local</p>";
  html += "</div>";
  
  html += "</div>"; // container
  html += "</body>";
  html += "</html>";
  
  server.send(200, "text/html", html);
}


void WebServerManager::handleRoot() {
  // Serial.println("📡 Requisição recebida: /");
  if (webInterface) {
    server.send(200, "text/html", webInterface->getMainPageHTML());
  } else {
    server.send(500, "text/plain", "Web Interface não configurada");
  }
}

void WebServerManager::handleData() {
  // Serial.println("📡 Requisição recebida: /data");
  sendJSONData();
}

void WebServerManager::handleNotFound() {
  String uri = server.uri();
  
  // Redirecionar /config para /advancedconfig
  if (uri == "/config") {
    Serial.println("📡 Redirecionando /config para /advancedconfig");
    server.sendHeader("Location", "/advancedconfig");
    server.send(302, "text/plain", "Redirecionando...");
    return;
  }
  
  // Para outras URLs não encontradas, mostrar erro normal
  Serial.print("❌ Arquivo não encontrado: ");
  Serial.println(uri);
  
  String message = "Arquivo não encontrado\n\n";
  message += "URI: ";
  message += uri;
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}

void WebServerManager::sendJSONData() {
  if (!config || !time || !display || !webInterface) {
    server.send(500, "application/json", "{\"error\":\"Managers não configurados\"}");
    return; // Esta mensagem de erro é importante, mantenha
  }
  
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  
  // Dados básicos
  root["time"] = webInterface->formatTimeForWeb();
  root["infoString"] = time->infoString;
  root["displayMode"] = (currentMode == MODE_CLOCK ? "Relógio" : "Scroll Info");
  root["brightness"] = String(display->getCurrentBrightness()) + "/15";
  root["ip"] = WiFi.localIP().toString();
  root["wifiStatus"] = (WiFi.status() == WL_CONNECTED ? "Conectado" : "Desconectado");
  
  // Dados do display
  JsonArray& displayData = root.createNestedArray("displayData");
  uint8_t* screenBuffer = display->getScreenBuffer();
  for (int i = 0; i < 32; i++) {
    displayData.add(screenBuffer[i]);
  }
  
  // Dados meteorológicos (se disponíveis)
  if (weather && config->getWeatherEnabled() && config->getWeatherAPIKey().length() > 0) {
    JsonObject& weatherObj = root.createNestedObject("weather");
    weatherObj["city"] = weather->cityName;
    weatherObj["temp"] = String(weather->currentTemp, 1);
    weatherObj["feelsLike"] = String(weather->feelsLike, 1);
    weatherObj["tempMin"] = String(weather->tempMin, 1);
    weatherObj["tempMax"] = String(weather->tempMax, 1);
    weatherObj["humidity"] = String(weather->humidity, 0);
    weatherObj["pressure"] = String(weather->pressure, 0);
    weatherObj["windSpeed"] = String(weather->windSpeed * 3.6, 1);
    weatherObj["windDir"] = weather->getWindDir(weather->windDeg);
    weatherObj["clouds"] = String(weather->clouds);
    weatherObj["sunrise"] = time->timestampToLocalTime(weather->sunriseTime);
    weatherObj["sunset"] = time->timestampToLocalTime(weather->sunsetTime);
    weatherObj["description"] = weather->weatherDescription;
  }
  
  String jsonStr;
  root.printTo(jsonStr);
  server.send(200, "application/json", jsonStr);
}
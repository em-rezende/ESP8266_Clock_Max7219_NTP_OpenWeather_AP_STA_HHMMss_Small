/*
 * RELÓGIO ESP8266 com MATRIZ LED 8x32 NTP e CLIMA + SERVIDOR WEB
 * Versão com relógio com números pequenos: HH:MM:ss
 * Autor: Ezequiel M Rezende / Deepseek
 * 2026/01/02 - Início
 * 2026/01/28 - Servidor Web com display virtual 8x32
 * 2026/01/29 - Dados climáticos detalhados na web
 * 2026/01/29 - Correção display virtual
 * 2026/01/29 - Correção mensagem clima
 * 2026/02/08 - Modulação do código, Correção HTML e Display Virtual responsivo
 * 2026/02/10 - Correção página de configuração avançada
 */
#include "Arduino.h"
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

/* ================== INCLUDES ================== */
#include "pins_config.h"
#include "max7219_final.h"  // Deve vir antes de outros que usam CMD_INTENSITY
#include "new_fonts.h"
#include "GlobalDefines.h"
#include "ConfigManager.h"
#include "DisplayManager.h"
#include "TimeManager.h"
#include "WeatherManager.h"
#include "WebInterface.h"
#include "WebServerManager.h"
#include "OTAManager.h"

/* ================== CONFIGURAÇÕES OTA ================== */
const char* OTA_USERNAME = "admin";
const char* OTA_PASSWORD = "admin123";

/* ================== INSTÂNCIAS GLOBAIS ================== */
ConfigManager configManager;
DisplayManager displayManager;
TimeManager timeManager;
WeatherManager weatherManager;
WebInterface webInterface;
WebServerManager webServerManager;
OTAManager otaManager;
ESP8266HTTPUpdateServer httpUpdater;

/* ================== VARIÁVEIS GLOBAIS ================== */
WiFiManager wifiManager;
DisplayMode currentMode = MODE_CLOCK;
unsigned long clockStartTime = 0;

/* ================== CONTROLE DE MODO ================== */
void checkModeTransition() {
  unsigned long currentTime = millis();
  
  if (currentMode == MODE_CLOCK) {
    if (currentTime - clockStartTime >= (configManager.getClockDisplayDuration() * 1000UL)) {
      if (WiFi.status() == WL_CONNECTED) {
        timeManager.update();  // Tentar atualizar hora
        
        if (configManager.getWeatherEnabled() && 
            configManager.getWeatherAPIKey().length() > 0) {
          if (currentTime - weatherManager.lastWeatherUpdate >= 
              (configManager.getWeatherUpdateInterval() * 60000UL)) {
            weatherManager.updateWeatherData();
			timeManager.updateInfoString();
          }
        }
      }
      
      timeManager.updateInfoString();
      
      currentMode = MODE_INFO_SCROLL;
      displayManager.resetScroll();
    }
  }
  
  else if (currentMode == MODE_INFO_SCROLL) {
    if (displayManager.isScrollCompleted()) {
      currentMode = MODE_CLOCK;
      clockStartTime = currentTime;
    }
  }
}

/* ================== FUNÇÕES DE CALLBACK ================== */
void saveConfigCallback() {
  configManager.saveConfigCallback();
}

/* ================== SETUP ================== */
void setup() {
  Serial.begin(74880);
  delay(4000);
  
  Serial.println("\n\n==========================================");
  Serial.println("🚀 Relógio LED Matrix ESP8266 - Iniciando");
  Serial.println("==========================================");

  // Carregar configurações
  configManager.loadSettings();
  configManager.applySettingsFromEEPROM();
  configManager.updateWiFiManagerBuffers();

  // Inicializar display
  displayManager.init();
  displayManager.clr();
  displayManager.refreshAll();

  // Mostrar "BOOT"
  displayManager.showChar('B', 5);
  displayManager.showChar('O', 11);
  displayManager.showChar('O', 17);
  displayManager.showChar('T', 23);
  displayManager.refreshAll();
  delay(1000);
  displayManager.clr();
  displayManager.refreshAll();

  // Configurar WiFiManager
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.setConfigPortalTimeout(180);
  configManager.setupWiFiManagerParameters(wifiManager);

  pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
  delay(500);

  // Verificar modo configuração via botão
  bool configMode = displayManager.showCountdown();
  
  if (configMode) {
    Serial.println("⚙️ Modo Configuração ativado via botão");
    
    displayManager.showChar('C', 2);
    displayManager.showChar('O', 7);
    displayManager.showChar('N', 12);
    displayManager.showChar('F', 18);
    displayManager.showChar('I', 23);
    displayManager.showChar('G', 27);
    displayManager.refreshAll();
    delay(1000);

    WiFi.disconnect(true);
    delay(1000);

    Serial.println("📡 Iniciando Portal de Configuração WiFi...");
    Serial.println("🌐 Conecte-se à rede: 'Relogio NTP+Clima'");
    Serial.println("🌐 Acesse: 192.168.4.1 no navegador");

    wifiManager.startConfigPortal("Relogio NTP+Clima");

    displayManager.clr();
    displayManager.refreshAll();
    displayManager.displayIPScroll();
  } else {
    Serial.println("🤖 Modo Automático ativado");

    // Mostrar "AUTO"
    displayManager.showChar('A', 6);
    displayManager.showChar('U', 11);
    displayManager.showChar('T', 16);
    displayManager.showChar('O', 22);
    displayManager.refreshAll();
    delay(1000);
    displayManager.clr();

    // Tentar conexão automática
    if (!wifiManager.autoConnect("Relogio NTP+Clima")) {
      Serial.println("❌ Falha na conexão WiFi. Iniciando modo configuração...");
      
      // Mostrar "ERRO"
      displayManager.showChar('E', 5);
      displayManager.showChar('R', 11);
      displayManager.showChar('R', 17);
      displayManager.showChar('O', 23);
      displayManager.refreshAll();
      delay(2000);
      
      wifiManager.startConfigPortal("Relogio NTP+Clima");
      displayManager.clr();
      displayManager.refreshAll();
      displayManager.displayIPScroll();
    } else {
      Serial.println("✅ Conectado ao WiFi com sucesso!");
      displayManager.clr();
      displayManager.refreshAll();
      displayManager.displayIPScroll();
    }
  }

  // Iniciar cliente NTP
  timeManager.begin();
  
  // Mostrar "NTP" enquanto sincroniza
  displayManager.clr();
  displayManager.showChar('N', 5);
  displayManager.showChar('T', 11);
  displayManager.showChar('P', 17);
  displayManager.refreshAll();
  delay(1000);
  
  // Forçar sincronização NTP
  Serial.println("🔄 Sincronizando NTP...");
  if (timeManager.forceUpdate()) {
    Serial.println("✅ NTP sincronizado!");
    Serial.print("🕒 Hora atual: ");
    Serial.println(timeManager.formatTime());
  } else {
    Serial.println("❌ Falha na sincronização NTP");
  }

  // Atualizar dados climáticos (se configurado)
  if (configManager.getWeatherEnabled() && configManager.getWeatherAPIKey().length() > 0) {
    Serial.println("🌤️ Atualizando dados meteorológicos...");
    weatherManager.updateWeatherData();
    // ATUALIZAR A STRING DE INFORMAÇÃO APÓS O CLIMA
    timeManager.updateInfoString();
  } else {
    // Atualizar string de informações SEM clima
    timeManager.updateInfoString();
  }

  // Configurar brilho inicial
  displayManager.updateBrightness();

  // Iniciar contagem do relógio
  clockStartTime = millis();

  // Iniciar servidor web ANTES do OTA
  webServerManager.begin();

  // Conectar as dependências
  webInterface.setConfigManager(&configManager);
  webInterface.setTimeManager(&timeManager);
  webInterface.setWeatherManager(&weatherManager);
  webInterface.setDisplayManager(&displayManager);
  
  webServerManager.setWebInterface(&webInterface);
  webServerManager.setConfigManager(&configManager);
  webServerManager.setTimeManager(&timeManager);
  webServerManager.setWeatherManager(&weatherManager);
  webServerManager.setDisplayManager(&displayManager);

  // Configurar OTA Web
  httpUpdater.setup(webServerManager.getServer(), "/update", OTA_USERNAME, OTA_PASSWORD);
  Serial.println("✅ OTA Web configurado em /update");

  //Forçar MDNS no setup() principal
  if (WiFi.status() == WL_CONNECTED) {
    // Configurar MDNS MANUALMENTE
    Serial.print("🔧 Configurando MDNS... ");
    
    // Tentar com timeout
    unsigned long mdnsStart = millis();
    bool mdnsStarted = false;
    
    while (millis() - mdnsStart < 5000) {  // Tentar por 5 segundos
      if (MDNS.begin("relogio-led-esp8266")) {
        mdnsStarted = true;
        break;
      }
      delay(100);
    }
    
    if (mdnsStarted) {
      Serial.println("✅ OK");
      
      // Adicionar serviços explicitamente
      MDNS.addService("arduino", "tcp", 8266);
      MDNS.addService("http", "tcp", 80);
      
      Serial.println("📡 Serviços MDNS registrados:");
      Serial.println("   - Arduino OTA: porta 8266");
      Serial.println("   - Web Server: porta 80");
      Serial.println("   Nome: relogio-led-esp8266.local");
    } else {
      Serial.println("❌ FALHA");
      Serial.println("   Use IP diretamente: " + WiFi.localIP().toString());
    }
  }
  
  // Iniciar OTA Arduino (para atualização via Arduino IDE)
  otaManager.begin();
  
  // Mostrar status final
  Serial.println("\n==========================================");
  Serial.println("✅ SISTEMA INICIALIZADO COM SUCESSO!");
  Serial.println("==========================================");


  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("🌐 IP Conectado: ");
    Serial.println(WiFi.localIP());
    Serial.print("📶 SSID WiFi: ");
    Serial.println(WiFi.SSID());
    Serial.print("📡 Força do sinal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.println("==========================================");
    Serial.println("Acesse o monitor web:");
    Serial.print("👉 http://");
    Serial.println(WiFi.localIP());
    Serial.println("Acesse o OTA web:");
    Serial.print("👉 http://");
    Serial.print(WiFi.localIP());
    Serial.println("/update");
  } else {
    Serial.println("❌ WiFi não conectado");
  }
  Serial.println("==========================================");
}

/* ================== LOOP ================== */
void loop() {
  otaManager.handle();
  
  static unsigned long lastClockUpdate = 0;
  static unsigned long lastNTPCheck = 0;
  static unsigned long lastWiFiCheck = 0;
  
  // WiFi check silencioso
  if (millis() - lastWiFiCheck >= 30000) {
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      WiFi.reconnect();
    }
  }

  // NTP sync silencioso a cada 30 minutos
  if (WiFi.status() == WL_CONNECTED && millis() - lastNTPCheck >= 1800000) {
    lastNTPCheck = millis();
    timeManager.forceUpdate(); // SEM log interno
  }

  // Atualizar relógio
  if (millis() - lastClockUpdate >= 1000) {
    lastClockUpdate = millis();
    
    // Sempre usar incremento normal
    timeManager.s++;
    if (timeManager.s >= 60) {
      timeManager.s = 0;
      timeManager.m++;
      if (timeManager.m >= 60) {
        timeManager.m = 0;
        timeManager.h = (timeManager.h + 1) % 24;
      }
    }
    
    timeManager.dots = !timeManager.dots;
    displayManager.updateBrightness();
  }

  checkModeTransition();
  
  if (currentMode == MODE_CLOCK) {
    displayManager.showAnimClock();
  } else if (currentMode == MODE_INFO_SCROLL) {
    displayManager.displayInfoScroll();
  }

  webServerManager.handleClient();
  delay(10);
}

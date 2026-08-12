#include "OTAManager.h"
#include <ArduinoOTA.h>

OTAManager::OTAManager() {
  // Construtor vazio
}

void OTAManager::begin() {
  Serial.println("🔄 Inicializando OTA Arduino...");
  
  // Configurar hostname
  String hostname = "relogio-led-esp8266";
  ArduinoOTA.setHostname(hostname.c_str());
  
  // Configurar senha (opcional)
  ArduinoOTA.setPassword("admin123");
  
  // Configurar MDNS
  if (!MDNS.begin(hostname.c_str())) {
    Serial.println("❌ Erro ao iniciar MDNS");
  } else {
    Serial.println("✅ MDNS iniciado: " + hostname + ".local");
    // Adicionar serviço OTA ao MDNS
    MDNS.addService("arduino", "tcp", 8266);
  }
  
  // Configurar handlers
  ArduinoOTA.onStart([]() {
    Serial.println("\n🚀 Iniciando atualização OTA...");
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Tipo: " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\n✅ Atualização OTA concluída!");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("📥 Progresso: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("❌ Erro OTA [%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Falha na autenticação");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Falha ao iniciar");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Falha na conexão");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Falha no recebimento");
    else if (error == OTA_END_ERROR) Serial.println("Falha ao finalizar");
  });
  
  // Iniciar OTA
  ArduinoOTA.begin();
  
Serial.println("\n==========================================");
  Serial.println("✅ OTA ARDUINO IDE CONFIGURADO");
  Serial.println("==========================================");
  Serial.println("📡 PARA FAZER UPLOAD VIA REDE:");
  Serial.println("1. Arduino IDE → Sketch → Upload por Rede");
  Serial.println("2. Na janela que abrir:");
  Serial.println("   - Host: " + WiFi.localIP().toString());
  Serial.println("   - Port: 8266");
  Serial.println("   - Username: admin");
  Serial.println("   - Password: admin123");
  Serial.println("3. Clique em 'Upload'");
  Serial.println("");
  Serial.println("⚠️  Se 'relogio-led-esp8266.local' não aparecer");
  Serial.println("   nas portas de rede, use o IP acima!");
  Serial.println("==========================================");
}

void OTAManager::handle() {
  ArduinoOTA.handle();
}
#include "OTAWebInterface.h"
#include <ESP8266WiFi.h>

OTAWebInterface::OTAWebInterface() {
  // Construtor vazio
}

String OTAWebInterface::getOTAPageHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Atualização OTA - Relógio LED</title>";

  html += "<style>";
  html += "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; margin: 20px; background: #f0f0f0; color: #333; }";
  html += ".container { max-width: 100%; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }";
  html += "h1, h2, h3 { color: #2c3e50; margin-bottom: 15px; }";
  html += ".section { margin-bottom: 30px; padding: 15px; background: #ecf0f1; border-radius: 5px; }";
  html += ".status { display: inline-block; padding: 4px 10px; border-radius: 4px; font-size: 0.9em; margin: 5px 0; }";
  html += ".connected { background: #d4edda; color: #155724; }";
  html += ".disconnected { background: #f8d7da; color: #721c24; }";
  
  /* Display Virtual Responsivo */
  html += ".display-container { margin: 15px 0; }";
  html += ".grid-wrapper { overflow-x: auto; margin: 10px 0; }";
  html += ".grid { display: grid; grid-template-columns: repeat(32, 1fr); grid-template-rows: repeat(8, 1fr); gap: 1px; background: #111; padding: 8px; border-radius: 4px; min-width: 320px; }";
  html += ".pixel { aspect-ratio: 1/1; min-width: 8px; border-radius: 1px; transition: background 0.2s; }";
  html += ".pixel-on { background: #ff4444; box-shadow: 0 0 3px #ff4444; }";
  html += ".pixel-off { background: #222; border: 1px solid #444; }";
  html += ".display-info { background: #34495e; color: white; padding: 8px 12px; border-radius: 4px; margin-top: 10px; font-family: monospace; font-size: 0.9em; }";
  
  html += ".clock { font-size: 2.2em; font-family: 'Courier New', monospace; margin: 15px 0; text-align: center; color: #2c3e50; font-weight: bold; letter-spacing: 2px; }";
  html += ".info-box { background: #fff3cd; padding: 12px; border-radius: 6px; margin: 10px 0; border-left: 4px solid #ffc107; }";
  
  html += "table { width: 100%; border-collapse: collapse; margin: 10px 0; }";
  html += "th { background: #3498db; color: white; text-align: left; padding: 10px; }";
  html += "td { padding: 8px 10px; border-bottom: 1px solid #ddd; }";
  html += "tr:nth-child(even) { background: #f9f9f9; }";
  
  html += ".btn { background: #3498db; color: white; border: none; padding: 8px 16px; border-radius: 5px; cursor: pointer; font-size: 0.95em; margin: 5px; }";

  html += ".btn:hover { background: #2980b9; }";
  html += ".btn-success { background: #27ae60; }";
  html += ".btn-success:hover { background: #219653; }";
  html += ".btn-warning { background: #ffc107; color: #212529; }";
  html += ".btn-warning:hover { background: #e0a800; }";
  
  html += ".quick-links { display: flex; flex-wrap: wrap; gap: 10px; margin: 15px 0; }";
  html += ".link-item { flex: 1; min-width: 150px; background: #ecf0f1; padding: 10px; border-radius: 5px; text-align: center; }";
  html += ".link-item a { color: #3498db; text-decoration: none; font-weight: bold; }";
  html += ".link-item a:hover { text-decoration: underline; }";
  
  html += ".weather-section { background: #e8f4f8; border-left: 4px solid #2ecc71; }";
  html += ".weather-param { margin: 8px 0; padding: 8px; background: white; border-radius: 4px; }";
  
  html += ".warning-box { background: #fff3cd; border: 1px solid #ffeaa7; color: #856404; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  html += ".step-box { background: #e8f4f8; border-left: 4px solid #17a2b8; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  html += ".credential-box { background: #d4edda; border-left: 4px solid #28a745; padding: 15px; border-radius: 5px; margin: 15px 0; }";
  
  html += ".form-group { margin-bottom: 20px; }";
  html += ".form-group label { display: block; margin-bottom: 8px; font-weight: 600; color: #2c3e50; }";
  html += ".form-group input[type='file'] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 16px; }";
  html += ".form-group input[type='text'] { width: 100%; padding: 8px 10px; border: 1px solid #ddd; border-radius: 4px; font-size: 14px; }";
  
  html += "ol { margin-left: 20px; margin-top: 10px; }";
  html += "li { margin-bottom: 10px; line-height: 1.5; }";
  
  html += ".code-box { background: #2c3e50; color: white; padding: 10px; border-radius: 4px; font-family: 'Courier New', monospace; margin: 10px 0; overflow-x: auto; }";
  
  html += ".status-indicator { display: inline-block; width: 12px; height: 12px; border-radius: 50%; margin-right: 8px; }";
  html += ".status-online { background: #28a745; box-shadow: 0 0 5px #28a745; }";
  
//  html += "@media (min-width: 1200px) {";
//  html += "  .container { max-width: 1200px; margin: 20px auto; }";
//  html += "}";
  
//  html += "@media (max-width: 768px) {";
//  html += "  .container { padding: 15px; margin: 10px; }";
//  html += "  .grid { min-width: 280px; }";
//  html += "  .pixel { min-width: 6px; }";
//  html += "  .clock { font-size: 1.8em; }";
//  html += "  .quick-links { flex-direction: column; }";
//  html += "  table { font-size: 0.9em; }";
//  html += "  .btn { width: 100%; margin: 5px 0; text-align: center; }";
//  html += "  .link-item { min-width: auto; }";
//  html += "}";
//  html += "</style>";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 15px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; text-align: center; }";
  html += "}";
  html += "</style>";
 

  html += "<script>";
  html += "function updateTime() {";
  html += "  const now = new Date();";
  html += "  const hours = now.getHours().toString().padStart(2, '0');";
  html += "  const minutes = now.getMinutes().toString().padStart(2, '0');";
  html += "  const seconds = now.getSeconds().toString().padStart(2, '0');";
  html += "  document.getElementById('currentTime').textContent = hours + ':' + minutes + ':' + seconds;";
  html += "}";
  
  html += "function showFileName(input) {";
  html += "  const fileName = input.files[0] ? input.files[0].name : 'Nenhum arquivo selecionado';";
  html += "  document.getElementById('fileName').textContent = fileName;";
  html += "}";
  
  html += "function startUpload() {";
  html += "  const fileInput = document.getElementById('firmwareFile');";
  html += "  if (!fileInput.files[0]) {";
  html += "    alert('Por favor, selecione um arquivo .bin primeiro');";
  html += "    return false;";
  html += "  }";
  html += "  if (!fileInput.files[0].name.endsWith('.bin')) {";
  html += "    alert('O arquivo deve ter extensão .bin');";
  html += "    return false;";
  html += "  }";
  html += "  const confirmed = confirm('⚠️ ATENÇÃO:\\n\\n' +";
  html += "    '• Não desconecte a alimentação durante a atualização\\n' +";
  html += "    '• O relógio reiniciará automaticamente após a atualização\\n' +";
  html += "    '• O processo pode levar 1-2 minutos\\n\\n' +";
  html += "    'Deseja continuar?');";
  html += "  if (!confirmed) return false;";
  html += "  document.getElementById('uploadForm').submit();";
  html += "  document.getElementById('uploadBtn').disabled = true;";
  html += "  document.getElementById('uploadBtn').innerHTML = '📤 Upload em andamento...';";
  html += "  return true;";
  html += "}";
  
  html += "function copyToClipboard(text) {";
  html += "  navigator.clipboard.writeText(text).then(function() {";
  html += "    alert('URL copiada para a área de transferência!');";
  html += "  });";
  html += "}";
  
  html += "document.addEventListener('DOMContentLoaded', function() {";
  html += "  updateTime();";
  html += "  setInterval(updateTime, 1000);";
  html += "});";
  html += "</script>";
  html += "</head>";
  
  html += "<body>";
  html += "<div class='container'>";
  html += "<h1>🔄 Atualização OTA - Relógio LED</h1>";
  
  // Seção 1: Status do sistema
  html += "<div class='section'>";
  html += "<h2>📊 Status do Sistema</h2>";
  html += "<div class='info-box' style='text-align: center;'>";
  html += "<div class='clock' id='currentTime'>--:--:--</div>";
  html += "</div>";
  
  html += "<table>";
  html += "<tr><th>Parâmetro</th><th>Valor</th></tr>";
  html += "<tr><td>IP do Relógio</td><td>" + WiFi.localIP().toString() + "</td></tr>";
  html += "<tr><td>SSID WiFi</td><td>" + WiFi.SSID() + "</td></tr>";
  html += "<tr><td>Sinal WiFi</td><td>" + String(WiFi.RSSI()) + " dBm</td></tr>";
  html += "<tr><td>Status</td><td><span class='status connected'>● Online</span></td></tr>";
  html += "</table>";
  html += "</div>";
  
  // Seção 2: Aviso importante
  html += "<div class='section'>";
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
  
  // Seção 3: Formulário de upload
  html += "<div class='section'>";
  html += "<h2>📤 Upload do Firmware</h2>";
  
  html += "<form id='uploadForm' action='/update' method='POST' enctype='multipart/form-data'>";
  
  html += "<div class='form-group'>";
  html += "<label for='firmwareFile'>Arquivo de Firmware (.bin):</label>";
  html += "<input type='file' id='firmwareFile' name='update' accept='.bin' onchange='showFileName(this)'>";
  html += "<div style='margin-top: 10px;'>";
  html += "<strong>Arquivo selecionado:</strong> <span id='fileName'>Nenhum arquivo selecionado</span>";
  html += "</div>";
  html += "</div>";
  
  html += "<div class='credential-box'>";
  html += "<strong>🔐 Credenciais de Acesso:</strong>";
  html += "<table style='margin-top: 10px;'>";
  html += "<tr><td><strong>Usuário:</strong></td><td>admin</td></tr>";
  html += "<tr><td><strong>Senha:</strong></td><td>admin123</td></tr>";
  html += "</table>";
  html += "<p style='margin-top: 10px; font-size: 0.9em;'><em>Utilize estas credenciais na página de upload</em></p>";
  html += "</div>";
  
  html += "<div style='text-align: center; margin-top: 20px;'>";
  html += "<button type='button' onclick='startUpload()' class='btn btn-success' id='uploadBtn'>";
  html += "📤 Iniciar Upload";
  html += "</button>";
  html += "<a href='/' class='btn'>Cancelar</a>";
  html += "</div>";
  
  html += "</form>";
  html += "</div>";
  
  // Seção 4: Instruções passo a passo
  html += "<div class='section'>";
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
  html += "<li><strong>Acesso ao Upload:</strong> Clique no botão <strong>'Iniciar Upload'</strong> acima</li>";
  html += "<li><strong>Autenticação:</strong> Faça login com as credenciais fornecidas</li>";
  html += "<li><strong>Upload:</strong> Selecione o arquivo .bin e clique em <strong>Upload</strong></li>";
  html += "<li><strong>Aguarde:</strong> Acompanhe o progresso até a conclusão</li>";
  html += "</ol>";
  html += "</div>";
  html += "</div>";
  
  // Seção 5: Para Arduino IDE (OTA via rede)
  html += "<div class='section'>";
  html += "<h2>🔧 Para Arduino IDE</h2>";
  
  String otaUrl = "http://" + WiFi.localIP().toString() + ":8266";
  html += "<p><strong>URL para Arduino IDE:</strong></p>";
  html += "<div class='code-box'>";
  html += otaUrl;
  html += "</div>";
  html += "<p><small>No Arduino IDE: <strong>Sketch → Upload por Rede</strong> e cole a URL acima</small></p>";
  
  html += "<p><strong>Configurações:</strong></p>";
  html += "<table>";
  html += "<tr><th>Configuração</th><th>Valor</th></tr>";
  html += "<tr><td>Host/Porta:</td><td>" + WiFi.localIP().toString() + ":8266</td></tr>";
  html += "<tr><td>Usuário:</td><td>admin</td></tr>";
  html += "<tr><td>Senha:</td><td>admin123</td></tr>";
  html += "</table>";
  
  html += "<button onclick=\"copyToClipboard('" + otaUrl + "')\" class='btn' style='margin-top: 10px;'>";
  html += "📋 Copiar URL";
  html += "</button>";
  html += "</div>";
  
  // Seção 6: Links Rápidos
  html += "<div class='section'>";
  html += "<h3>🌐 Acesso Rápido</h3>";
  html += "<div class='quick-links'>";
  html += "<div class='link-item'><strong>📊 Monitor</strong><br><a href='/'>/</a></div>";
  html += "<div class='link-item'><strong>⚙️ Configurações</strong><br><a href='/config'>/config</a></div>";
  html += "<div class='link-item'><strong>📡 Dados JSON</strong><br><a href='/data'>/data</a></div>";
  html += "<div class='link-item'><strong>🔄 OTA Update</strong><br><a href='/ota'>/ota</a></div>";
  html += "</div>";
  html += "</div>";
  
  // Seção 7: Informações técnicas
  html += "<div class='section'>";
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
  
  return html;
}

String OTAWebInterface::getUpdateSuccessHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Atualização Concluída - Relógio LED</title>";
  
  html += "<style>";
  html += "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; margin: 20px; background: #f0f0f0; color: #333; }";

  html += ".container { max-width: 900px; margin: 0 auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }";

  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 15px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; text-align: center; }";
  html += "  .quick-links { flex-direction: column; }";
  html += "  table { font-size: 0.9em; }";
  html += "}";


  html += "h1, h2 { color: #2c3e50; margin-bottom: 15px; }";
  html += ".success-icon { font-size: 80px; color: #27ae60; margin: 20px 0; }";
  
  html += ".btn { background: #3498db; color: white; border: none; padding: 8px 16px; border-radius: 5px; cursor: pointer; font-size: 0.95em; margin: 5px; }";
  
  html += ".btn:hover { background: #2980b9; }";
  html += ".btn-success { background: #27ae60; }";
  html += ".btn-success:hover { background: #219653; }";
  
  html += ".info-box { background: #e8f4f8; padding: 20px; border-radius: 5px; margin: 20px 0; border-left: 4px solid #3498db; text-align: left; }";
  
  html += ".countdown { font-size: 24px; font-weight: bold; color: #3498db; margin: 10px 0; }";
  
  html += "@media (min-width: 1200px) {";
  html += "  .container { max-width: 1200px; margin: 20px auto; }";
  html += "}";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 20px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; }";
  html += "}";
  html += "</style>";
  
  html += "<script>";
  html += "function startCountdown() {";
  html += "  let seconds = 10;";
  html += "  const countdownElement = document.getElementById('countdown');";
  html += "  const timer = setInterval(function() {";
  html += "    countdownElement.textContent = seconds;";
  html += "    seconds--;";
  html += "    if (seconds < 0) {";
  html += "      clearInterval(timer);";
  html += "      window.location.href = '/';";
  html += "    }";
  html += "  }, 1000);";
  html += "}";
  html += "document.addEventListener('DOMContentLoaded', startCountdown);";
  html += "</script>";
  
  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<div class='success-icon'>✅</div>";
  html += "<h1>✅ Atualização Concluída!</h1>";
  
  html += "<div class='info-box'>";
  html += "<p><strong>Firmware atualizado com sucesso!</strong></p>";
  html += "<p>O novo firmware foi instalado corretamente no dispositivo.</p>";
  html += "<p>O relógio irá reiniciar automaticamente em <span class='countdown' id='countdown'>10</span> segundos.</p>";
  html += "<p><small>Após o reinício, o dispositivo estará executando a nova versão do firmware.</small></p>";
  html += "</div>";
  
  html += "<div style='margin-top: 30px;'>";
  html += "<a href='/' class='btn btn-success'>🏠 Ir para o Monitor Principal</a>";
  html += "<a href='/ota' class='btn'>🔄 Nova Atualização</a>";
  html += "</div>";
  
  html += "</div>";
  html += "</body>";
  html += "</html>";
  
  return html;
}

String OTAWebInterface::getUpdateErrorHTML(const String& error) {
  String html = "<!DOCTYPE html>";
  html += "<html lang='pt-BR'>";
  html += "<head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Erro na Atualização - Relógio LED</title>";
  
  html += "<style>";
  html += "@import url('https://fonts.googleapis.com/css2?family=Inter:wght@300;400;600;700&display=swap');";
  html += "* { box-sizing: border-box; margin: 0; padding: 0; }";
  html += "body { font-family: 'Inter', 'Segoe UI', system-ui, sans-serif; margin: 20px; background: #f0f0f0; color: #333; }";
  html += ".container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); text-align: center; }";
  html += "h1, h2 { color: #2c3e50; margin-bottom: 15px; }";
  html += ".error-icon { font-size: 80px; color: #e74c3c; margin: 20px 0; }";
  
  html += ".btn { background: #3498db; color: white; border: none; padding: 8px 16px; border-radius: 5px; cursor: pointer; font-size: 0.95em; margin: 5px; }";
  
  html += ".btn:hover { background: #2980b9; }";
  html += ".btn-danger { background: #e74c3c; }";
  html += ".btn-danger:hover { background: #c0392b; }";
  
  html += ".error-box { background: #f8d7da; color: #721c24; padding: 20px; border-radius: 5px; margin: 20px 0; border: 1px solid #f5c6cb; text-align: left; font-family: monospace; font-size: 0.9em; word-break: break-all; }";
  
  html += ".warning-box { background: #fff3cd; color: #856404; padding: 15px; border-radius: 5px; margin: 20px 0; border: 1px solid #ffeaa7; }";
  
  html += "@media (min-width: 1200px) {";
  html += "  .container { max-width: 1200px; margin: 20px auto; }";
  html += "}";
  
  html += "@media (max-width: 768px) {";
  html += "  .container { padding: 20px; margin: 10px; }";
  html += "  .btn { width: 100%; margin: 5px 0; }";
  html += "}";
  html += "</style>";
  
  html += "</head>";
  html += "<body>";
  html += "<div class='container'>";
  html += "<div class='error-icon'>❌</div>";
  html += "<h1>❌ Erro na Atualização</h1>";
  
  html += "<div class='warning-box'>";
  html += "<p><strong>⚠️ A atualização falhou!</strong></p>";
  html += "<p>O dispositivo não foi atualizado e continua com o firmware anterior.</p>";
  html += "</div>";
  
  html += "<div class='error-box'>";
  html += "<p><strong>Mensagem de erro:</strong></p>";
  html += "<p>" + error + "</p>";
  html += "</div>";
  
  html += "<div style='text-align: left; margin: 20px 0; padding: 15px; background: #f8f9fa; border-radius: 5px;'>";
  html += "<p><strong>Possíveis causas:</strong></p>";
  html += "<ul style='margin-left: 20px; margin-top: 10px;'>";
  html += "<li>Arquivo .bin incompatível com o hardware</li>";
  html += "<li>Falha na conexão de rede durante o upload</li>";
  html += "<li>Arquivo corrompido ou inválido</li>";
  html += "<li>Espaço insuficiente na memória do ESP8266</li>";
  html += "<li>Versão de firmware incorreta para este dispositivo</li>";
  html += "</ul>";
  html += "</div>";
  
  html += "<div style='margin-top: 30px;'>";
  html += "<a href='/ota' class='btn btn-danger'>🔄 Tentar Novamente</a>";
  html += "<a href='/' class='btn'>🏠 Voltar ao Principal</a>";
  html += "</div>";
  
  html += "</div>";
  html += "</body>";
  html += "</html>";
  
  return html;
}
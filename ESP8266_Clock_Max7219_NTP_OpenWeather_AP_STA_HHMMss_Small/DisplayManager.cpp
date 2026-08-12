#include "DisplayManager.h"
#include "max7219_final.h"
#include "ConfigManager.h"
#include "TimeManager.h"
#include "WeatherManager.h"
#include "GlobalDefines.h"
#include <Arduino.h>
#include "pins_config.h"

extern ConfigManager configManager;
extern TimeManager timeManager;
extern WeatherManager weatherManager;
extern const uint8_t* textFont;

DisplayManager::DisplayManager() {
  // Inicializar variáveis
  currentBrightness = 8;
  lastBrightnessCheck = 0;
  scrollPos = 0;
  scrollCompleted = false;
  lastScrollTime = 0;
  
  // Inicializar arrays
  for (int i = 0; i < 6; i++) {
    dig[i] = 0;
    digold[i] = 10;
    digtrans[i] = 0;
  }
  
  // Configuração de brilho padrão
  brightnessConfig.night = 1;
  brightnessConfig.dawn = 3;
  brightnessConfig.morning = 5;
  brightnessConfig.day = 8;
  brightnessConfig.afternoon = 6;
  brightnessConfig.evening = 4;
}

void DisplayManager::init() {
  initMAX7219();
  clr();
  refreshAll();
}

void DisplayManager::clr() {
  for (int i = 0; i < 32; i++) {
    scr[i] = 0;
  }
}

void DisplayManager::refreshAll() {
  ::refreshAll(); // Chama a função global de max7219_final.cpp
}

void DisplayManager::showDigit(int digit, int col, const uint8_t* font) {
  if (digit < 0 || digit > 9) return;

  int bytesPerChar = pgm_read_byte(font);
  int startPos = 1 + (digit * bytesPerChar);
  int width = pgm_read_byte(font + startPos);

  for (int i = 0; i < width; i++) {
    if (col + i < 32) {
      scr[col + i] = pgm_read_byte(font + startPos + 1 + i);
    }
  }
}

void DisplayManager::showChar(char c, int col) {
  if (c < 32 || c > 255) return;
  if (!textFont) return;

  int bytesPerChar = pgm_read_byte(textFont);
  int fontIndex = c - 32;
  
  if (fontIndex < 0) return;
  
  int startPos = 1 + (fontIndex * bytesPerChar);
  int width = pgm_read_byte(textFont + startPos);

  // Limpar área
  for (int i = 0; i < 6; i++) {
    if (col + i < 32) {
      scr[col + i] = 0;
    }
  }

  // Desenhar caractere
  for (int i = 0; i < width; i++) {
    if (col + i < 32) {
      scr[col + i] = pgm_read_byte(textFont + startPos + 1 + i);
    }
  }
}

void DisplayManager::setCol(int col, uint8_t v) {
  if (col >= 0 && col < 32) {
    scr[col] = v;
  }
}

BrightnessPeriod DisplayManager::getCurrentPeriod() {
  int currentHour = timeManager.h;
  
  if (currentHour >= 22 || currentHour < 6) {
    return PERIOD_NIGHT;      // 22:00 - 05:59
  } else if (currentHour >= 6 && currentHour < 7) {
    return PERIOD_DAWN;       // 06:00 - 06:59
  } else if (currentHour >= 7 && currentHour < 11) {
    return PERIOD_MORNING;    // 07:00 - 10:59
  } else if (currentHour >= 11 && currentHour < 16) {
    return PERIOD_DAY;        // 11:00 - 15:59
  } else if (currentHour >= 16 && currentHour < 18) {
    return PERIOD_AFTERNOON;  // 16:00 - 17:59
  } else if (currentHour >= 18 && currentHour < 22) {
    return PERIOD_EVENING;    // 18:00 - 21:59
  }
  
  return PERIOD_DAY;  // Default
}

uint8_t DisplayManager::getBrightnessForPeriod(BrightnessPeriod period) {
  switch (period) {
    case PERIOD_NIGHT:
      return brightnessConfig.night;
    case PERIOD_DAWN:
      return brightnessConfig.dawn;
    case PERIOD_MORNING:
      return brightnessConfig.morning;
    case PERIOD_DAY:
      return brightnessConfig.day;
    case PERIOD_AFTERNOON:
      return brightnessConfig.afternoon;
    case PERIOD_EVENING:
      return brightnessConfig.evening;
    default:
      return brightnessConfig.day;
  }
}

void DisplayManager::updateBrightness() {
  static BrightnessPeriod lastPeriod = PERIOD_ALWAYS_ON;
  static unsigned long lastUpdate = 0;
  
  unsigned long currentTime = millis();
  
  if (currentTime - lastUpdate >= 60000 || 
      currentTime - lastBrightnessCheck >= 1000) {
    
    lastBrightnessCheck = currentTime;
    BrightnessPeriod currentPeriod = getCurrentPeriod();
    
    if (currentPeriod != lastPeriod) {
      uint8_t newBrightness = getBrightnessForPeriod(currentPeriod);
      
      if (newBrightness != currentBrightness) {
        currentBrightness = newBrightness;
        sendCmdAll(CMD_INTENSITY, currentBrightness);
      }
      
      lastPeriod = currentPeriod;
      lastUpdate = currentTime;
    }
  }
}

bool DisplayManager::showCountdown() {
  // Configurar brilho máximo para contagem
  sendCmdAll(CMD_INTENSITY, 15);
  
  unsigned long countdownStart = millis();
  bool buttonPressed = false;
  int lastSecondShown = 10;  // Começa em 10 segundos
  
  const uint8_t* countdownFont = dig4x8;  // Usar fonte dos números grandes
  
  // Mostrar "10" inicial
  clr();
  showDigit(1, 10, countdownFont);  // Dígito 1
  showDigit(0, 16, countdownFont);  // Dígito 0
  refreshAll();
  
  // Iniciar contagem regressiva
  while (millis() - countdownStart < (10 * 1000UL)) {  // 10 segundos total
    unsigned long elapsed = millis() - countdownStart;
    int secondsRemaining = 10 - (elapsed / 1000);
    
    // Atualizar display apenas quando o segundo mudar
    if (secondsRemaining != lastSecondShown && secondsRemaining >= 0) {
      lastSecondShown = secondsRemaining;
      
      clr();  // Limpar tela
      
      if (secondsRemaining >= 10) {
        // Mostrar dois dígitos (10)
        showDigit(secondsRemaining / 10, 10, countdownFont);
        showDigit(secondsRemaining % 10, 16, countdownFont);
      } else if (secondsRemaining >= 0) {
        // Mostrar um dígito (0-9) centralizado
        showDigit(secondsRemaining, 13, countdownFont);
      }
      
      refreshAll();
      
      // Feedback no Serial Monitor
      Serial.print("Countdown: ");
      Serial.println(secondsRemaining);
    }
    
    // Verificar botão de configuração
    if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
      delay(50);  // Debounce
      if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
        buttonPressed = true;
        
        // Mostrar "CFG" no display
        clr();
        showChar('C', 5);
        showChar('F', 11);
        showChar('G', 17);
        refreshAll();
        delay(500);
        
        break;
      }
    }
    
    delay(10);  // Pequeno delay para evitar uso excessivo da CPU
  }
  
  // Limpar display ao final
  clr();
  refreshAll();
  
  // Restaurar brilho normal
  updateBrightness();
  
  return buttonPressed;
}

void DisplayManager::showAnimClock() {
  byte digPos[6] = { 1, 6, 13, 18, 25, 29 };
  int animFrames = 8;
  int num = 6;

  static unsigned long lastUpdate = 0;

  unsigned long now = millis();

  if (now - lastUpdate >= 40) {
    lastUpdate = now;

    int displayHour = timeManager.h;
    if (configManager.getIs12HFormat()) {
      displayHour %= 12;
      if (displayHour == 0) displayHour = 12;
    }

    int currentDigits[6] = {
      displayHour / 10 ? displayHour / 10 : 10,
      displayHour % 10,
      timeManager.m / 10,
      timeManager.m % 10,
      timeManager.s / 10,
      timeManager.s % 10
    };

    // Atualizar dígitos se mudaram
    for (int i = 0; i < num; i++) {
      if (currentDigits[i] != dig[i]) {
        digold[i] = dig[i];
        dig[i] = currentDigits[i];
        digtrans[i] = animFrames;
      }
    }

    clr();

    for (int i = 0; i < num; i++) {
      const uint8_t* font = (i >= 4) ? dig3x5 : dig4x8;

      if (digtrans[i] == 0) {
        showDigit(dig[i], digPos[i], font);
      } else {
        int progress = animFrames - digtrans[i];

        if (digold[i] >= 0 && digold[i] <= 9) {
          int startPosOld = 1 + (digold[i] * pgm_read_byte(font));
          int widthOld = pgm_read_byte(font + startPosOld);

          for (int col = 0; col < widthOld; col++) {
            int displayCol = digPos[i] + col;
            if (displayCol < 32) {
              byte data = pgm_read_byte(font + startPosOld + 1 + col);
              data >>= progress;
              scr[displayCol] |= data;
            }
          }
        }

        int startPosNew = 1 + (dig[i] * pgm_read_byte(font));
        int widthNew = pgm_read_byte(font + startPosNew);

        for (int col = 0; col < widthNew; col++) {
          int displayCol = digPos[i] + col;
          if (displayCol < 32) {
            byte data = pgm_read_byte(font + startPosNew + 1 + col);
            data <<= (animFrames - 1 - progress);
            scr[displayCol] |= data;
          }
        }

        digtrans[i]--;
      }
    }

    // Adicionar dois pontos (:) entre horas e minutos
    setCol(11, timeManager.dots ? 0b00100100 : 0);
    setCol(23, timeManager.dots ? 0b00100100 : 0);

    refreshAll();
  }
}

void DisplayManager::displayInfoScroll() {
  // Usar a infoString atualizada do TimeManager
  String displayText = "   " + timeManager.infoString + "   ";
  
  if (millis() - lastScrollTime > 80) {
    lastScrollTime = millis();

    clr();

    int totalWidth = 0;
    for (int i = 0; i < displayText.length(); i++) {
      char c = displayText[i];
      if (c >= 32 && c <= 255) {
        int fontIndex = c - 32;
        int startPos = 1 + (fontIndex * pgm_read_byte(textFont));
        totalWidth += pgm_read_byte(textFont + startPos) + 1;
      }
    }
    
    int maxScroll = totalWidth + 32;
    int startX = -scrollPos;

    int currentX = startX;
    for (int i = 0; i < displayText.length(); i++) {
      char c = displayText[i];
      if (c >= 32 && c <= 255) {
        int fontIndex = c - 32;
        int startPos = 1 + (fontIndex * pgm_read_byte(textFont));
        int width = pgm_read_byte(textFont + startPos);
        
        for (int col = 0; col < width; col++) {
          int screenCol = currentX + col;
          if (screenCol >= 0 && screenCol < 32) {
            scr[screenCol] = pgm_read_byte(textFont + startPos + 1 + col);
          }
        }
        currentX += width + 1;
      }
    }

    refreshAll();
    scrollPos += 2;

    if (scrollPos >= maxScroll) {
      scrollCompleted = true;
    }
  }
}

void DisplayManager::displayIPScroll(int scrollSpeed, int scrollStep) {
  sendCmdAll(CMD_INTENSITY, 8);

  String ipText = "       Conectado IP: " + WiFi.localIP().toString() + "   ";
  int ipScrollPos = 0;
  bool completed = false;
  unsigned long lastIPScrollTime = 0;

  while (!completed) {
    unsigned long currentTime = millis();

    if (currentTime - lastIPScrollTime > scrollSpeed) {
      lastIPScrollTime = currentTime;

      clr();

      int textWidth = 0;
      for (int i = 0; i < ipText.length(); i++) {
        char c = ipText[i];
        if (c >= 32 && c <= 255) {
          int fontIndex = c - 32;
          int startPos = 1 + (fontIndex * pgm_read_byte(textFont));
          textWidth += pgm_read_byte(textFont + startPos) + 1;
        }
      }
      
      int maxScroll = textWidth + 32;
      int startX = -ipScrollPos;

      int currentX = startX;
      for (int i = 0; i < ipText.length(); i++) {
        char c = ipText[i];
        if (c >= 32 && c <= 255) {
          int fontIndex = c - 32;
          int startPos = 1 + (fontIndex * pgm_read_byte(textFont));
          int width = pgm_read_byte(textFont + startPos);
          
          for (int col = 0; col < width; col++) {
            int screenCol = currentX + col;
            if (screenCol >= 0 && screenCol < 32) {
              scr[screenCol] = pgm_read_byte(textFont + startPos + 1 + col);
            }
          }
          currentX += width + 1;
        }
      }

      refreshAll();
      ipScrollPos += scrollStep;

      if (ipScrollPos >= maxScroll) {
        completed = true;
      }
    }
    delay(10);
  }

  delay(500);
  clr();
  refreshAll();
  updateBrightness();
}

void DisplayManager::resetScroll() {
  scrollPos = 0;
  scrollCompleted = false;
  lastScrollTime = 0;
}

bool DisplayManager::isScrollCompleted() {
  return scrollCompleted;
}

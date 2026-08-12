#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include "Arduino.h"
#include "pins_config.h"
#include "GlobalDefines.h"
#include "new_fonts.h"
#include "max7219_final.h" 

class DisplayManager {
public:
  DisplayManager();
  
  // Inicialização
  void init();
  void clr();
  void refreshAll();
  void sendCmd(uint8_t cmd, uint8_t data);
  
  // Exibição
  void showDigit(int digit, int col, const uint8_t* font);
  void showChar(char c, int col);
  void setCol(int col, uint8_t v);
  
  // Brilho
  void updateBrightness();
  BrightnessPeriod getCurrentPeriod();
  uint8_t getBrightnessForPeriod(BrightnessPeriod period);
  
  // Animações
  bool showCountdown();
  void showAnimClock();
  
  // Scroll da mensagem "Conectado IP"
  // Muito lento/suave: scrollSpeed = 120, scrollStep = 1
  // Normal (default): scrollSpeed = 80, scrollStep = 1
  // Rápido: scrollSpeed = 50, scrollStep = 2
  void displayInfoScroll();
  void displayIPScroll(int scrollSpeed = 60, int scrollStep = 1);
  
  // Controle de scroll
  void resetScroll();
  bool isScrollCompleted();
  
  // Getters
  uint8_t* getScreenBuffer() { return scr; }
  uint8_t getCurrentBrightness() { return currentBrightness; }
  BrightnessConfig* getBrightnessConfig() { return &brightnessConfig; }
  
  // Setters
  void setCurrentBrightness(uint8_t brightness) { 
    currentBrightness = brightness; 
    sendCmd(CMD_INTENSITY, currentBrightness);
  }
  
  // Variáveis de animação
  int dig[6];
  int digold[6];
  int digtrans[6];
  
  // Configuração de brilho
  BrightnessConfig brightnessConfig;
  
private:
  unsigned long lastBrightnessCheck;
  static const unsigned long BRIGHTNESS_CHECK_INTERVAL = 60000;
  
  uint8_t currentBrightness;
  
  // Controle de scroll
  int scrollPos;
  bool scrollCompleted;
  unsigned long lastScrollTime;
};

extern DisplayManager displayManager;

#endif
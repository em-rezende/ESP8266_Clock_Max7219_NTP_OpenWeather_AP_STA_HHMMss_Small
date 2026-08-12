#ifndef GLOBALDEFINES_H
#define GLOBALDEFINES_H

#include <Arduino.h>
#include "pins_config.h"

// Array com nomes dos dias abreviados
extern const char* dayNames[];
extern const char* monthNames[];

// Variáveis do display
extern uint8_t scr[NUM_MAX * 8];

// Ponteiro para fonte de texto
extern const uint8_t* textFont;

// Enum para modo de exibição
enum DisplayMode {
  MODE_CLOCK,
  MODE_INFO_SCROLL
};

// Períodos do dia para controle de brilho
enum BrightnessPeriod {
  PERIOD_NIGHT,      // 22:00 - 05:59
  PERIOD_DAWN,       // 06:00 - 06:59
  PERIOD_MORNING,    // 07:00 - 10:59
  PERIOD_DAY,        // 11:00 - 15:59
  PERIOD_AFTERNOON,  // 16:00 - 17:59
  PERIOD_EVENING,    // 18:00 - 21:59
  PERIOD_ALWAYS_ON   // Sempre ligado (para testes)
};

// Configurações de brilho padrão (0-15)
struct BrightnessConfig {
  uint8_t night;      // 22:00 - 05:59
  uint8_t dawn;       // 06:00 - 06:59
  uint8_t morning;    // 07:00 - 10:59
  uint8_t day;        // 11:00 - 15:59
  uint8_t afternoon;  // 16:00 - 17:59
  uint8_t evening;    // 18:00 - 21:59
  
  // Construtor para inicialização padrão
  BrightnessConfig() : night(1), dawn(3), morning(5), day(8), afternoon(6), evening(4) {}
};

#endif

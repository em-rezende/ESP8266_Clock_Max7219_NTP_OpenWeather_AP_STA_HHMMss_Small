#include "GlobalDefines.h"
#include "new_fonts.h"
#include "pins_config.h"  // <-- Adicione para ter NUM_MAX

// Array com nomes dos dias abreviados
const char* dayNames[] = {
  "Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"
};

// Array com nomes dos meses abreviados
const char* monthNames[] = {
  "jan", "fev", "mar", "abr", "mai", "jun",
  "jul", "ago", "set", "out", "nov", "dez"
};

// Buffer do display - tamanho NUM_MAX * 8
// uint8_t scr[NUM_MAX * 8] = {0};  // <-- Especifique o tamanho
uint8_t scr[32] = {0};  // 32 bytes = 32 colunas x 8 bits

// Definir qual fonte usar para texto geral (não-relógio)
const uint8_t* textFont = font_emr;

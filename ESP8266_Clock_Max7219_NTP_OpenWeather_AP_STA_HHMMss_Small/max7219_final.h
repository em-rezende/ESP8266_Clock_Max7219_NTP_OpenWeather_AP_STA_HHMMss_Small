#ifndef MAX7219_FINAL_H
#define MAX7219_FINAL_H

#include <Arduino.h>
#include "pins_config.h"

// MAX7219 commands:
#define CMD_NOOP 0
#define CMD_DIGIT0 1
#define CMD_DIGIT1 2
#define CMD_DIGIT2 3
#define CMD_DIGIT3 4
#define CMD_DIGIT4 5
#define CMD_DIGIT5 6
#define CMD_DIGIT6 7
#define CMD_DIGIT7 8
#define CMD_DECODEMODE 9
#define CMD_INTENSITY 10
#define CMD_SCANLIMIT 11
#define CMD_SHUTDOWN 12
#define CMD_DISPLAYTEST 15

// Defina ROTATE conforme seu display
#define ROTATE 90

// Declaração das funções
void sendCmd(int addr, byte cmd, byte data);
void sendCmdAll(byte cmd, byte data);
void refresh(int addr);
void refreshAll();
void clr();
void scrollLeft();
void invert();
void initMAX7219();

#endif

#ifndef PINS_CONFIG_H
#define PINS_CONFIG_H

/* ================== PINOS ================== */
#define NUM_MAX 4

// PINOS PARA ESP-01S
// #define DIN_PIN 2            // GPIO2
// #define CS_PIN 3             // GPIO3/RX
// #define CLK_PIN 0            // GPIO0
// #define CONFIG_BUTTON_PIN 1  // GPIO1/TX

// PINOS PARA ESP8266 NodeMcu ESP-12E
#define CLK_PIN 12  // D6
#define CS_PIN  13  // D7
#define DIN_PIN 15  // D8
#define CONFIG_BUTTON_PIN 2  // GPIO2/D4

/* ================== DEFINIÇÕES ================== */
#define DEFAULT_CLOCK_DURATION 30
#define SCROLL_SPEED 80
#define SCROLL_STEP 2
#define COUNTDOWN_DURATION 10

#endif
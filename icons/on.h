#include <Arduino.h>

#define on_width 12
#define on_height 12

const PROGMEM uint8_t on_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0xF0, 0x00, 0xF8, 0x01, 0xFC, 0x03, 0xFC, 0x03,
  0xFC, 0x03, 0xFC, 0x03, 0xF8, 0x01, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
};
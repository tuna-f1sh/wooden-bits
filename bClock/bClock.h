#ifndef HBCLOCK
#define HBCLOCK

#include <Arduino.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6
// matrix diamentions
#define PIXEL_COLUMN   8
#define PIXEL_ROW      8

#ifndef true
  #define true 1
  #define false 0
#endif

void pixelTime(tmElements_t tm, byte *bTime);
void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait);
void setMatrix(byte bMatrix[], size_t size, uint32_t color1, uint32_t color2);
void initMatrixMap(uint16_t map[PIXEL_ROW][PIXEL_COLUMN], uint8_t rotate);
void solidColor(uint32_t r,uint32_t g,uint32_t b);
void setPixel(uint8_t pixel, uint8_t set, uint32_t c);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

#endif

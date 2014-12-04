#ifndef HBCLOCK
#define HBCLOCK

#include <Arduino.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6
// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      16
// matrix diamentions
#define PIXEL_COLUMN   4
#define PIXEL_ROW      4

void pixelTime(tmElements_t tm, byte *bTime, uint8_t size);
void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait);
void setMatrix(byte bMatrix[PIXEL_COLUMN], uint8_t colIndex[PIXEL_COLUMN], uint32_t color);
void solidColor(uint16_t r,uint16_t g,uint16_t b);
void setPixel(uint8_t pixel, uint8_t set, uint32_t c);
void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

#endif

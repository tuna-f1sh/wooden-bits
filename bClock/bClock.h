#ifndef HBCLOCK
#define HBCLOCK

#include <Arduino.h>

#define VERSION_MINOR  8
#define VERSION_MAJOR  1

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6
// matrix diamentions
#define PIXEL_COLUMN   4
#define PIXEL_ROW      4
// RTC chip type
#define CHIP_DS3232RTC
// #define CHIP_DS1307RTC

#ifndef true
  #define true 1
  #define false 0
#endif

// converts tm struct to binary nybles
void pixelTime(tmElements_t tm, byte *bTime);
// quarter hour row indication
void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait);
// set nybles to matrix - color1 is bottom left cell, color2 rest of cell (only used on larger than 4x4)
void setMatrix(byte bMatrix[], size_t size, uint32_t color1, uint32_t color2);
// fills lookup map for matrix [x][y]
void initMatrixMap(uint16_t map[PIXEL_ROW][PIXEL_COLUMN], uint8_t rotate);
// set time function
void setBClock(tmElements_t &tm, byte *bTime, uint8_t size);
// set matrix to solid colour
void solidColor(uint32_t r,uint32_t g,uint32_t b);
// set single pixel in matrix (pixel can be map refer eg. map[1][1])
void setPixel(uint8_t pixel, uint8_t set, uint32_t c);
// wipe across matrix in wiring formation
void colorWipe(uint32_t c, uint8_t wait);
// rainbow colour display (adafruit)
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(byte WheelPos);

#endif

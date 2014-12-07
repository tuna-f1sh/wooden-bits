#ifndef HBCLOCK
#define HBCLOCK

#include "ws2812-RPi.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <signal.h>

#ifndef true
  #define true 1
  #define false 0
#endif

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      64
// matrix diamentions
#define PIXEL_COLUMN   8
#define PIXEL_ROW      8

void loop(void);
void pixelTime(struct tm *tm, uint8_t *bTime);
void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait);
void setMatrix(uint8_t bMatrix[], size_t size, Color_t color1, Color_t color2);
void solidColor(uint32_t r,uint32_t g,uint32_t b);
void setPixel(uint8_t pixel, uint8_t set, Color_t c);
void unicorn_exit(int status);
void initMatrixMap(uint16_t map[PIXEL_ROW][PIXEL_COLUMN], uint8_t rotate);

#endif

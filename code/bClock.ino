#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <Adafruit_NeoPixel.h>
#include "bClock.h"

// delay for startup wipe
#define WIPE_DELAY     50
// delay for quarter hour display
#define QUARTER_WAIT   10000
// pulse at second intervals
const bool pulse_second = 0;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  /* Serial.begin(9600);*/
  /* while (!Serial) ; // wait for serial*/
  pixels.begin(); // This initializes the NeoPixel library.

  // wipe through rgb for LED debug
  colorWipe(pixels.Color(255,0,0),WIPE_DELAY);
  colorWipe(pixels.Color(0,255,0),WIPE_DELAY);
  colorWipe(pixels.Color(0,0,255),WIPE_DELAY);
  colorWipe(pixels.Color(0,0,0),WIPE_DELAY);
}

void loop() {
  tmElements_t tm; // time struct holder
  // snakes and ladders for column pixel index, the index refers to right most column
  uint8_t colIndex[PIXEL_COLUMN] = {0,7,8,15};
  // nyble vector for matrix columns
  // [hour/10, hour/1, minute/10, minute/1]
  byte bTime[PIXEL_COLUMN] = {B0000};

  // get the time from RTC
  if (RTC.read(tm)) {
  } else {
    // rtc isn't reading
    if (RTC.chipPresent()) {
      /* Serial.println("The DS1307 is stopped.  Please run the SetTime");*/
      /* Serial.println("example to initialize the time and begin running.");*/
      /* Serial.println();*/
      solidColor(255,165,0); // all orange
    } else {
      /* Serial.println("DS1307 read error!  Please check the circuitry.");*/
      /* Serial.println();*/
      solidColor(255,0,0); // all red
    }
    // error loop: wait 9s then try again
    delay(9000);
    return;
  }

  // convert the time to nybles for the matrix
  pixelTime(tm, bTime, PIXEL_COLUMN);

  // debug
  /* Serial.println("Hour Digit 1:");*/
  /* Serial.println(bTime[3],BIN);*/
  /* Serial.println("Hour Digit 2:");*/
  /* Serial.println(bTime[2],BIN);*/
  /* Serial.println("Minute Digit 1:");*/
  /* Serial.println(bTime[1],BIN);*/
  /* Serial.println("Minute Digit 2:");*/
  /* Serial.println(bTime[0],BIN);*/

  // quarter hour indicator
  if ( (tm.Minute % 15 == 0) && tm.Second == 0) {
    quarterHour(tm.Hour, tm.Minute, QUARTER_WAIT);
  }
  // turn them all off for second indicator flash
  if (pulse_second) {
    solidColor(0,0,0);
    delay(10);
  }
  // set the matrix to the binary time
  setMatrix(bTime, colIndex, pixels.Color(255,255,255));
  // wait a second before checking the time again
  delay(1000);

}

void pixelTime(tmElements_t tm, byte *bTime, uint8_t size) {
  // convert the hour and minute into uint8
  uint8_t hour = tm.Hour;
  uint8_t minute = tm.Minute;

  // convert the time into 4 nybles for each column of matrix
  if (hour >=0 && hour < 10) {
    bTime[1] = hour | B0000;
    bTime[0] = B0000; // 2nd digit still needs clearing
  } else {
    bTime[1] = uint8_t (hour-(floor(hour/10)*10)) | B0000;
    bTime[0] = uint8_t ((hour-(hour-floor(hour)))/10) | B0000;
  }
  if (minute >=0 && minute < 10) {
    bTime[3] = minute | B0000;
    bTime[2] = B0000; // 2nd digit still needs clearing
  } else {
    bTime[3] = uint8_t (minute-(floor(minute/10)*10)) | B0000;
    bTime[2] = uint8_t ((minute-(minute-floor(minute)))/10) | B0000;
  }
}

void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait) {
  int8_t no_rows;
  // light 4,8,12 or 16 pixels depending on the hour quarter (floored to be safe)
  no_rows = (PIXEL_ROW * floor(minute / 15)) - 1;
  // one hour indicator is actually 0 but we want full display
  no_rows = (no_rows == -1) ? 15 : no_rows;

  solidColor(0,0,0); // turn them all off
  pixels.show();

  if ((hour == 12 || hour == 24 ) && minute == 0) {
    rainbowCycle(25); // special edition hour for midday
  } else { // fade columns in and out
    for (int16_t j=0;j<=255;j+=5) {
      for (uint8_t i=0;i<=no_rows;i++) {
        setPixel(i,1,pixels.Color(0,0,j));
      }
      pixels.show();
      delay(10);
    }
    delay(wait);
    for (int16_t j=255;j>=0;j-=5) {
      for (uint8_t i=0;i<=no_rows;i++) {
        setPixel(i,1,pixels.Color(0,0,j));
      }
      pixels.show();
      delay(10);
    }
  }
}

void setMatrix(byte bMatrix[PIXEL_COLUMN], uint8_t colIndex[PIXEL_COLUMN], uint32_t color) {
  int8_t b;

  for (uint8_t i=0;i <= PIXEL_COLUMN - 1;i++) {
    for (uint8_t x=0;x <= PIXEL_COLUMN - 1;x++) {
    /* for (uint8_t x=0;x <= 3;x++) {*/
      b = ((x % 2) == 0) ? 1 : -1; // snakes and ladders pixel arrangement alternates between taking and adding one
      setPixel(colIndex[x]+(i*b),((bMatrix[i] >> x) & 1),color);
    }
  }

  pixels.show();
}

// set the matrix to a solid colour
void solidColor(uint16_t r,uint16_t g,uint16_t b) {
  for (uint16_t i=0;i<NUMPIXELS;i++) {
    pixels.setPixelColor(i, pixels.Color(r,g,b));
  }
  pixels.show();
}

// set the pixel on or off
void setPixel(uint8_t pixel, uint8_t set, uint32_t c) {
  if (set) {
    pixels.setPixelColor(pixel, c);
  } else {
    pixels.setPixelColor(pixel, pixels.Color(0,0,0));
  }
}

// Fill the dots one after the other with a color (Adafruit)
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<NUMPIXELS; i++) {
      pixels.setPixelColor(i, c);
      pixels.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel((i+j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< pixels.numPixels(); i++) {
      pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
    }
    pixels.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

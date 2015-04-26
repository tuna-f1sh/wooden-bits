/*
===================================================================================================

01000010 01101001 01101110 01100001 01110010 01111001  01000011 01101100 01101111 01100011 01101011
______ _                          _____ _            _    
| ___ (_)                        /  __ \ |          | |   
| |_/ /_ _ __   __ _ _ __ _   _  | /  \/ | ___   ___| | __
| ___ \ | '_ \ / _` | '__| | | | | |   | |/ _ \ / __| |/ /
| |_/ / | | | | (_| | |  | |_| | | \__/\ | (_) | (__|   < 
\____/|_|_| |_|\__,_|_|   \__, |  \____/_|\___/ \___|_|\_\
                           __/ |                          
                          |___/                           

 Binary Clock
 Originally made for Arduino controller in my 'Wooden Bits' project
 John Whittington @j_whittington http://www.jbrengineering.co.uk 2014

 Compile using the Arduino IDE or using the Makefile. Using the makefile reqruies 'Arduino.mk':
 https://github.com/sudar/Arduino-Makefile
 "
 Set the number of pixels in each row and column in 'bClock.h'. The clock will automatically scale
 the binary bits to the pixel grid - thanks to the pixelMap generation.
 
 You can defined a flash on seconds 'pulse_second', rotated display 'rotate' and brightness

 Arduino Libraries - Adafruit NeoPixel: https://github.com/adafruit/Adafruit_NeoPixel
                     DS1307RTC RTC: https://www.pjrc.com/teensy/td_libs_DS1307RTC.html

 References - Adafruit NeoPixel library: https://github.com/adafruit/Adafruit_NeoPixel

===================================================================================================
*/

#include <SPI.h>
#include <Wire.h>
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_BLE_UART.h>
#include "bClock.h"
#ifdef CHIP_DS1307RTC
  #include <DS1307RTC.h>
  #define RTC_RETURN 1
#endif
#ifdef CHIP_DS3232RTC
  #include <DS3232RTC.h>
  #define RTC_RETURN 0
#endif

 /* ---- CONFIGURE ---- */
/*==========================*/

// delay for startup wipe
#define WIPE_DELAY     50
// delay for quarter hour display
#define QUARTER_WAIT   10000
// number of pixels (row and column are defined in header)
#define NUMPIXELS PIXEL_ROW*PIXEL_COLUMN
// Width and height of square to show bit
const static uint8_t width = PIXEL_ROW/4;
const static uint8_t height = PIXEL_COLUMN/4;
// Cordinate map of pixel init (gets filled by initMatrixMap)
uint16_t pixelMap [PIXEL_ROW][PIXEL_COLUMN] = {{0}};
// pulse at second intervals
const static uint8_t pulse_second = false;
// rotate the grid 90deg?
const static uint8_t rotate = false;
// brightness (0 darkest (off) - 255 retina searing)
const static uint8_t brightness = 255;
// set time flag from ISR
uint8_t flag = false;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
// Adafruit BLE
Adafruit_BLE_UART bluart = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);

/**************************************************************************/
/*!
    This function is called whenever select ACI events happen
*/
/**************************************************************************/
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
    case ACI_EVT_DEVICE_STARTED:
      Serial.println(F("Advertising started"));
      break;
    case ACI_EVT_CONNECTED:
      Serial.println(F("Connected!"));
      break;
    case ACI_EVT_DISCONNECTED:
      Serial.println(F("Disconnected or advertising timed out"));
      break;
    default:
      break;
  }
}

/**************************************************************************/
/*!
    This function is called whenever data arrives on the RX channel
*/
/**************************************************************************/
void rxCallback(uint8_t *buffer, uint8_t len)
{
  uint8_t h1,h2,m1,m2;
  uint16_t temp, colour;
  byte bTemp[4] = {B0000};
  tmElements_t tm; // time struct holder

  Serial.print(F("Received "));
  Serial.print(len);
  Serial.print(F(" bytes: "));
  for(int i=0; i<len; i++)
   Serial.print((char)buffer[i]); 

  Serial.print(F(" ["));

  for(int i=0; i<len; i++)
  {
    Serial.print(" 0x"); Serial.print((char)buffer[i], HEX); 
  }
  Serial.println(F(" ]"));

  for(int i=1; i<len; i++)
  {
    if ('0' <= buffer[i] &&  buffer[i] <= '9') {
      ;
    } else {
      Serial.println("Invalid time, should be format hhmm");
      return;
    }
  }

  h1 = (buffer[1] - 0x30) * 10;
  h2 = (buffer[2] - 0x30);
  m1 = (buffer[3] - 0x30) * 10;
  m2 = (buffer[4] - 0x30);
  tm.Hour = h1 + h2;
  tm.Minute = m1 + m2;
  tm.Second = 0;

  switch (buffer[0]) {
    case 's':
      RTC.write(tm);
      pixelTime(tm, bTemp);
      setMatrix(bTemp, sizeof(bTemp)/sizeof(bTemp[1]), pixels.Color(0,255,0), pixels.Color(255,255,255));
      delay(1000);
      Serial.println("Time set!");
      break;
    case 'a':
      RTC.setAlarm(ALM2_MATCH_HOURS, (m1+m2), (h1+h2), 0);
      pixelTime(tm, bTemp);
      setMatrix(bTemp, sizeof(bTemp)/sizeof(bTemp[1]), pixels.Color(255,140,0), pixels.Color(255,255,255));
      delay(1000);
      Serial.println("Alarm set!");
      break;
    case 't':
      temp = (RTC.temperature() / 4);
      Serial.print(temp);Serial.println("degC");
      tm.Minute = temp;
      tm.Hour = 0;
      colour = map(temp, 0, 30, 85, 0);
      pixelTime(tm, bTemp);
      setMatrix(bTemp, sizeof(bTemp)/sizeof(bTemp[1]), Wheel(colour), pixels.Color(255,255,255));
      delay(10000);
      break;
    default:
      Serial.println("Invalid setting!");
  }
}

/* ---- SETUP ---- */
/*=====================*/

void setup() {
  attachInterrupt(0, setISR, RISING);

  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  bluart.setRXcallback(rxCallback);
  bluart.setACIcallback(aciCallback);
  bluart.setDeviceName("WoodBit"); /* 7 characters max! */
  bluart.begin();
  pixels.begin(); // This initializes the NeoPixel library.

  // Create the matrix lookup map
  initMatrixMap(pixelMap,rotate);

  pixels.setBrightness(brightness);

  // wipe through rgb for LED debug
  colorWipe(pixels.Color(255,0,0),WIPE_DELAY);
  colorWipe(pixels.Color(0,255,0),WIPE_DELAY);
  colorWipe(pixels.Color(0,0,255),WIPE_DELAY);
  colorWipe(pixels.Color(0,0,0),WIPE_DELAY);
}

/* ---- LOOP ---- */
/*====================*/

void loop() {
  tmElements_t tm; // time struct holder
  int i = 0;
  // nyble vector for matrix columns
  // [hour/10, hour/1, minute/10, minute/1]
  byte bTime[4] = {B0000};

  // get the time from RTC
  if (RTC.read(tm) == RTC_RETURN) {
  } else {
    // rtc isn't reading
    Serial.println("RTC read error!  Please check the circuitry.");
    Serial.println();
    solidColor(255,0,0); // all red
    // error loop: wait 9s then try again
    delay(9000);
    return;
  }

  // poll BLE
  bluart.pollACI();

  // convert the time to nybles for the matrix
  pixelTime(tm, bTime);

  if (flag) {
    setBClock(tm, bTime, sizeof(bTime));
  }

  if (RTC.alarm(ALARM_2) ) {
    Serial.println("Alarm!");
    while (i < 30) {
      setMatrix(bTime, sizeof(bTime)/sizeof(bTime[1]), pixels.Color(255,140,0), pixels.Color(255,255,255));
      delay(500);
      solidColor(0,0,0);
      delay(500);
      i++;
    }
  }

  // debug
  /* Serial.println("Hour Digit 1:");*/
  /* Serial.println(bTime[0],BIN);*/
  /* Serial.println("Hour Digit 2:");*/
  /* Serial.println(bTime[1],BIN);*/
  /* Serial.println("Minute Digit 1:");*/
  /* Serial.println(bTime[2],BIN);*/
  /* Serial.println("Minute Digit 2:");*/
  /* Serial.println(bTime[3],BIN);*/

  // quarter hour indicator
  if ( (tm.Minute % 15 == 0) && tm.Second == 0) {
    quarterHour(tm.Hour, tm.Minute, QUARTER_WAIT);
  }
  // turn them all off for second indicator flash
  if (pulse_second) {
    for (uint8_t x = 0; x < PIXEL_ROW; x += width) {
      for (uint8_t y  = 0; y < PIXEL_COLUMN; y += height) {
        setPixel(pixelMap[x][y], 1, pixels.Color(0,0,0));
      }
    }
    pixels.show();
    delay(10);
  }
  // set the matrix to the binary time
  setMatrix(bTime, sizeof(bTime)/sizeof(bTime[1]), pixels.Color(255,255,255), pixels.Color(255,0,0));
  // wait a second before checking the time again
  /* delay(1000);*/

}

 /* ---- CLOCK FUNCTIONS ----*/
/*===============================*/

// fills the binary array with nybles
void pixelTime(tmElements_t tm, byte *bTime) {
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

// fills multiples 1/4 of matrix for 1/4 hour multiple
void quarterHour(uint8_t hour, uint8_t minute, uint16_t wait) {
  uint8_t no_rows;
  uint8_t y;
  uint8_t x;
  int16_t j;
  // light number of rows depending on the hour quarter (floored to be safe)
  no_rows = floor(minute / 15) * height;
  // one hour indicator is actually 0 but we want full display
  no_rows = (no_rows == 0) ? (4 * height) : no_rows;

  solidColor(0,0,0); // turn them all off
  pixels.show();

  if ((hour == 12 || hour == 0 ) && minute == 0) {
    rainbowCycle(25); // special edition hour for midday
  } else { // fade columns in and out
    for (j = 0; j <= 255; j += 5) {
      for (y = 0; y < no_rows; y++) {
        for (x = 0; x < PIXEL_ROW; x++) {
        setPixel(pixelMap[x][y],1,pixels.Color(0,0,j));
        }
      }
      pixels.show();
      delay(10);
    }
    delay(wait); // show for 10s
    for (j = 255; j >= 0; j -= 5) {
      for (y = 0; y < no_rows; y++) {
        for (x = 0; x < PIXEL_ROW; x++) {
        setPixel(pixelMap[x][y],1,pixels.Color(0,0,j));
        }
      }
      pixels.show();
      delay(10);
    }
  }
}

// sets the binary array to the matrix columns
void setMatrix(byte bMatrix[], size_t size, uint32_t color1, uint32_t color2) {
  uint8_t x; // row inc.
  uint8_t y; // height inc.
  uint8_t i; // binary set matrix
  uint8_t yy; // full column inc.
  uint8_t xx = 0; // full row inc.

  for ( i = 0; i < size; i++) {
    for (yy = 0;yy < PIXEL_COLUMN; yy += height) {
      for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
          if (x == 0 && y == 0) {
            setPixel(pixelMap[xx+x][yy+y],((bMatrix[i] >> yy/height) & 1),color1);
          } else {
            setPixel(pixelMap[xx+x][yy+y],((bMatrix[i] >> yy/height) & 1),color2);
          }
        }
      }
    }
    xx += width;
  }
  pixels.show();
}

// creates a cordinate map of pixels in the matrix
void initMatrixMap(uint16_t pixelMap[PIXEL_ROW][PIXEL_COLUMN], uint8_t rotate) {
  uint8_t i; // row inc.
  uint8_t j; // column inc.
  uint16_t pixel_inc; // pixel number inc.

  /* What we're trying to draw (64 pixel unicorn hat grid) */
  /* It's a snakes and ladders type arrangement for any matrix */
  /*   {7 ,6 ,5 ,4 ,3 ,2 ,1 ,0 },*/
  /*   {8 ,9 ,10,11,12,13,14,15},*/
  /*   {23,22,21,20,19,18,17,16},*/
  /*   {24,25,26,27,28,29,30,31},*/
  /*   {39,38,37,36,35,34,33,32},*/
  /*   {40,41,42,43,44,45,46,47},*/
  /*   {55,54,53,52,51,50,49,48},*/
  /*   {56,57,58,59,60,61,62,63}*/
  /* */

  // The cord either starts at 0 or length of row depending on whether it's rotated
  pixel_inc = rotate ? PIXEL_ROW : 0;

  for (i = 0; i < PIXEL_COLUMN; i++) {
    for (j = 0; j < PIXEL_ROW; j++) {
      // We either increment or decrement depending on column due to snakes and ladders arrangement
      if (rotate) {
        pixelMap[i][j] = (i % 2 == 0) ? --pixel_inc : ++pixel_inc;
      } else {
        pixelMap[j][i] = (i % 2 != 0) ? pixel_inc-- : pixel_inc++;
      }
    }
    pixel_inc += PIXEL_ROW;
    (i % 2 == 0) ? pixel_inc-- : pixel_inc++;
  }
}

void setBClock(tmElements_t &tm, byte *bTime, uint8_t size) {
  uint32_t entry = millis(); // entry time - reset each button press
  uint32_t hold = millis(); // hold timer to increase time change
  uint32_t debounce = 0; // debouce holder
  uint8_t dperiod = 200; // debouce period
  uint8_t state; // button state

  // disable interrupts to use the button
  noInterrupts();

  // run for 5s then save the time to RTC
  while (millis() - entry < 5000) {

    // set the matrix to the binary time for user to see curret setting
    pixels.clear();
    setMatrix(bTime, size/sizeof(bTime[1]), pixels.Color(255,0,0), pixels.Color(255,0,0));
    
    state = digitalRead(2);

    if (state) {

      if (millis() - hold > 1000) {
        dperiod = 40;
      } else {
        dperiod = 200;
      }

      if ((millis() - debounce) > dperiod ) {
        entry = millis();

        tm.Minute++;

        if (tm.Minute == 60) {
          tm.Hour++;
          tm.Minute = 0;
          if (tm.Hour == 24) {
            tm.Hour = 0;
          }
        }

        debounce = entry;
      }

    } else {
      hold = millis(); // reset the hold button as hasn't been pressed
    }

    pixelTime(tm, bTime);
  }

  // save to RTC, clear ISR flag and resume interrupts
  tm.Second = 0;
  RTC.write(tm);
  flag = false;
  interrupts();

}

// set the matrix to a solid colour
void solidColor(uint32_t r,uint32_t g,uint32_t b) {
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

 /* ---- ADAFRUIT NEOPIXEL FUNCTIONS ---- */
/*============================================*/

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

void setISR(void) {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();

  // debouce
  if (interrupt_time - last_interrupt_time > 200)
    flag = true;

  last_interrupt_time = interrupt_time;
}

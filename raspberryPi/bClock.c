/*===================================================================================================
//
//01000010 01101001 01101110 01100001 01110010 01111001  01000011 01101100 01101111 01100011 01101011
//______ _                          _____ _            _
//| ___ (_)                        /  __ \ |          | |
//| |_/ /_ _ __   __ _ _ __ _   _  | /  \/ | ___   ___| | __
//| ___ \ | '_ \ / _` | '__| | | | | |   | |/ _ \ / __| |/ /
//| |_/ / | | | | (_| | |  | |_| | | \__/\ | (_) | (__|   <
//\____/|_|_| |_|\__,_|_|   \__, |  \____/_|\___/ \___|_|\_\
//                           __/ |
//                          |___/
//
// Binary Clock
// Originally made for Arduino controller in my 'Wooden Bits' project
// John Whittington @j_whittington http://www.jbrengineering.co.uk 2014
//
// Compile using make then run sudo ./bClock [brightness] [pulse_second] [rotate]. Designed for Raspberry Pi
// and Pimoroni Unicorn Hat but code can scale to any size matrix. 'pixelMap' generator is useful for
// other functions other than this code.
//
// References - Unicorn Hat Driver: https://github.com/pimoroni/UnicornHat/tree/master/c/unicorn
//              WS2812 Driver: https://github.com/626Pilot/RaspberryPi-NeoPixel-WS2812
//
//===================================================================================================*/

#include "bClock.h"

// Usleep for startup wipe
#define WIPE_DELAY     1
// Usleep for quarter hour display
#define QUARTER_WAIT   10000

// Width and height of square to show bit
const static uint8_t width = PIXEL_ROW/4;
const static uint8_t height = PIXEL_COLUMN/4;
// Cordinate map of pixel init (gets filled by initMatrixMap)
uint16_t pixelMap [PIXEL_ROW][PIXEL_COLUMN] = {{0}};
// Pulse at second intervals
static int pulse_second = true;
// Rotate the display 90deg
static int rotate = false;

int main(int argc, char **argv) {

  // Catch all signals possible - it's vital we kill the DMA engine on process exit!
  uint8_t i;
  for (i = 0; i < 64; i++) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = unicorn_exit;
    sigaction(i, &sa, NULL);
  }

  // Get the input args if any
  int newbrightness = DEFAULT_BRIGHTNESS*100;
  if (argc == 2) {
    if(sscanf (argv[1], "%i", &newbrightness)!=1){
      printf ("Error, brightness must be an integer \n");
      return 0;
    }
  } else if (argc == 3) {
    if(sscanf (argv[1], "%i", &newbrightness)!=1){
      printf ("Error, brightness must be an integer \n");
      return 0;
    }
    if(sscanf (argv[2], "%i", &pulse_second) > 1){
      printf ("Error, second pulsing must be an bool \n");
      return 0;
    }
  } else if (argc == 4) {
    if(sscanf (argv[1], "%i", &newbrightness)!=1){
      printf ("Error, brightness must be an integer \n");
      return 0;
    }
    if(sscanf (argv[2], "%i", &pulse_second) > 1){
      printf ("Error, second pulsing must be an bool \n");
      return 0;
    }
    if(sscanf (argv[3], "%i", &rotate) > 1){
      printf ("Error, rotate must be an bool \n");
      return 0;
    }
  }


  // Don't buffer console output
  setvbuf(stdout, NULL, _IONBF, 0);

  // Set the number of pixels
  numLEDs = PIXEL_COLUMN*PIXEL_ROW;

  // Set the brightness
  setBrightness(newbrightness/100.0);

  // init WS2812
  initHardware();
  clearLEDBuffer();

  // Create the matrix lookup map
  initMatrixMap(pixelMap,rotate);

  // Wipe through rgb for LED debug at start up
  colorWipe(Color(255,0,0),WIPE_DELAY);
  colorWipe(Color(0,255,0),WIPE_DELAY);
  colorWipe(Color(0,0,255),WIPE_DELAY);
  colorWipe(Color(0,0,0),WIPE_DELAY);

  while(1) {
      // The get time and set matrix loop (Arduino lagacy)
      loop();
      // Wait a second before checking the time again
      usleep(1000*1000);
    }

  // Shouldn't get here but if we do, exit gracefully
  unicorn_exit(0);

  return 0;
}

void loop() {
  time_t rawtime; // raw time
  struct tm *ptime; // time struct holder
  // nyble vector for matrix columns
  // [hour/10, hour/1, minute/10, minute/1]
  uint8_t bTime[4] = {0b0000};
  // row/column inc.
  uint8_t x = 0;
  uint8_t y = 0;

  // get the time from system
  time(&rawtime);
  // load into the tm struct
  ptime = localtime(&rawtime);

  // convert the time to nybles for the matrix
  pixelTime(ptime, bTime);

  // quarter hour indicator
  if ( (ptime->tm_min % 15 == 0) && ptime->tm_sec == 0) {
    quarterHour(ptime->tm_hour, ptime->tm_min, QUARTER_WAIT);
  }

  if (pulse_second) {
    for (x = 0; x < PIXEL_ROW; x += width) {
      for (y = 0; y < PIXEL_COLUMN; y += height) {
        setPixelColorT(pixelMap[x][y],Color(0,0,0));
      }
    }
    show();
    usleep(100*100);
  }

  // set the matrix to the binary time
  setMatrix(bTime, sizeof(bTime)/sizeof(bTime[1]), Color(255,255,255), Color(255,0,0));

}

void pixelTime(struct tm *tm, uint8_t *bTime) {
  // convert the hour and minute into uint8
  uint8_t hour = tm->tm_hour;
  uint8_t minute = tm->tm_min;

  // convert the time into 4 nybles for each column of matrix
  if (hour >= 0 && hour < 10) {
    bTime[1] = hour | 0b0000;
    bTime[0] = 0b0000; // 2nd digit still needs clearing
  } else {
    bTime[1] = (uint8_t) (hour-(floor(hour/10)*10)) | 0b0000;
    bTime[0] = (uint8_t) (floor(hour-(hour-floor(hour)))/10) | 0b0000;
  }
  if (minute >= 0 && minute < 10) {
    bTime[3] = minute | 0b0000;
    bTime[2] = 0b0000; // 2nd digit still needs clearing
  } else {
    bTime[3] = (uint8_t) (minute-(floor(minute/10)*10)) | 0b0000;
    bTime[2] = (uint8_t) (floor(minute-(minute-floor(minute)))/10) | 0b0000;
  }
}

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
  show();

  if ((hour == 12 || hour == 0 ) && minute == 0) {
    rainbowCycle(25); // special edition hour for midday
  } else { // fade columns in and out
    for (j = 0; j <= 255; j += 5) {
      for (y = 0; y < no_rows; y++) {
        for (x = 0; x < PIXEL_ROW; x++) {
        setPixel(pixelMap[x][y],1,Color(0,0,j));
        }
      }
      show();
      usleep(10*1000);
    }
    usleep(wait*1000); // show for 10s
    for (j = 255; j >= 0; j -= 5) {
      for (y = 0; y < no_rows; y++) {
        for (x = 0; x < PIXEL_ROW; x++) {
        setPixel(pixelMap[x][y],1,Color(0,0,j));
        }
      }
      show();
      usleep(10*1000);
    }
  }
}

void setMatrix(uint8_t bMatrix[], size_t size, Color_t color1, Color_t color2) {
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
  show();
}

// set the matrix to a solid colour
void solidColor(uint32_t r,uint32_t g,uint32_t b) {
  uint16_t i;
  for (i = 0; i < numLEDs; i++) {
    setPixelColorT(i, Color(r,g,b));
  }
  show();
}

// set the pixel on or off
void setPixel(uint8_t pixel, uint8_t set, Color_t c) {
  if (set) {
    setPixelColorT(pixel, c);
  } else {
    setPixelColorT(pixel, Color(0,0,0));
  }
}

/*
  Clear the display and exit gracefully
*/
void unicorn_exit(int status){
  int i;
  for (i = 0; i < numLEDs; i++){
    setPixelColor(i,0,0,0);
  }
  show();
  terminate(status);
}

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

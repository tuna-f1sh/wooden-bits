
```
01000010 01101001 01101110 01100001 01110010 01111001  01000011 01101100 01101111 01100011 01101011
______ _                          _____ _            _    
| ___ (_)                        /  __ \ |          | |   
| |_/ /_ _ __   __ _ _ __ _   _  | /  \/ | ___   ___| | __
| ___ \ | '_ \ / _` | '__| | | | | |   | |/ _ \ / __| |/ /
| |_/ / | | | | (_| | |  | |_| | | \__/\ | (_) | (__|   < 
\____/|_|_| |_|\__,_|_|   \__, |  \____/_|\___/ \___|_|\_\
                           __/ |                          
                          |___/                           
```

# Binary Clock

Originally made for Arduino controller in my *Wooden Bits* project, this is a
binary clock for WS2812 LEDS that scales to any size pixel matrix. It includes
a function for generating the 'snakes and ladders' arrangment of pixels, which
can be used as a map reference for other functions.

## Requirements

### Hardware

* Arduino/attiny328 or Raspberry Pi.
* Pixel grid, user created or Adafruit NeoPixel, Pimoroni Unicorn Hat. Should
  be connected to pin 6 on Arduino or pin 18 on rPi by default.
* For Arduino you'll need a DS1307RTC chip, already set with the time (see
  library page).

### Arduino Libraries  

* **Adafruit NeoPixel:** https://github.com/adafruit/Adafruit_NeoPixel
* **DS1307RTC RTC:** https://www.pjrc.com/teensy/td_libs_DS1307RTC.html

## Build

### Arduino

Compile using the Arduino IDE or using the Makefile. Using the makefile reqruies 'Arduino.mk':
https://github.com/sudar/Arduino-Makefile

Set the number of pixels in each row and column in 'bClock.h'. The clock will automatically scale
the binary bits to the pixel grid - thanks to the pixelMap generation.

You can defined a flash on seconds 'pulse_second', rotated display 'rotate' and brightness

### Raspberry Pi

Compile using `make` then run `sudo ./bClock [brightness] [pulse_second] [rotate]`. Designed for Raspberry Pi
and Pimoroni Unicorn Hat but code can scale to any size matrix. `pixelMap` generator is useful for 
other functions other than this code.

## References

* **Adafruit NeoPixel library:** https://github.com/adafruit/Adafruit_NeoPixel
* **Unicorn Hat Driver:** https://github.com/pimoroni/UnicornHat/tree/master/c/unicorn
* **WS2812 Driver:** https://github.com/626Pilot/RaspberryPi-NeoPixel-WS2812

John Whittington [@j_whittington](http://www.twitter.com/j_whittington) http://www.jbrengineering.co.uk 2014

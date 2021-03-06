
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

![Wooden Bits](http://i.imgur.com/n8bL5TM.gif)

Originally made for Arduino controller in my [*Wooden Bits*](http://wp.me/p2C0Bi-7R) project, this is a
binary clock for WS2812 LEDS that scales to any size pixel matrix. It includes
a function for generating the 'snakes and ladders' arrangment of pixels, which
can be used as a map reference for other functions.

## Features

* Version for Arduino/Atmega328 and Raspberry Pi.
* Scales to any size WS2812 matrix.
* Ability to rotate display.
* Ability to set second indicator pulsing.
* Quarter hour indicator fills n/quarter rows of the display blue (n is the
  quater multiple).
* Full rainbow at midday and midnight.
* `setMatrix` for sending any binary size to display; not just time.
* `initMatrixMap` for creating lookup map of disaply.

For more details, see my blog post: http://wp.me/p2C0Bi-7R

## Buy One!

[My Shop](https://shop.jbrengineering.co.uk/product/wooden-bits-binary-wall-clock/)
[Etsy](https://www.etsy.com/uk/listing/224396022/wooden-bits-laser-cut-wooden-binary)

## Requirements

### Hardware

* Arduino/atmega328 or Raspberry Pi.
* Pixel grid, user created or Adafruit NeoPixel, Pimoroni Unicorn Hat. Should
  be connected to pin 6 on Arduino or pin 18 on rPi by default.
* For Arduino you'll need a DS1307RTC/DS3231RTC chip, already set with the time (see
  library page).
* There is a KiCAD PCB project, which is what I now use for clocks I sell.

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

![Wooden Bits](http://i.imgur.com/KnXqW3E.gif)

Compile using `make` then run `sudo ./bClock [brightness] [pulse_second] [rotate]`. Designed for Raspberry Pi
and Pimoroni Unicorn Hat but code can scale to any size matrix. `pixelMap` generator is useful for 
other functions other than this code.

### Open SCAD

Compile within Open SCAD and export as .dxf. The comments explain how to
export the various different layout options. The designs are supplied for personal
use only under Creative Commons Attribution-NonCommercial 4.0 International
License.

## References

* **Adafruit NeoPixel library:** https://github.com/adafruit/Adafruit_NeoPixel
* **Unicorn Hat Driver:** https://github.com/pimoroni/UnicornHat/tree/master/c/unicorn
* **WS2812 Driver:** https://github.com/626Pilot/RaspberryPi-NeoPixel-WS2812

John Whittington [@j_whittington](http://www.twitter.com/j_whittington) http://www.jbrengineering.co.uk 2014

# Attribution and License

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=CFA7TQXNFURLQ)

**Software** is [![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)

**Hardware (KiCad, OpenSCAD, design exports etc.)** is [![License: CC BY-SA 4.0](https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg)](http://creativecommons.org/licenses/by-sa/4.0/)

I make many of my projects Open Source so others can learn as I have, but
please remember to follow the license terms and attribute any derivations or
code reuse. You can do this by linking back to the project or
[jbrengineering.co.uk](www.jbrengineering.co.uk). Feel free to click the
donate button above if you do use the source :).

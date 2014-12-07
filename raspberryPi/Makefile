all: lib
	gcc -Wall -D_XOPEN_SOURCE -D_BSD_SOURCE bClock.c -o bClock -Ilib/ -Llib/ -lm -lws2812-RPi

lib:
	make -C lib/

clean:
	-rm bClock

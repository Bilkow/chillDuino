# chillDuino

Academic project for a AVR-controlled computer (3-pins?) Fan.

# Compiling and Uploading

Find out what's the arduino device fisrt! You may need to ls your /dev or build with the arduino IDE in verbose.


```
avr-gcc -g -Wall -Os -mmcu=atmega2560 -c main.c uart.c && \
avr-gcc -g -mmcu=atmega2560 -o main.elf main.o uart.o && \
avr-objcopy -j .text -j .data -O ihex main.elf main.hex && \
\
avrdude -v -p atmega2560 -c wiring -P /dev/$arduino -b 115200 -D -U flash:w:main.hex:i
```

#!/bin/bash 

avr-objcopy -j .text -j .data -O ihex $1 $1.hex
dfu-programmer atmega32u2 erase
dfu-programmer atmega32u2 flash $1.hex
dfu-programmer atmega32u2 reset
dfu-programmer atmega32u2 reset

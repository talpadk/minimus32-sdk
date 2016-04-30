#!/bin/bash 

avr-objcopy -j .text -j .data -O ihex $1 $1.hex
avrdude -c usbasp-clone -p m32u4 -e -U flash:w:$1.hex

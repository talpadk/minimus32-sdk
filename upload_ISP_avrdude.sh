#!/bin/bash 

avr-objcopy -j .text -j .data -O ihex $3 $3.hex
avrdude -c $1 -p $2 -e -U flash:w:$3.hex

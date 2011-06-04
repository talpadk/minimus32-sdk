#!/bin/bash 

avr-objcopy -j .text -j .data -O ihex $1 $1.hex
#sudo  dfu-programmer atmega32u2 erase
#sudo  dfu-programmer atmega32u2 flash $1.hex
#sudo  dfu-programmer atmega32u2 reset
#sudo  dfu-programmer atmega32u2 reset

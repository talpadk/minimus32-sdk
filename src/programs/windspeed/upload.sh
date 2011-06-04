#!/bin/sh
if [ -z "$1" ]; then
	echo "Usage: $0 <file.hex>"
	exit 1
fi
echo "Uploading '$1' to Minimus USB"
dfu-programmer atmega32u2 erase && dfu-programmer atmega32u2 flash "$1"

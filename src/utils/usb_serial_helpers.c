#include "usb_serial_helpers.h"


void usb_serial_printHexNibble(uint8_t nibble, FILE *stream){
  if (nibble>9){
    nibble += 'A'-10;
  }
  else {
    nibble += '0';
  }
  fputc(nibble, stream);
}


void usb_serial_printHexByte(uint8_t byte, FILE *stream){
  usb_serial_printHexNibble(byte>>4, stream);
  usb_serial_printHexNibble(byte&0x0f, stream);
}

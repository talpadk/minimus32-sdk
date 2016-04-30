#ifndef USB_SERIAL_HELPERS_H
#define USB_SERIAL_HELPERS_H

/**
 * @file   usb_serial_helpers.h
 * 
 * @brief  Helper functions for using a USB based serialport
 * @ingroup util
 */


#include <stdint.h>
#include <stdio.h>

void usb_serial_printHexNibble(uint8_t nibble, FILE *stream);
void usb_serial_printHexByte(uint8_t byte, FILE *stream);

#endif /* USB_SERIAL_HELPERS_H */

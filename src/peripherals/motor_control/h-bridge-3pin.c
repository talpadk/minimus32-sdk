#include "h-bridge-3pin.h"
#include <stdlib.h>

void hBridge3PinInit(uint16_t maxPower, hBridge3PinIO io, HBridge3PinHandle *handle){
  sw_dither_init (&(handle->dither), maxPower);
  sw_dither_set(&(handle->dither), 0);
  handle->io = io;
  handle->a=255;
  handle->b=0;
}

void hBridge3PinSetPower(int16_t power, HBridge3PinHandle *handle){
  if (power<0){
    power = -power;
    handle->b=255;
    handle->a=0;
  }
  else {
    handle->a=255;
    handle->b=0;
  }
  sw_dither_set(&(handle->dither), power);
}

void hBridge3PinAnimate(HBridge3PinHandle *handle){
  uint8_t enable=0;
  if (sw_dither_animate(&(handle->dither))) enable=255;
  handle->io(enable, handle->a, handle->b);
}

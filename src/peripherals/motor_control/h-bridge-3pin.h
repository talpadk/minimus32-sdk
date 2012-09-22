#ifndef MOD_H_BRIDGE_3PIN_H
#define MOD_H_BRIDGE_3PIN_H

#include <stdint.h>
#include "sw_dither.h"

typedef void(*hBridge3PinIO)(uint8_t enable, uint8_t aSide, uint8_t bSide);

typedef struct {
  hBridge3PinIO io;
  sw_dither dither;
  uint8_t a;
  uint8_t b;
} HBridge3PinHandle;

void hBridge3PinInit(uint16_t maxPower, hBridge3PinIO io, HBridge3PinHandle *handle);
void hBridge3PinSetPower(int16_t power, HBridge3PinHandle *handle);
void hBridge3PinAnimate(HBridge3PinHandle *handle);

#endif //MOD_H_BRIDGE_3PIN_H

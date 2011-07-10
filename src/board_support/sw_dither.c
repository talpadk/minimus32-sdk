#include "sw_dither.h"


void sw_dither_init(sw_dither *dither, uint16_t max){
  dither->counter=0;
  dither->max = max;
  dither->power = 0;
}

void sw_dither_set(sw_dither *dither, uint16_t power){
  dither->power = power;
}

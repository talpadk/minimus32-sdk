#include "ds18s20.h"

#include <inttypes.h>

void ds18s10_blocking_print_temp(ds18s20_scratchpad *scratchpad, char buffer[6]){
  int i;
  int16_t tmp=((scratchpad->temperature_msb)<<8)+scratchpad->temperature_lsb;
  buffer[5]=0;
  if (tmp&1) buffer[4]='5';
  else buffer[4]='0';
  buffer[3]='.';
  tmp=tmp>>1;
    
  for (i=2; i>=0; i--){
    buffer[i]=(tmp%10)+'0';
    tmp /= 10;
  }
}

#ifndef MOD_DS18S20_H
#define MOD_DS18S20_H

#include <stdint.h>

#include "onewire.h"

///Defines and functions specific to the DS18S20

/**
 * @file
 * @author Visti Andresen
 * @ingroup per
 * @ingroup per_sensors
 * @ingroup per_sensors_temperature
 * @todo write one_wire.h 
 */


typedef struct {
  uint8_t temperature_lsb;
  uint8_t temperature_msb;
  uint8_t th;
  uint8_t tl;
  uint8_t res1;
  uint8_t res2;
  uint8_t count_remaining;
  uint8_t count_per_c;
  uint8_t crc;
} ds18s20_scratchpad;

void ds18s10_blocking_print_temp(ds18s20_scratchpad *scratchpad, char buffer[6]);

#endif //MOD_DS18S20_H

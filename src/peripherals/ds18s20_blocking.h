#ifndef MOD_DS18S20_BLOCKING_H
#define MOD_DS18S20_BLOCKING_H

#include <stdint.h>
#include "ds18s20.h"

#define DS18S20_CMD_CONVERT (0x44)
#define DS18S20_CMD_READ_SCRATCHPAD (0xBE)


/** 
 * Reads the scratch pad from the DS18S20
 * 
 * @param code the rom code of the device to read
 * @param scratchpad the buffer to store the result in
 * 
 * @return true (1) if the read succeeded
 */
uint8_t ds18s20_blocking_read_scratchpad(const onewire_rom_code *code, ds18s20_scratchpad *scratchpad);

void ds18s20_blocking_start_conversion(const onewire_rom_code *code);

#endif //#ifndef MOD_DS18S20_BLOCKING_H

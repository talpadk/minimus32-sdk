#include "ds18s20_blocking.h"

void ds18s20_blocking_send_bytes(int count, const uint8_t *bytes){
  int i;
  for (i=0; i<count; i++){
    onewire_send_byte(bytes[i]);
  }
}

void ds18s20_blocking_read_bytes(int count, uint8_t *buffer){
  int i;
  for (i=0; i<count; i++){
    buffer[i]=onewire_read_byte();
  }
}

uint8_t ds18s20_blocking_read_scratchpad(const onewire_rom_code *code, ds18s20_scratchpad *scratchpad){
  onewire_reset();
  onewire_send_byte(ONEWIRE_MATCH_ROM);
  ds18s20_blocking_send_bytes(7, code->rom_code);
  onewire_send_byte(code->crc);
  onewire_send_byte(DS18S20_CMD_READ_SCRATCHPAD);
  ds18s20_blocking_read_bytes(9, (uint8_t*)scratchpad);
  return 1;
}

void ds18s20_blocking_start_conversion(const onewire_rom_code *code){
  onewire_reset();
  onewire_send_byte(ONEWIRE_MATCH_ROM);
  ds18s20_blocking_send_bytes(7, code->rom_code);
  onewire_send_byte(code->crc);
  onewire_send_byte(DS18S20_CMD_CONVERT);
}

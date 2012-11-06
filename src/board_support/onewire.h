#ifndef MOD_1WIRE_H
#define MOD_1WIRE_H

///Support for using 1-wire devices semi hw accelerated

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 */

#include <stdint.h>

#define ONEWIRE_MATCH_ROM (0x55)
#define ONEWIRE_READ_ROM (0x33)

typedef void(*onewire_action)(void);
typedef uint8_t(*onewire_get_bit)(void);

typedef struct{
  uint8_t rom_code[7];
  uint8_t crc;
} onewire_rom_code;

void onewire_init(onewire_action pull_low, onewire_action release, onewire_get_bit get_bit);
void onewire_reset(void);
void onewire_send_byte(uint8_t byte);
uint8_t onewire_read_byte(void);

uint8_t onewire_calculate_crc(uint8_t *buffer, uint8_t length);
#endif //MOD_1WIRE_H

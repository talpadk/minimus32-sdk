#ifndef MOD_1WIRE_H
#define MOD_1WIRE_H

///Support for using 1-wire devices semi hw accelerated

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 */

#include <stdint.h>

typedef void(*onewire_action)(void);
typedef uint8_t(*onewire_get_bit)(void);

typedef enum {
  onewire_sending_reset=0,
  onewire_idle,
  onewire_error_presence_pulse_missing,
  onewire_sending_byte,
  onewire_read_byte
} onewire_state;

typedef enum {
  onewire_pulling_low=0,
  onewire_waiting_for_presence_pulse,
  onewire_waiting_reset_rest,
  onewire_high_period
} onewire_substate;

void onewire_init(uint8_t interrupt_id, onewire_action pull_low, onewire_action release, onewire_get_bit get_bit);
void onewire_interrupt(void);
onewire_state onewire_get_state(void);
void onewire_send_byte(uint8_t byte);
void onewire_wait_idle(void);
void onewire_start_read_byte(void);
uint8_t onewire_get_buffer(void);
#endif //MOD_1WIRE_H

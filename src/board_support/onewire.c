#include "onewire.h"

#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

onewire_action pull_low_;
onewire_action release_;
onewire_get_bit get_bit_;

void onewire_reset(void){
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    (*pull_low_)();
    _delay_us(480+2);
    (*release_)();
  }
  _delay_us(480+2);
}

void onewire_init(onewire_action pull_low, onewire_action release, onewire_get_bit get_bit){
  pull_low_ = pull_low;
  release_ = release;
  get_bit_ = get_bit;
  onewire_reset();
}

void onewire_send_byte(uint8_t byte){
  uint8_t i;
  for (i = 0; i<8; i++){
    while (!(*get_bit_)()){} //wait for idle bus
    if (byte&1){
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	(*pull_low_)();
	_delay_us(2); //atleast 1 us
	(*release_)();
      }	
      _delay_us(60-2); //longer is okay, so out of atomic
    }
    else {
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	(*pull_low_)();
	_delay_us(60);
	(*release_)();
      }
    }
    _delay_us(1);
    byte = byte >> 1; 
  }
}

uint8_t onewire_read_byte(void){
  uint8_t result = 0;
  uint8_t i;
  for (i = 0; i < 8; i++){
    result = result >> 1;
    while (!(*get_bit_)()){} //wait for idle bus
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
	(*pull_low_)();
	_delay_us(2); //atleast 1 us
	(*release_)();
	_delay_us(2);
	if ((*get_bit_)()){
	  result |= 128;
	}
    }
    _delay_us(60-2-2-1);   	
  }
  return result;
}


#include "external_interrupt.h"

void enable_external_interrupt_input(uint8_t id, interrupt_edge edge) {
  EIMSK |= (1<<id);
  if (id<4){
    EICRA &= ~(0b11<<(id*2));
    EICRA |= edge<<(id*2);
  }
  else {
    EICRB &= ~(0b11<<((id-4)*2));
    EICRB |= edge<<((id-4)*2);
  }

  // Global interrupt enable
  sei();
}

void disable_external_interrupt_input(uint8_t id) {
  EIMSK &= ~(1<<id);
}

#include "external_interrupt.h"

void enable_external_interrupt_input(uint8_t id, interrupt_edge edge) {
  EIMSK |= (1<<id);
  if (id<4){
    EICRA &= ~(0b11<<id*2);
    EICRA |= edge<<id*2;
  }
  else {
    EICRB &= ~(0b11<<(id-4)*2);
    EICRB |= edge<<(id-4)*2;
    
  }

  // Global interrupt enable
  sei();
}

void disable_external_interrupt_input(uint8_t id) {
	switch (id) {
		case 1:
			EIMSK |= (0<<INT1);
		break;
		case 2:
			EIMSK |= (0<<INT2);
		break;
		case 3:
			EIMSK |= (0<<INT3);
		break;
		case 4:
			EIMSK |= (0<<INT4);
		break;
		case 5:
			EIMSK |= (0<<INT5);
		break;
		case 6:
			EIMSK |= (0<<INT6);
		break;
		case 7:
			EIMSK |= (0<<INT7);
		break;
	}
}

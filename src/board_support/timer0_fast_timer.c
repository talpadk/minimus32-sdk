#include "timer0_fast_timer.h"

#include <avr/io.h>

void timer0_fast_timer_init(timer0_prescaler prescaler, uint8_t counter){
  TCCR0B = prescaler;
  TCCR0A = 2;
  OCR0A = counter;
  TIMSK0 = 2; //enable int
}

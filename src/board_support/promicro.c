#include "promicro.h"
#include <avr/io.h>

void promicro_init(void){
  PORTB |= 0b00000001; //turn off the yellow led
  DDRB  |= 0b00000001; //configure the pin as output

  PORTD |= 0b00100000; //turn off the green led
  DDRD  |= 0b00100000; //configure the pin as output
}

void led_yellow(char on){
  if (on) PORTB &= 0b11111110;
  else    PORTB |= 0b00000001;
}

void led_yellow_toggle(void){
  PORTB ^= 0b00000001;
}

void led_green(char on){
  if (on) PORTD &= 0b11011111;
  else    PORTD |= 0b00100000;
}

void led_green_toggle(void){
  PORTD ^= 0b00100000;
}

#include "minimus32.h"
#include <avr/io.h>

void minimus32_init(){
  PORTD |= 0b01100000; //turn off leds
  DDRD  |= 0b01100000; //enable leds as output
  DDRD  &= 0b01111111; //make hwb an input
}

void led_blue(char on){
  if (on) PORTD &= 0b11011111;
  else    PORTD |= 0b00100000;
}

void led_blue_toggle(){
  PORTD ^= 0b00100000;
}

void led_red(char on){
  if (on) PORTD &= 0b10111111;
  else    PORTD |= 0b01000000;
}

void led_red_toggle(){
  PORTD ^= 0b01000000;
}


char hwb_is_pressed(){
  return !(PIND & 0b10000000);
}

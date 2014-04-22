#include "hw_pc_fan.h"
#include "timer0.h"
#include <avr/io.h>

void hw_pc_fan_init(uint8_t power){
  hw_pc_fan_set(power);


  //Clear OC0B on match, set at TOP(OCRA), fast PWM (WGM2 in TCCR0B)
  TCCR0A = (1<<COM0B1)|(0<<COM0B0)|(1<<WGM01)|(1<<WGM00);
  //Divide clock (16MHz) with 8
  TCCR0B = TIMER0_8|(1<<WGM02);
  //Set TOP to 79, fout=16MHz/(8*(79+1))=25kHz
  OCR0A = 79;
  //set output
  DDRD |= 1;
}

void hw_pc_fan_set(uint8_t power){
  OCR0B = power;
}

void hw_pc_fan_free(void){
  TCCR0B = TIMER0_STOP;
  //set input
  DDRD &= ~1;
}

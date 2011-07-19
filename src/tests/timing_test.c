//#exe

///Demonstrates using timer 1 to flash a led on and off every second

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @todo Rewrite to use the BSP functions
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
int clock;

ISR(TIMER1_COMPA_vect)
{
  //on/off period of 1 sec @ 16Mhz
  clock++;
  if (clock>1000){
    clock=0;
    led_blue_toggle();
  }
}


int main(void) {
  unsigned int i;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  


  OCR1AH = 62; //set OCR1A to 16000
  OCR1AL = 128; 
  
  TCCR1A = 0; //prescaler = 1 & count up to OCR1A function 
  TCCR1B = 1<<WGM12|1<<CS10;

  TIMSK1 = 1<<ICIE1|1<<OCIE1A; //enable timer1 int and compare a int

  sei(); //global int enable

  while (!hwb_is_pressed()){
    if (CLKSEL0&1<<CLKS){
      led_red(1);
    }
    else {
      led_red(0);
    }
  }

  while (1){
    led_red_toggle();
    for (i=0; i<60000; i++){
    }
  }
  return 0;
}

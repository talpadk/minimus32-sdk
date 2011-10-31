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
#include "timer1_clock.h"

ISR(TIMER1_COMPB_vect)
{
  timer1_sub_timer_b_unset();
  PORTD |= 0b00100000; //led off
}

void blink(void *data){
  timer1_sub_timer_b_delay(160);
  PORTD &= 0b11011111;//led on
  led_red_toggle();
}

int main(void) {
  uint32_t i;
  timer1_callback blink_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  //Setup a blink pattern
  timer1_clock_init();
  timer1_clock_register_callback(0, 200, 1, &blink, 0, &blink_call_back);

  while (1){
    for (i=0;i<50000;i++){}

  }
  return 0;
}

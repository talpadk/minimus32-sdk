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
#include "hw_rc_servo.h"

hw_rc_servo servos[1];

hw_rc_servos servosHandle;


void blink(void *data){
  led_blue_toggle();
}

int main(void) {
  uint32_t i;
  timer1_callback blinkCallback;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  timer1_clock_init();
  timer1_clock_register_callback (0,500, 1, &blink, 0, &blinkCallback);

  hw_servo_init(&servosHandle, servos, 1);

  sei(); //global int enable

  while (1){
    
    for (i=0; i!=6000; i++){
    }
  }
  return 0;
}

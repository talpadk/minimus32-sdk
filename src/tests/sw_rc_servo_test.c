//#exe

/**
 * CAUTION: The USB port has limits to the current it can supply, it is highly recommended that the servo is't supplied with 5V from Vcc, especially if it is loaded.
 *
 * PD6 (the red led) is used for the pulse width signal to the servo.
 * 
 * The blue led brightness is set according to the servo position setpoint
 *
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer0_fast_timer.h"
#include "sw_rc_servo.h"
#include "sw_dither.h"

sw_dither dither;

sw_rc_servo servo;



ISR(TIMER0_COMPA_vect){
  //The led setting functions are to slow.. using bit ops.
  PORTD = (PORTD&0b10011111) |
    ((!sw_dither_animate(&dither))<<5) |
    (sw_rc_servo_animate(&servo)<<6);
}

int main(void) {
  int32_t i;
  uint8_t pos=0;
  uint8_t dir=1;
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  sw_dither_init(&dither, 50); //Full brightness was too bright 50% is better
  sw_rc_servo_init(&servo, 25);
  timer0_fast_timer_init(TIMER0_8, 80); //setup 25 ints per ms

  sei(); //ints on

  while (1){
    sw_dither_set(&dither, pos);
    sw_rc_servo_set_pos(&servo, pos);
    pos+=dir;
    if (pos==25) dir=255;
    if (pos==0) dir = 1;
    
    for (i=0; i<6000; i++){
    }
  }

}

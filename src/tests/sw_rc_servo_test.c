//#exe

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer0_fast_timer.h"
#include "sw_rc_servo.h"

sw_rc_servo servo;

ISR(TIMER0_COMPA_vect){
  led_red(!sw_rc_servo_animate(&servo));
}

int main(void) {
  int i;
  uint8_t pos=0;
  uint8_t dir=1;
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  sw_rc_servo_init(&servo, 50);
  timer0_fast_timer_init(TIMER0_1, 320); //setup 50 ints per ms

  sei(); //ints on

  while (1){
    sw_rc_servo_set_pos(&servo, pos);
    pos+=dir;
    if (pos==50) dir=255;
    if (pos==0) dir = 1;
    
    for (i=0; i<300; i++){
    }
  }

}

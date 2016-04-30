//#global_cflags -DMCU=atmega32u4
//#exe

///A simple "hello world" that flashes the two on board LEDs 

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @ingroup board_promicro
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "promicro.h"
int clock;

int main(void) {
  uint32_t i;

  watchdog_disable();
  promicro_init();
  clock_prescale_none();
  led_yellow(0);
  led_green(1);
  
  while (1){
    led_yellow_toggle();
    led_green_toggle();
    for (i=0; i<600000; i++){
    }
  }
  return 0;
}

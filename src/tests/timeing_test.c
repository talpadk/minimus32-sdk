//#exe
#include <avr/io.h>
#include <avr/power.h>

#include "minimus32.h"
#include "watchdog.h"


int main(void) {
  int i;

  watchdog_disable();
  minimus32_init();

  while (!hwb_is_pressed()){
    led_blue_toggle();
    for (i=0; i<5000; i++){
    }
  }

  while (1){
    led_red_toggle(1);
    for (i=0; i<20000; i++){
    }
  }
  return 0;
}

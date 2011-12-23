//#exe

///A ultra dirty demo detecting touches and signaling it using the blue LED

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 */


#include <avr/io.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "lcd_44780.h"

int main(){
  uint16_t i,j,k;
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  //PC7 = output
  //PC6 = detector;
  DDRC |= 0b10000000;
  
  

  while (1){
    i=0;
    for (j=0; j<100; j++){
      PORTC = 0b10111111;
      while (!(PINC&0b01000000)){i++;}   
      PORTC = 0b00111111;
      //      while (PINC&0b01000000){i++;}
      for (k=0; k<200; k++){ }
    }

    led_blue(i>=605);
  }
}

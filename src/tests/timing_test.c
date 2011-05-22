//#exe
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"

int clock;

void clock_prescale_set_local(uint8_t a)
{
    uint8_t __tmp = _BV(CLKPCE);
    __asm__ __volatile__ (
        "in __tmp_reg__,__SREG__" "\n\t"
        "cli" "\n\t"
        "sts %1, %0" "\n\t"
        "sts %1, %2" "\n\t"
        "out __SREG__, __tmp_reg__"
        : /* no outputs */
        : "d" (__tmp),
          "M" (_SFR_MEM_ADDR(CLKPR)),
          "d" (a)
        : "r0");
}

ISR(TIMER1_COMPA_vect) //(TIMER1_OVF_vect)
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

  clock_prescale_set_local(0); //1 as prescaler

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

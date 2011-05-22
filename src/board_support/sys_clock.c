#include "sys_clock.h"

#include <avr/io.h>

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



void clock_prescale_none() {
  clock_prescale_set_local(0); //1 as prescaler
}

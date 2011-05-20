#include "watchdog.h"

#include <avr/io.h>

void watchdog_disable() {
  asm("WDR");
  //Clear watchdog has reset us flag
  MCUSR  &= ~(1<<WDRF); 
  //Triger alterations to WD are OK
  WDTCSR = (1<<WDCE) | (1<<WDE);
  //Stop watchdog actions
  WDTCSR = 0x00;
  //We should probably lookinto powering down the WD oscilator if possible to save power
}

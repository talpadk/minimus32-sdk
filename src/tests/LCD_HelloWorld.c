//#exe
#include <avr/io.h>
#include <avr/power.h>

#include "minimus32.h"
#include "sys_clock.h"
#include "watchdog.h"
#include "lcd_44780.h"

/** 
 * @file LCD_HelloWorld.c
 * 
 * A 44780 based LCD demo
 *
 * PD0 = DB4
 *
 * PD1 = DB5
 *
 * PD2 = DB6
 *
 * PD3 = DB7
 *
 * PD4 = RS
 *
 * PD5 = RW
 *
 * PD6 = E
 * 
 */

int main(void) {
  watchdog_disable();

  lcd_44780_power_up_delay();
  lcd_44780_init();

  lcd_44780_print("Hello world");

  while (1) { }
  return 0;
}

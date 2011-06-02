//#exe
#include <avr/io.h>
#include <avr/power.h>

#include "minimus32.h"
#include "sys_clock.h"
#include "timer1_clock.h"
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

void print_time(){
  char * time_buffer = "Dxx hh:mm:ss.mmm";
  timer1_wall_time time;
  timer1_clock_get_time(&time);

  time_buffer[1] = (time.day/10)%10+'0';
  time_buffer[2] = (time.day%10)+'0';

  time_buffer[4] = (time.hour/10)+'0';
  time_buffer[5] = (time.hour%10)+'0';

  time_buffer[7] = (time.min/10)+'0';
  time_buffer[8] = (time.min%10)+'0';

  time_buffer[10] = (time.sec/10)+'0';
  time_buffer[11] = (time.sec%10)+'0';

  time_buffer[13] = (time.msec/100)+'0';
  time_buffer[14] = ((time.msec%100)/10)+'0';
  time_buffer[15] = (time.msec%10)+'0';

  lcd_44780_print(time_buffer);
}

int main(void) {
  watchdog_disable();
  clock_prescale_none();
  timer1_clock_init();

  lcd_44780_power_up_delay();
  lcd_44780_init();

  lcd_44780_print("Hello world");

  while (1) {
    lcd_44780_command(LCD_44780_GOTO_CMD+40);
    print_time();  
  }
  return 0;
}

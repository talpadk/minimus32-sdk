//#exe

///A quick an dirty demo of reading the capacitance value of a "touch sensor" displaying the result on a LCD

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
  
  

  lcd_44780_power_up_delay();
  lcd_44780_init();

  lcd_44780_command(LCD_44780_GOTO_CMD);
  lcd_44780_print("Capacitive");
  lcd_44780_command(LCD_44780_GOTO_CMD+64);
  lcd_44780_print("sensor: ");
  while (1){
    i=0;
    for (j=0; j<100; j++){
      PORTC = 0b10111111;
      while (!(PINC&0b01000000)){i++;}   
      PORTC = 0b00111111;
      //      while (PINC&0b01000000){i++;}
            for (k=0; k<100; k++){ }
    }

    /*    PORTC = 0b10000000;
    while (!(PINC&0b01000000)){i++;}
    i=0;
    PORTC = 0;
    while (PINC&0b01000000){i++;}*/

    lcd_44780_command(LCD_44780_GOTO_CMD+64+8);
    lcd_44780_write_byte ((i/10000)+'0', 0);
    lcd_44780_write_byte (((i%10000)/1000)+'0', 0);
    lcd_44780_write_byte (((i%1000)/100)+'0', 0);
    lcd_44780_write_byte (((i%100)/10)+'0', 0);
    lcd_44780_write_byte (((i%10))+'0', 0);
  }
}

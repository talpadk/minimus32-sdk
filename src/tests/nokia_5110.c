//#exe

///A program that demonstrates the usage om a Nokia 5110 (PCD8544 based) display

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * 
 * Red  blinking             = Start up delay
 * Blue blinking, red  solid = Status OK 
 *
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)
 * The N5110 must be supplied with 3.3V on N5110 (Vcc) with enough current
 * The N5110 must be connected to GND on N5100 (GND)
 *
 * PB1 (SCLK)  -> N5110.7 (SCLK)
 * PB2 (MOSI)  -> N5110.6 (DNK)
 * 
 * PB4         -> N5110.3 (SCE)
 * PB5         -> N5110.4 (RST)
 * PB6         -> N5110.5 (D/C)
 * 
 */

#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "pcd8544.h"

#define POWER_ON_DELAY (11)
#define BLINK_FATAL (100)
char blinkTimer=0;

uint8_t bar_=0;

void blink(void *data){
  if (blinkTimer==BLINK_FATAL){
    led_red_toggle();
    led_blue(1);
  }
  else {
    if (blinkTimer!=POWER_ON_DELAY){
      led_red_toggle();
      blinkTimer++;
    }
    else {
      led_blue_toggle();
    }
    pcd8544_test(bar_);
    bar_++;
    if (bar_==8) bar_=0;

  }

}

void assertReset()   { PORTB &= ~(1<<PB5); }
void desertReset()   { PORTB |=  (1<<PB5); }
void assertCS()      { PORTB &= ~(1<<PB4); }
void desertCS()      { PORTB |=  (1<<PB4); }
void assertCMD()     { PORTB &= ~(1<<PB6); }
void desertCMD()     { PORTB |=  (1<<PB6); }


pcd8544_io pcdIO = {
  assertReset,
  desertReset,
  assertCS,
  desertCS,
  assertCMD,
  desertCMD  
};

int main(void) { 
  uint8_t i;
  timer1_callback blink_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  spi_config_io_for_master_mode();
  DDRB |= 0b01110000; //IO setup for the reset,cs and cmd pin

  pcd8544_init(&pcdIO, 45);

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 150, 1, &blink, 0, &blink_call_back);


  while (1) {
    for (i=0; i<8; i++){
      
    }
  }
  return 0;
}

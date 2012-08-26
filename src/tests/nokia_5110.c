//#exe

///A program that demonstrates the usage om a Nokia 5110 (PCD8544 based) display

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * 
 * Red  blinking             = Status OK<br>
 *
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)<br>
 * The N5110 must be supplied with 3.3V on N5110 (Vcc) with enough current<br>
 * The N5110 must be connected to GND on N5100 (GND)<br>
 *<br>
 * PB1 (SCLK)  -> N5110.7 (SCLK)<br>
 * PB2 (MOSI)  -> N5110.6 (DNK)<br>
 * 
 * PB4         -> N5110.3 (SCE)<br>
 * PB5         -> N5110.4 (RST)<br>
 * PB6         -> N5110.5 (D/C)<br>
 *<br>
 * Optional<br>
 * PB -> 62Ohm -> N5110.8 (LED) I haven't found any data on the LED,
 * but a current limiting resistor of 62 Ohm seems safe (~7mA) at 3.3V<br>
 */

#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "spi.h"
#include "pcd8544.h"
#include "sw_dither.h"

uint8_t bar_=0;
sw_dither backlight;

void handleBackLight(void *data){
  if (sw_dither_animate(&backlight)) PORTB |= (1<<PB7);
  else PORTB &= ~(1<<PB7);
}

void blink(void *data){
  led_red_toggle();
  pcd8544_test(bar_);
  bar_++;
  if (bar_==8) bar_=0;
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
  uint16_t i,j;
  timer1_callback blink_call_back;
  timer1_callback backlight_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  spi_config_io_for_master_mode();
  DDRB |= 0b11110000; //IO setup for the reset,cs, cmd and led pin

  pcd8544_init(&pcdIO, 45);

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  //Enable the pulsed output that controls the backlight
  sw_dither_init(&backlight, 1023);
  timer1_clock_register_callback(0, 1, 1, &handleBackLight, 0, &backlight_call_back);

  for (i=0; i<1024; i++){
    for(j=0; j<1000; j++){}
    sw_dither_set(&backlight, i);
  } 

  while (1) { }
  return 0;
}

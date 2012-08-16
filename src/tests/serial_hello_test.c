//#exe

///A program that demonstrates a hello world over the serial port

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 */

#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "vt100_codes.h"
#include "async_serial.h"


void blink(void *data){
  led_blue_toggle();
}

int main(void) { 
  timer1_callback blink_call_back;

  char tmp;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  async_serial_init(SERIAL_SPEED_9600);

  sei(); //global int enable

  //Setup a blink pattern indicating a working MCU
  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  async_serial_write_string(VT100_CURSOR_OFF);
  async_serial_write_string(VT100_CLEAR_SCREEN);
  async_serial_write_string("=== Hello world ===\r\n\r\nPress any key for the next screen.");

  while (1) {
    if (async_serial_byte_ready()){
      tmp = async_serial_read_byte();

      async_serial_write_string(VT100_CLEAR_SCREEN);
      async_serial_write_string("You send me a '");
      async_serial_write_byte(tmp);
      async_serial_write_byte('\'');
    }
  }
}

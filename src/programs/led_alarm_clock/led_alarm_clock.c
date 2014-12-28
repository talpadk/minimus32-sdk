//#exe
//#global_cflags -D MCU=ATMEGA328P

///A program for a LED/light based alarm clock

/**
 * @file
 * @author Visti Andresen
 * @ingroup prog
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "vt100_codes.h"
#include "async_serial_0.h"
#include "adc_single_ended.h"
#include "int_print.h"
#include "sw_dither.h"
#include "timer0_fast_timer.h"
#include "timer1_clock.h"

sw_dither blueLed;
uint8_t blueLedOn;


//Brightness control the LED
ISR(TIMER0_COMPA_vect) {
  if(sw_dither_animate(&blueLed)){
    PORTB |= 0b00100000;
  }
  else {
    PORTB &= 0b11011111;
  }
}

void ledAnimate(void *data){
  if(blueLedOn){
    blueLedOn = 0;
    sw_dither_set(&blueLed, 0);
  }
  else {
    blueLedOn = 1;
    sw_dither_set(&blueLed, 10);
  }
}

int main(void) {
  uint32_t supplyVoltage;
	char buffer[7];
  timer1_callback ledTimer;
  
	watchdog_disable();
	clock_prescale_none();	
	
	PORTB &= 0b00000000; //turn off led
	DDRB  |= 0b00100000; //enable led as output
  sw_dither_init(&blueLed, 1023);
  
  //3.91 kHz interrupt for LED control 
  timer0_fast_timer_init(TIMER0_256, 16);
  
  timer1_clock_init();
  timer1_clock_register_callback (0, 500, 1, &ledAnimate, 0, &ledTimer);
  

  
	
	async_serial_0_init(SERIAL_SPEED_9600);

	adc_set_reference_and_adj(ADC_REF_AVCC, 0);
	adc_set_channel(ADC0);
	adc_set_speed_and_enable(ADC_DIV_128, 0, 0);

	sei(); //global int enable

	async_serial_0_write_string(VT100_CURSOR_OFF);
	async_serial_0_write_string(VT100_CLEAR_SCREEN);
  while (1){
		adc_start_conversion();
		while(adc_is_busy()){}
    //1024=5V from a 100k + 220k voltage divider  
		supplyVoltage = adc_get_result();
    supplyVoltage = supplyVoltage*1600/1024;
    uint16PrintCentiNull(supplyVoltage, buffer);



		async_serial_0_write_string(VT100_CURSOR_HOME);
		async_serial_0_write_string("=== Hello world ===\r\n");
		async_serial_0_write_string(buffer);
    async_serial_0_write_string("V\r\n");
  }
  return 0;
}

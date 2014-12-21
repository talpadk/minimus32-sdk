//#exe
//#global_cflags -D MCU=ATMEGA328P

///Demonstrates using timer 1 to flash a led on and off every second

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @todo Rewrite to use the BSP functions
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "vt100_codes.h"
#include "async_serial_0.h"

volatile int clock;



ISR(TIMER1_COMPA_vect)
{
	static unsigned char max=255;
  //on/off period of 1 sec @ 16Mhz
  clock++;
  if (clock>max){
    clock=0;
		PORTB ^= 0b00100000;
  }

}


void doJob(){
	unsigned int i,j;
	for (i=0; i<60000; i++){
		for (j=0; j<60; j++){
			PORTB ^= 0b00000001;
		}
	}
	PORTB ^= 0b00100000;
}

int main(void) {
	watchdog_disable();
	
	
	PORTB &= 0b00000000; //turn off led
	DDRB  |= 0b00100000; //enable led as output
	clock_prescale_none();
  
	

	OCR1AH = 62; //set OCR1A to 16000
  OCR1AL = 128; 
  
  TCCR1A = 0; //prescaler = 1 & count up to OCR1A function 
  TCCR1B = 1<<WGM12|1<<CS10;
	
  TIMSK1 = 1<<OCIE1A; //enable timer1 int and compare a int
	
	async_serial_0_init(SERIAL_SPEED_9600);

	sei(); //global int enable

	async_serial_0_write_string(VT100_CURSOR_OFF);
	async_serial_0_write_string(VT100_CLEAR_SCREEN);
  while (1){
		async_serial_0_write_string(VT100_CURSOR_HOME);
		async_serial_0_write_string("=== Hello world ===\r\n");
		//doJob();
  }
  return 0;
}

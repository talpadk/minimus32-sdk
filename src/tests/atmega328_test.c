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
volatile int clock;



ISR(TIMER1_COMPA_vect)
{
	static unsigned char max=0;
  //on/off period of 1 sec @ 16Mhz
  clock++;
  if (clock>max){
		max++;
    clock=0;
		PORTB ^= 0b00100000;
  }

	if(max>75){
		max=0;
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
	//	unsigned int i,j;

		/*		SPH = 1;
					SPL = 0;*/

	  watchdog_disable();


		PORTB &= 0b00000000; //turn off led
		DDRB  |= 0b00100000; //enable led as output
		clock_prescale_none();
  


	OCR1AH = 62; //set OCR1A to 16000
  OCR1AL = 128; 
  
  TCCR1A = 0; //prescaler = 1 & count up to OCR1A function 
  TCCR1B = 1<<WGM12|1<<CS10;

  TIMSK1 = 1<<OCIE1A; //enable timer1 int and compare a int
	
	sei(); //global int enable
	//led_blue_toggle();
  while (1){
		//doJob();
  }
  return 0;
}

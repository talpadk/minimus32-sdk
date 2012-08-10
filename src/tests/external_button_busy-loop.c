//#exe

// Demonstrates using an external button to turn a led on the board on and off

/**
 * @file
 * @author Morten Bressendorff Schmidt
 * @ingroup tests
 */

/**
 * HW setup:
 * Connect an external button between GND and PB0
 */

#include <avr/io.h>
#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"

#define TIMER 100000

void blink(void *data) {
	led_red_toggle();
}

int main(void) {                                   // program starts here
	// Initializing the minimus
	watchdog_disable();                        // disable watchdog
	minimus32_init();                          // initialize minimus
	clock_prescale_none();                     // set clock-speed to 16MHz ;)

	//Setup a blink pattern indicating a working MCU
	timer1_clock_init();
	timer1_callback blink_callback;
	timer1_clock_register_callback(0, 500, 1, &blink, 0, &blink_callback);

	long count = 100000;                       // initializing the variable count
	DDRB &= ~(1<<DDB1);                        // PB1 input
	PORTB |= (1<<PB1);                         // pull-up resistor PB1
	while(1) {                                 // repeat forever
		if(!(PINB & (1<<PB1))) {           // if PB1 is 0V
			led_blue_toggle();         // turn the led on or of
			count = TIMER;             // set count
			while(count)               // repeat 'TIMER' times, to prevent debouncing from the button
				count--;           // count subtracted by 1
			while(!(PINB & (1<<PB1))); // wait until the button is released
		}
	}
	return 0;                                  // just by principle. the program never comes this far
}

//#exe

// Demonstrates using an external buttons to turn a led on the board on and off, and switch enable/disable a callback using interrupts

/**
 * @file
 * @author Morten Bressendorff Schmidt
 * @ingroup tests
 */

/**
 * HW setup:
 * Connect an external button between GND and PD4
 * Connect an external button between GND and PD7
 */

#include <avr/io.h>
#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "external_interrupt.h"

#define TIMER 100000

timer1_callback blink_callback;
unsigned int blink_callback_enabled = 0;
timer1_callback reenable_interrupt_INT5_callback;
timer1_callback reenable_interrupt_INT7_callback;

void reenable_interrupt_INT5() {
	enable_external_interrupt_input(INT5, falling);
}

void reenable_interrupt_INT7() {
	enable_external_interrupt_input(INT7, falling);
}

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
	timer1_clock_register_callback(0, 500, 1, &blink, 0, &blink_callback);

	DDRD &= ~(1<<DDD4);                        // PD4 input
	PORTD |= (1<<PD4);                         // pull-up resistor PD4
	enable_external_interrupt_input(INT5, falling); // enable the external interrupt on PD4

	DDRD &= ~(1<<DDD7);                        // PD7 input
	PORTD |= (1<<PD7);                         // pull-up resistor PD7
	enable_external_interrupt_input(INT7, falling); // enable the external interrupt on PD7

	while(1);
	return 0;                                  // just by principle. the program never comes this far
}

// Shortcut PD4 and GND to trigger INT5
ISR(INT5_vect) {
	// Disable the interrupt, and set timer to enable interrupt in 20 ms
	disable_external_interrupt_input(INT5);
	timer1_clock_register_callback(0, 20, 0, &reenable_interrupt_INT5, 0, &reenable_interrupt_INT5_callback);
	led_blue_toggle();                         // toggle the onboard blue led
}

// Shortcut PD7 and GND to trigger INT7
ISR(INT7_vect) {
	// Disable the interrupt, and set timer to enable interrupt in 20 ms
	disable_external_interrupt_input(INT7);
	timer1_clock_register_callback(0, 20, 0, &reenable_interrupt_INT7, 0, &reenable_interrupt_INT7_callback);
	blink_callback_enabled = !blink_callback_enabled;
	if (blink_callback_enabled) {
		timer1_clock_unregister_callback(&blink_callback);
		led_red(0);
	} else {
		timer1_clock_register_callback(0, 500, 1, &blink, 0, &blink_callback);
	}
}

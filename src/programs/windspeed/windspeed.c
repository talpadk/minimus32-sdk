//#exe
/*
 * windspeed.c
 * Record and display windspeed from a digital manometer
 * Morten Bressendorff Schmidt <smet@smet.dk>
 */

#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <stdlib.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "external_interrupt.h"
#include "lcd_44780.h"

int count = 0;

void print_count(void *data) {
	char str_count[33];
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		itoa(count, str_count, 10);
		count = 0;
	}
	lcd_44780_command(LCD_44780_GOTO_CMD+64);
	lcd_44780_print(str_count);
}

int main(void) {
	watchdog_disable();
	minimus32_init();
	// Clock-speed set to 16MHz
	clock_prescale_none();
	enable_external_interrupt_input(7, rising);
	// Initialize LCD
	lcd_44780_power_up_delay();
	lcd_44780_init();
	// Print "headline"
	lcd_44780_print("1 sec. count:");
	
	timer1_callback print_count_callback;
	timer1_clock_init();
	timer1_clock_register_callback(1, 0, 1, &print_count, 0, &print_count_callback);

	while (1) {
	}
	return 0;
}

ISR(INT7_vect) {
	// Shortcut PD7 and GND to trigger INT7
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		count++;
	}
}

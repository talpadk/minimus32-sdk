//#exe

/// Reads the temperature using a DS18B20 and displays it on a Nokia 5110 display and logging over bluetooth
/// Logging datastructure: "seconds-elapsed;temperature\n". First line a header "time;temp\n"
/// It also blinks an external LED og logging, and a DOT in the uppper left corner, to indicate a working MCU

/**
 * @file
 * @author Visti Andresen
 * @author Morten Bressendorff Schmidt
 * @ingroup programs
 */

/** 
 * @file
 *
 *
 * Hardware:<br>
 * The minimus must operate at 3.3V (or level shifters must be used)<br>
 * <br>
 * The N5110 must be supplied with 3.3V on N5110 (Vcc) with enough current<br>
 * The N5110 must be connected to GND on N5100 (GND)<br>
 * <br>
 * PB1 (SCLK)  -> N5110.7 (SCLK)<br>
 * PB2 (MOSI)  -> N5110.6 (DNK)<br>
 * <br>
 * PB4         -> N5110.3 (SCE)<br>
 * PB5         -> N5110.4 (RST)<br>
 * PB6         -> N5110.5 (D/C)<br>
 * <br>
 * Optional<br>
 * PB -> 62 Ohm -> N5110.8 (LED) I haven't found any data on the LED,<br>
 * but a current limiting resistor of 62 Ohm seems safe (~7mA) at 3.3V<br>
 * <br>
 * PC7 must be wired to the DS18B20 data pin (DQ).<br>
 * DQ must be pulled high (4.7k Ohm)<br>
 * This example requires power to be supplied to the DS18B20, parasitic power is not supported (yet)<br>
 * <br>
 * BT module: <br>
 * Vcc        -> pin 12 on BT<br>
 * GND        -> pin 13 on BT<br>
 * PD2 (RXD1) -> pin 1 on BT<br>
 * PD3 (TXD1) -> pin 2 on BT<br>
 * <br>
 * LED:<br>
 * GND -> LED -> 1K resistor -> PD0<br>
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <string.h>
#include <stdio.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "onewire.h"
#include "timer1_clock.h"
#include "ds18s20_blocking.h"
#include "vt100_codes.h"
#include "async_serial.h"

#include "spi.h"
#include "pcd8544.h"
#include "vertical_byte_font_6x8.h"
#include "vertical_byte_font_12x16.h" // Font is 0-9 and other mappings are :=.  ;=-  <=[space]  ==[degrees celsius]   

const uint8_t LOGINTERVAL = 10;

// Mapping for font 12x16
const char DOT = ':';
const char MINUS = ';';
const char SPACE = '<';
const char CELCIUS = '=';

char loggerTemp[7] = "";

void pull_low (void) {
	DDRC  |= 1<<7;
	PORTC &= ~(1<<7);
}

void release (void) {
	DDRC &= ~(1<<7);
}

uint8_t get_bit(void){
	if (PINC & (1<<7))
		return 1<<7;
	else
		return 0;
}

ISR(INT4_vect) {
	onewire_interrupt();
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

timer1_callback dummyLoggingLedCallback;
void blinkLoggingLedOff() {
	timer1_clock_unregister_callback(&dummyLoggingLedCallback);
	PORTD &= 0b11111110; // turn off loggingLed
}

void blinkLoggingLed() {
	timer1_clock_register_callback(0, 150, 1, &blinkLoggingLedOff, 0, &dummyLoggingLedCallback);
	PORTD |= 0b00000001; // turn on loggingLed
}

void async_serial_send_time() {
	timer1_wall_time time;
	timer1_clock_get_time(&time);

	char time_buffer[10] = "";
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		sprintf(time_buffer, "%d", (time.freerunning_sec-LOGINTERVAL));
	}
	async_serial_write_string(time_buffer);
}

onewire_rom_code rom_code;
ds18s20_scratchpad scratchpad;
uint8_t calculateCRC(uint8_t *buffer, uint8_t length){
	uint8_t crc = 0;
	uint8_t tmp;
	uint8_t i, j, input_bit;
	for (i=0; i<length; i++) {
		tmp = buffer[i];
		for (j=0; j<8; j++){
			input_bit = (crc ^ tmp) & 1; // first xor

			if (input_bit) crc = crc ^ 0x18; // xor the top line in the crc (minus the last part)

			crc = crc >> 1; // the cyclic part
			tmp = tmp >> 1;
			crc = crc | (input_bit<<7); // add the missing part
		}
	}
	return crc;
}

char *lefttrim(char *str) {
	while (*str == ' ') str++;
	return str;
}

char runningDot_[2] = " ";
void runningDotForMCU(void *data) {
	runningDot_[0] = runningDot_[0] == ' ' ? '*' : ' ';
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		pcd8544_print(0, 0, runningDot_, &vertical_byte_font_6x8);
	}
}

uint8_t printState_ = 0;
void printTemp(void *data) {
	// Run every 500ms, making it update every second
	// Wait a minute! Up to 750ms to convert? Does this end up right then?
  	int i;
	int decimal;
	if (printState_ == 0) {
		// Conversion
		printState_ = 1;
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			ds18s20_blocking_start_conversion(&rom_code);
		}
	} else {
		// Readout
		printState_ = 0;
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			ds18s20_blocking_read_scratchpad(&rom_code, &scratchpad);
		}
		
		int16_t temp = (scratchpad.temperature_msb<<8)+scratchpad.temperature_lsb;

		char sign = ' ';
		if (temp < 0) {
			sign = '-';
			temp = -temp;
		}
		
		decimal = temp & 0b1111;              // masking
		decimal = (decimal * 100) / 16;       // conversion of decimal part to int value range 99-0 (not all values used)
		if ((decimal%10) >= 5) decimal += 10; // rounding up
		decimal /= 10;                        // rounding down
		
		temp = temp>>4;                       // remove decimal part
		if (decimal == 10) {                  // rounding up
		  temp++;
		  decimal = 0;
		}

		char buffer[7];
		buffer[6] = 0; // Null termination
		buffer[5] = CELCIUS;
		buffer[4] = decimal+'0'; // the decimal
		buffer[3] = DOT;

		for (i=2; i>=0; i--){
			buffer[i] = (temp%10)+'0';
			temp /= 10;
		}

		// Remove leading 0's unless 0.x
		for (i=0; i<=2; i++){
			if (buffer[i+1] == DOT) {
				break;
			}
			if (buffer[i] == '0') {
				buffer[i] = SPACE;
			} else {
				break;
			}
		}

		// Add - if negative	
		if (sign == '-') {
			for (int i=1; i<7; i++) {
				if (buffer[i] != SPACE) {
					buffer[i-1] = MINUS;
					break;
				}
			}
		}

		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			strcpy(loggerTemp, buffer);
			pcd8544_print(8, 1, buffer, &vertical_byte_font_12x16);
		}
	}
}

void printLogged(void *data) {
	// Don't log temp if not set...
	if (!strlen(loggerTemp)) {
		return;
	}
	// Fix font-mapping
	for (uint8_t i=0; i<=5; i++) {
		if (loggerTemp[i] == SPACE) {
			loggerTemp[i] = ' ';
		} else if (loggerTemp[i] == DOT) {
			loggerTemp[i] = '.';
		} else if (loggerTemp[i] == MINUS) {
			loggerTemp[i] = '-';
		} else if (loggerTemp[i] == CELCIUS) {
			loggerTemp[i] = ' ';
		}
	}

	async_serial_send_time();
	async_serial_write_byte(';');
	async_serial_write_string(lefttrim(loggerTemp));
	async_serial_write_string("\r\n");
	
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		pcd8544_print(54, 5, loggerTemp, &vertical_byte_font_6x8);
	}
	blinkLoggingLed();
}

void ds18b20_init(void) {
	onewire_init(4, &pull_low, &release, &get_bit);
	onewire_wait_idle();
	onewire_send_byte(0x33);
	onewire_wait_idle();

	for (uint8_t b=0; b<7; b++) {
		onewire_start_read_byte();
		onewire_wait_idle();
		rom_code.rom_code[b] = onewire_get_buffer();
	}

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.crc = onewire_get_buffer();
}

int main(){
	watchdog_disable();
	minimus32_init();
	clock_prescale_none();

	sei(); // interupts on
	timer1_clock_init();

	// Input/output
	DDRB |= 0b11110000; // IO setup for the reset, cs, cmd, led
	DDRD |= (1<<DDD0); // PD0 loggingLed as output

	// BT-card
	async_serial_init(SERIAL_SPEED_9600);
	async_serial_write_string(VT100_CURSOR_OFF);
	async_serial_write_string(VT100_CLEAR_SCREEN);
	async_serial_write_string(VT100_CURSOR_HOME);
	async_serial_write_string("time;temp\n\r");

	// Display
	spi_config_io_for_master_mode();
	pcd8544_init(&pcdIO, 55);

	// Logging LED
	blinkLoggingLedOff();

	pcd8544_fill_screen(0); // Clear screen

	// DS18B20
	ds18b20_init();

	timer1_clock_init();
	timer1_callback dummyRunningDotCallback; // Blinking dot, to indicate a working MCU 
	timer1_clock_register_callback(1, 0, 1, &runningDotForMCU, 0, &dummyRunningDotCallback);

	timer1_callback dummyPrintCallback; // Printing the temperature
	timer1_clock_register_callback(0, 500, 1, &printTemp, 0, &dummyPrintCallback);

	timer1_callback dummyLoggedCallback; // Logging through BT
	timer1_clock_register_callback(LOGINTERVAL, 0, 1, &printLogged, 0, &dummyLoggedCallback);

	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		pcd8544_print(4, 5, "Logged:  init", &vertical_byte_font_6x8);
	}
	printTemp(0);
	printLogged(0);

	while (1) {
	}
	return 0;
}

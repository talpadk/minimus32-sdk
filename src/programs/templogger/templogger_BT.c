//#exe

/// Reads the temperature using a DS18B20 and displays it on a Nokia 5110 display and logging over bluetooth
/// Logging datastructure: "time-elapsed;temp\n"

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
 *
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
 * PB -> 62 Ohm -> N5110.8 (LED) I haven't found any data on the LED,
 * but a current limiting resistor of 62 Ohm seems safe (~7mA) at 3.3V<br>
 *
 * PC7 must be wired to the DS18B20 data pin (DQ).<br>
 * DQ must be pulled high (4.7k Ohm)
 * This example requires power to be supplied to the DS18B20, parasitic power is not supported (yet)
 * 
 * BT module: 
 * Vcc        -> pin 12 on BT
 * GND        -> pin 13 on BT
 * PD2 (RXD1) -> pin 1 on BT
 * PD3 (TXD1) -> pin 2 on BT
 * 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
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
#include "vertical_byte_font_12x16.h" // :=.  ;=-  <=[space]  ==[degrees celsius]   

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
	PORTB |= 0b00000001; // turn off loggingLed
}

void blinkLoggingLed() {
	timer1_clock_register_callback(0, 150, 1, &blinkLoggingLedOff, 0, &dummyLoggingLedCallback);
	PORTB &= 0b11111110; // turn on loggingLed
}

void async_serial_send_time() {
	timer1_wall_time time;
	timer1_clock_get_time(&time);

	char time_buffer[10] = "";
	sprintf(time_buffer, "%d", time.freerunning_sec);
	async_serial_write_string(time_buffer);
}

onewire_rom_code rom_code;
uint8_t rom_crc;
ds18s20_scratchpad scratchpad;

uint8_t calculateCRC(uint8_t *buffer, uint8_t length){
	uint8_t crc=0;
	uint8_t tmp;
	uint8_t i,j,input_bit;
	for (i=0; i<length; i++) {
		tmp = buffer[i];
		for (j=0; j<8; j++){
			input_bit = (crc ^ tmp) & 1;//first xor

			if (input_bit) crc = crc ^ 0x18; //xor the top line in the crc (minus the last part)

			crc = crc >> 1; //the cyclic part
			tmp = tmp >> 1;
			crc = crc | (input_bit<<7); //add the missing part
		}
	}
	return crc;
}

char *lefttrim(char *str) {
	int i=0;
	while(strlen(str) > 0 && str[0] == ' ') {
		for(i=1; i<strlen(str); i++) {
			str[i-1] = str[i];
			str[strlen(str)] = '\0';
		}
		return(str);
	}
	return "";
}

uint8_t printState_ = 0;
uint8_t printStateInit_ = 0;
void printTemp(void *data){
	int i;
	if (printState_ == 0) {
		//Conversion
		printState_ = 1;
		ds18s20_blocking_start_conversion(&rom_code);
		if (printStateInit_ == 0) {
			pcd8544_print(54, 5, "init", &vertical_byte_font_6x8);
			printStateInit_ = 1;
		}
	} else {
		//Readout
		printState_ = 0;
		ds18s20_blocking_read_scratchpad(&rom_code, &scratchpad);
		int16_t temp = (scratchpad.temperature_msb<<8)+scratchpad.temperature_lsb;

		char sign = ' ';
		if (temp < 0) {
			sign = '-';
			temp = -temp;
		}
		
		// Correction for the difference from DS18S20 to DS18B20
		// Doesn't do propper rounding, it only rounds down... but so what?
		temp=temp>>3; 

		char buffer[7];
		buffer[6] = 0; //Null termination
		buffer[5] = '='; // Actually "degrees C" due to font-mapping
		buffer[4] = (temp%10)+'0'; // the decimal
		buffer[3]=':'; // Actually a . due to the font-mapping

		temp = temp>>1;
		for (i=2; i>=0; i--){
			buffer[i]=(temp%10)+'0';
			temp /= 10;
		}

		// Remove leading 0's
		for (i=0; i<=2; i++){
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

		strcpy(loggerTemp, buffer);
		pcd8544_print(8, 1, buffer, &vertical_byte_font_12x16);
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
	
	pcd8544_print(54, 5, loggerTemp, &vertical_byte_font_6x8);
	blinkLoggingLed();
}

int main(){
	watchdog_disable();
	minimus32_init();
	clock_prescale_none();

	sei(); //interupts on
	timer1_clock_init();

	// BT-card
	async_serial_init(SERIAL_SPEED_9600);
	async_serial_write_string(VT100_CURSOR_OFF);
	async_serial_write_string(VT100_CLEAR_SCREEN);
	async_serial_write_string(VT100_CURSOR_HOME);
	async_serial_write_string("time;temp\n\r");

	// Display
	spi_config_io_for_master_mode();
	DDRB |= 0b11110001; //IO setup for the reset,cs, cmd, led and loggingLed pin
	pcd8544_init(&pcdIO, 55);

	// Logging LED
	blinkLoggingLedOff();

	pcd8544_fill_screen(0); // Clear screen

	// DS18B20
	onewire_init(4, &pull_low, &release, &get_bit);
	onewire_wait_idle();
	onewire_send_byte(0x33);
	onewire_wait_idle();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[0]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[1]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[2]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[3]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[4]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[5]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.rom_code[6]=onewire_get_buffer();

	onewire_start_read_byte();
	onewire_wait_idle();
	rom_code.crc=onewire_get_buffer();

	rom_crc = calculateCRC(rom_code.rom_code, 7);

	timer1_clock_init();
	timer1_callback dummyPrintCallback;
	timer1_clock_register_callback(0, 500, 1, &printTemp, 0, &dummyPrintCallback);

	timer1_callback dummyLoggedCallback;
	timer1_clock_register_callback(10, 0, 1, &printLogged, 0, &dummyLoggedCallback);

	pcd8544_print(4, 5, "Logged:", &vertical_byte_font_6x8);
	printTemp(0);
	printLogged(0);

	while (1) {
	}
	return 0;
}

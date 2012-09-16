
//#exe

///Reads the temperatur using a DS18S20 and displays it on a Nokia 5110 display and logging over bluetooth

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
 * 16x2 display wired according to LCD_HelloWorld.c<br>
 * PC7 must be wired to the DS18S20 data pin (DQ).<br>
 * DQ must be pulled high (4.7k Ohm)
 * This example requires power to be supplied to the DS18S20, parasitic power is not supported (yet)
 * 
 */
#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

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

char loggerTemp[6] = "";

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

void async_serial_send_time() {
	char * time_buffer = "Dxx hh:mm:ss.mmm";
	timer1_wall_time time;
	timer1_clock_get_time(&time);

	time_buffer[1] = (time.day/10)%10+'0';
	time_buffer[2] = (time.day%10)+'0';

	time_buffer[4] = (time.hour/10)+'0';
	time_buffer[5] = (time.hour%10)+'0';

	time_buffer[7] = (time.min/10)+'0';
	time_buffer[8] = (time.min%10)+'0';

	time_buffer[10] = (time.sec/10)+'0';
	time_buffer[11] = (time.sec%10)+'0';

	time_buffer[13] = (time.msec/100)+'0';
	time_buffer[14] = ((time.msec%100)/10)+'0';
	time_buffer[15] = (time.msec%10)+'0';
	
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


uint8_t printState_=0;
void printTemp(void *data){
        int i;
	if (printState_==0){
	  //Conversion
	  printState_=1;
	  ds18s20_blocking_start_conversion(&rom_code);
	}
	else {
	  //Readout
	  printState_=0;
	  ds18s20_blocking_read_scratchpad(&rom_code, &scratchpad);
	  int16_t temp = (scratchpad.temperature_msb<<8)+scratchpad.temperature_lsb;

	  char sign = ' ';
	  if (temp < 0) {
	    sign = '-';
	    temp = (~temp)+1; // * -1
	  }

	  char buffer[6];
	  buffer[5] = '=';
	  if (temp&1)
	    buffer[4]='5';
	  else
	    buffer[4]='0';
	
	  buffer[3]=':'; // Actually a . due to the font-mapping
	
	  temp = temp>>1;
	  //TODO fix this... don't divide by 8.... find the issue
	  //temp=temp/8;
	  for (i=2; i>=0; i--){
	    buffer[i]=(temp%10)+'0';
	    temp /= 10;
	  }

	  // Remove leading 0's
	  for (i=0; i<=2; i++){
	    if (buffer[i] == '0') {
	      buffer[i] = '<'; // Actually space
	    } else {
	      break;
	    }
	  }
	
	  // Add - if negative	
	  if (sign == '-') {
	    buffer[0] = ';'; // Actually -
	  }
	  
	  strcpy(loggerTemp, buffer);

	  pcd8544_print(8, 1, buffer, &vertical_byte_font_12x16);
	}
}

void printLogged(void *data) {
	// Fix font-mapping
	for (uint8_t i=0; i<=5; i++) {
		if (loggerTemp[i] == '<') {
			loggerTemp[i] = ' ';
		} else if (loggerTemp[i] == ':') {
			loggerTemp[i] = '.';
		} else if (loggerTemp[i] == ';') {
			loggerTemp[i] = '-';
		} else if (loggerTemp[i] == '=') {
			loggerTemp[i] = ' ';
		}
	}

	async_serial_send_time();
	async_serial_write_byte(';');
	async_serial_write_string(loggerTemp);
	async_serial_write_string("\r\n");

	pcd8544_print(54, 5, loggerTemp, &vertical_byte_font_6x8);
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
	async_serial_write_string("#templogger-START\n\r");

	// Display
	spi_config_io_for_master_mode();
	DDRB |= 0b11110000; //IO setup for the reset,cs, cmd and led pin
	pcd8544_init(&pcdIO, 55);

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

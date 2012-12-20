//#exe

/// Reads the temperature using a DS18B20 and displays it on a Nokia 5110 display and logging over bluetooth
/// Logging datastructure: "seconds-elapsed;temperature\n". First line a header "time;temp\n"
/// It also blinks an external LED og logging, and a DOT in the uppper left corner, to indicate a working MCU

/**
 * @file
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
 * PB -> 62 Ohm -> N5110.8 (LED) I haven't found any data on the displays LEDs,<br>
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
 * <br>
 * RELAY:<br>
 * Between GND and PD1
 * 
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

// Defines
#define PROGRAM_VALUE_LENGTH 8

// Mapping for font 12x16
const char DOT = ':';
const char MINUS = ';';
const char SPACE = '<';
const char CELCIUS = '=';

char nowTemp[7] = " init ";
char loggerTemp[7] = "      ";
char programValue[PROGRAM_VALUE_LENGTH];
uint8_t programPointer = 0;
// All 4 program_X variables are in "deci-seconds"
//TODO: Allow negative temperatures in code below
volatile int16_t program_c = 0;
volatile int16_t program_d = 0;
volatile int16_t program_t = 0;
volatile int16_t program_w = 0;
// Interval in full seconds between logging
volatile uint16_t program_log = 10;
char programChar[2] = " ";
char runningChar[2] = " ";
uint8_t alwaysRun = 0;

void pull_low (void) {
	DDRC  |= 1<<7;
	PORTC &= ~(1<<7);
}

void release (void) {
	DDRC &= ~(1<<7);
}

uint8_t get_bit(void){
	if (PINC & (1<<7)) {
		return 1<<7;
	} else {
		return 0;
	}
}

void assertReset() { PORTB &= ~(1<<PB5); }
void desertReset() { PORTB |=  (1<<PB5); }
void assertCS() { PORTB &= ~(1<<PB4); }
void desertCS() { PORTB |=  (1<<PB4); }
void assertCMD() { PORTB &= ~(1<<PB6); }
void desertCMD() { PORTB |=  (1<<PB6); }

pcd8544_io pcdIO = {
	assertReset,
	desertReset,
	assertCS,
	desertCS,
	assertCMD,
	desertCMD  
};

// Blinking the logging LED
timer1_callback dummyLoggingLedCallback;
void blinkLoggingLedOff() {
	timer1_clock_unregister_callback(&dummyLoggingLedCallback);
	PORTD &= 0b11111110; // turn off loggingLed
}
void blinkLoggingLed() {
	timer1_clock_register_callback(0, 150, 1, &blinkLoggingLedOff, 0, &dummyLoggingLedCallback);
	PORTD |= 0b00000001; // turn on loggingLed
}

// Enable/disable the relay
void relayStatus(uint8_t on) {
	if (on == 1) {
		PORTD |= 0b00000010; // turn on the relay (PD1)
	} else {
		PORTD &= 0b11111101; // turn off the relay (PD1)
	}
}

// Timers for the relay....
timer1_callback dummyStartWaitingCallback;
void stopWaiting() {
	timer1_clock_unregister_callback(&dummyStartWaitingCallback);
	runningChar[0] = ' ';
}
void startWaiting() {
	runningChar[0] = 'W';
	timer1_clock_register_callback(program_w/10, 0, 1, &stopWaiting, 0, &dummyStartWaitingCallback);
}
timer1_callback dummyStartRelayTimerCallback;
void stopRelayTimer() {
	timer1_clock_unregister_callback(&dummyStartRelayTimerCallback);
	relayStatus(0);
	startWaiting();
}
void startRelayTimer() {
	relayStatus(1);
	timer1_clock_register_callback(program_t/10, 0, 1, &stopRelayTimer, 0, &dummyStartRelayTimerCallback);
	runningChar[0] = 'O';
}

// Send the timestamp to the serial connection
void async_serial_send_time() {
	timer1_wall_time time;
	timer1_clock_get_time(&time);

	char time_buffer[10];
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		sprintf(time_buffer, "%d", (time.freerunning_sec-program_log));
	}
	async_serial_write_string(time_buffer);
}

onewire_rom_code rom_code;
ds18s20_scratchpad scratchpad;

// Trim all spaces from the left
char *lefttrim(char *str) {
	while (*str == ' ') str++;
	return str;
}

// Fix the mappings for the 12x16-font
void fixFontMapping(char *out, char *in) {
	for (uint8_t i=0; i<strlen(in); i++) {
		if (in[i] == SPACE) {
			out[i] = ' ';
		} else if (in[i] == DOT) {
			out[i] = '.';
		} else if (in[i] == MINUS) {
			out[i] = '-';
		} else if (in[i] == CELCIUS) {
			out[i] = ' ';
		} else {
			out[i] = in[i];
		}
	}
}

// Remove leading 0's unless 0.x
void removeLeadingZeros(char *out, char *in, uint8_t fixMap) {
	for (uint8_t i=0; i<strlen(in); i++) {
		if (in[i+1] == '.' || in[i+1] == DOT) {
			break;
		}
		out[i] = (in[i] == '0') ? (fixMap ? SPACE : ' ') : in[i];
	}
}

// Next update is what?
typedef enum {
	_CONVERT,
	_READOUT,
} UpdateState;
UpdateState updateState = _CONVERT;
void updateTemp() {
  	int i;
	int decimal;
	if (updateState == _CONVERT) {
		// Conversion
		updateState = _READOUT;
		ATOMIC_BLOCK(ATOMIC_FORCEON) {
			ds18s20_blocking_start_conversion(&rom_code);
		}
	} else {
		// Readout
		updateState = _CONVERT;
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
			strcpy(nowTemp, buffer);
		}
	}
}

void updateLogged() {
	// Fix font-mapping
	fixFontMapping(loggerTemp, nowTemp);

	async_serial_send_time();
	async_serial_write_byte(';');
	async_serial_write_string(lefttrim(loggerTemp));
	async_serial_write_string("\n");
	
	blinkLoggingLed();
}

void ds18b20_init(void) {
	onewire_init(&pull_low, &release, &get_bit);
	onewire_send_byte(ONEWIRE_READ_ROM);

	for (uint8_t b=0; b<7; b++) {
		rom_code.rom_code[b] = onewire_read_byte();
	}

	rom_code.crc = onewire_read_byte();
}

void setRelay() {
	if (alwaysRun) {
		relayStatus(1);
		runningChar[0] = '+';
	} else if (programChar[0] == 'R' && runningChar[0] == ' ') {
//TODO: Test
/*
		int16_t deciTemp = 0;
		uint8_t negative = 0;
		uint8_t dotFound = 0;
		for (uint8_t c=0; nowTemp[c] != '\0'; c++) {
			if (nowTemp[c] == SPACE) {
				dotFound++;
			} else if (nowTemp[c] == DOT) {
				dotFound++;
			} else if (nowTemp[c] == MINUS) {
				negative++;
			} else {
				deciTemp = deciTemp*10 + nowTemp[c]-'0';
			}
		}
		if (dotFound == 0) {
			deciTemp = deciTemp * 10;
		}
		if (negative > 0) {
			deciTemp = deciTemp * -1;
		}
*/
		char deciTemp[] = "      ";
		for (uint8_t c=0; c<6; c++) {
			// Don't use special-chars from fontmapping
			if (nowTemp[c] == SPACE || nowTemp[c] == CELCIUS) {
				continue;
			}
			if (nowTemp[c] == MINUS) {
				deciTemp[c] = '-';
				continue;
			}
			if (nowTemp[c] != DOT && nowTemp[c] != '.') {
				deciTemp[c] = nowTemp[c];
				continue;
			}
			deciTemp[c] = nowTemp[c+1];
			deciTemp[c+1] = '\0';
			break;
		}
		if (atoi(deciTemp) <= (program_c - program_d)) {
			startRelayTimer();
		} else {
			relayStatus(0);
		}
	}
}

void setProgramVariable(char* str) {
	if (str[0] != 'R' && str[0] != 'S' && str[0] != '+' && str[0] != '-' && str[1] != ':') {
		return;
	}
	char type = str[0];
	char output[8];
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		strncpy(output, str, 7);
		sprintf(output, "%-7s", output);
		output[7] = '\0';
	}
	str = str + 2;
	uint16_t deciTemp = 0;
	uint8_t dotFound = 0;
	for (uint8_t c=0; str[c] != '\0'; c++) {
		if (str[c] != '.') {
			deciTemp = deciTemp*10 + str[c]-'0';
		} else {
			dotFound++;
		}
	}
	// More than one dot? That's a no-no!
	if (dotFound > 1) {
		return;
	}
	// Times 10, if no decimal found. Must be deci-seconds, except for program_log
	if (!dotFound && type != 'L') {
		deciTemp = deciTemp*10;
	}
	uint8_t changeFound = 0;
	switch (type) {
		case '+':
			alwaysRun = 1;
			relayStatus(1);
		break;
		case '-':
			alwaysRun = 0;
		case 'S':
			// Deactivate program
			programChar[0] = ' ';
			relayStatus(0);
			stopWaiting();
		break;
		case 'R':
			if (program_c != 0 && program_d != 0 && program_t != 0 && program_w != 0) {
				programChar[0] = 'R';
			}
		break;
		case 'C':
			if (program_c != deciTemp) {
				changeFound = 1;
				ATOMIC_BLOCK(ATOMIC_FORCEON) { program_c = deciTemp; }
			}
		break;
		case 'D':
			if (program_d != deciTemp) {
				changeFound = 1;
				ATOMIC_BLOCK(ATOMIC_FORCEON) { program_d = deciTemp; }
			}
		break;
		case 'T':
			if (program_t != deciTemp) {
				changeFound = 1;
				ATOMIC_BLOCK(ATOMIC_FORCEON) { program_t = deciTemp; }
			}
		break;
		case 'W':
			if (program_w != deciTemp) {
				changeFound = 1;
				ATOMIC_BLOCK(ATOMIC_FORCEON) { program_w = deciTemp; }
			}
		break;
		case 'L':
			ATOMIC_BLOCK(ATOMIC_FORCEON) { program_log = deciTemp; }
		break;
	}
	if (changeFound) {
		setRelay();
	}
}

typedef enum {
	_NONE,
	_1SEC,
	_LOG,
} Machinestates;
uint16_t _stateCounter = 0;
void statemachine(void *data) { // Called every 10 ms/0.01s
	_stateCounter++;
	Machinestates _state = _NONE;
	if (_stateCounter % (program_log * 100) == 0) {
		_state = _LOG;
		_stateCounter = 0;
	} else if (_stateCounter % 100 == 0) {
		_state = _1SEC;
	}

	switch(_state) {
		case _1SEC: // Show the MCU is running, fetch program, and print the temperature. Check relay, and flash W
			updateTemp();
			setRelay(); // Always run updateTemp() first, it sets the temperature to log
		break;
		case _LOG: // Show the MCU is running, fetch program, print the temperature log set the relay it too.
			updateTemp();
			updateLogged(); // Always run updateTemp() first, it sets the temperature to log
			setRelay();
		break;
		case _NONE: // Do nothing
		default:
		break;
	}
}

void prepareDeciPrint(char *str, uint16_t value) {
	str[5] = 0;
	for (int8_t i=4; i>=0; i--) {
		if (i == 3) {
			str[i--] = '.';
		}
		str[i] = (value%10)+'0';
		value = value/10;
	}
}

char runningDot_[2] = " ";
uint8_t runningDotCount_ = 0;
void updateDisplay() {
	char buffer[6];
	if (program_c != 0) {
		prepareDeciPrint(buffer, program_c);
		removeLeadingZeros(buffer, buffer, 0);
		pcd8544_print(4, 4, ":", &vertical_byte_font_6x8);
		pcd8544_print(0, 4, "C", &vertical_byte_font_6x8);
		pcd8544_print(8, 4, buffer, &vertical_byte_font_6x8);
	}
	if (program_d != 0) {
		prepareDeciPrint(buffer, program_d);
		removeLeadingZeros(buffer, buffer, 0);
		pcd8544_print(50, 4, ":", &vertical_byte_font_6x8);
		pcd8544_print(44, 4, "D", &vertical_byte_font_6x8);
		pcd8544_print(54, 4, buffer, &vertical_byte_font_6x8);
	}
	if (program_t != 0) {
		prepareDeciPrint(buffer, program_t);
		removeLeadingZeros(buffer, buffer, 0);
		pcd8544_print(4, 5, ":", &vertical_byte_font_6x8);
		pcd8544_print(0, 5, "T", &vertical_byte_font_6x8);
		pcd8544_print(8, 5, buffer, &vertical_byte_font_6x8);
	}
	if (program_w != 0) {
		prepareDeciPrint(buffer, program_w);
		removeLeadingZeros(buffer, buffer, 0);
		pcd8544_print(50, 5, ":", &vertical_byte_font_6x8);
		pcd8544_print(44, 5, "W", &vertical_byte_font_6x8);
		pcd8544_print(54, 5, buffer, &vertical_byte_font_6x8);
	}
	
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		sprintf(buffer, "%-4d", program_log);
	}
	pcd8544_print(4, 3, ":", &vertical_byte_font_6x8);
	pcd8544_print(0, 3, "L", &vertical_byte_font_6x8);
	pcd8544_print(8, 3, buffer, &vertical_byte_font_6x8);

	pcd8544_print(0, 1, programChar, &vertical_byte_font_6x8);
	pcd8544_print(0, 2, runningChar, &vertical_byte_font_6x8);

	if (nowTemp[5] != ' ') {
		pcd8544_print(8, 1, nowTemp, &vertical_byte_font_12x16);
	}
	pcd8544_print(54, 0, loggerTemp, &vertical_byte_font_6x8);

	if (runningDotCount_++ == 25) {
		runningDot_[0] = runningDot_[0] == ' ' ? '*' : ' ';
		pcd8544_print(0, 0, runningDot_, &vertical_byte_font_6x8);
		runningDotCount_ = 0;
	}
}

ISR(USART1_RX_vect) {
	// Fetch one byte
	char byte = async_serial_read_byte();
	// Parse, if wee need to!
	if (byte == ';' || byte == '\n' || byte == '\r' || programPointer >= PROGRAM_VALUE_LENGTH-1) {
		programValue[programPointer] = 0;
		setProgramVariable(programValue);
		programPointer = 0;
	}
	// Add the byte to the buffer 
	if (byte != ';' && byte != '\n' && byte != '\r') {
		programValue[programPointer++] = byte;
	}
}

// Toggle onboard blue LED to indicate running MCU, called by interrupt
void toggleBlueLed(void *data) {
	led_blue_toggle();
}

int main() {
	char buffer[4];
	uint8_t crc;

	watchdog_disable();
	minimus32_init();
	clock_prescale_none();
	
	async_serial_init(SERIAL_SPEED_9600);

	sei(); // interupts on
	timer1_clock_init();

	// Input/output
	DDRB |= 0b11110000; // IO setup for the reset, cs, cmd, led
	DDRD |= (1<<DDD0); // PD0 loggingLed as output
	DDRD |= (1<<DDD1); // PD1 is output for the relay
	DDRD |= (1<<DDD3); // PD3 is output for BT-module

	// Display
	spi_config_io_for_master_mode();
	pcd8544_init(&pcdIO, 65);

	// Logging LED
	blinkLoggingLedOff();

	// Turn of the relay
	relayStatus(0);

	// Clear screen
	pcd8544_fill_screen(0);

	// DS18B20
	ds18b20_init();

	// Print the logging time interval
	char l[6];
	char strBuffer[4];
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		itoa(program_log, strBuffer, 10);
		sprintf(l, "L:%s", strBuffer);
	}
	setProgramVariable(l);

	// Not atomic... noone else writes at this point
	pcd8544_print(10, 0, "Logged: init", &vertical_byte_font_6x8);
	
	updateTemp(0);
	updateLogged(0);

	// BT-card
	async_serial_write_string(VT100_CURSOR_OFF);
	async_serial_write_string(VT100_CLEAR_SCREEN);
	async_serial_write_string(VT100_CURSOR_HOME);
	async_serial_write_string("time;temp\n");

	// Setting up the timer for the statemachine
	timer1_clock_init();
	timer1_callback dummyStatemachineCallback;
	timer1_clock_register_callback(0, 10, 1, &statemachine, 0, &dummyStatemachineCallback);

	// Toggle onboard blue LED to indicate running MCU
	timer1_callback dummyRunningMCUCallback;
	timer1_clock_register_callback(0, 250, 1, &toggleBlueLed, 0, &dummyRunningMCUCallback);

	// Enable interrupt for the serial connection
	async_serial_rx_interrupt(1);
	
	// Run the main loop, and do ALL the magic
	crc = onewire_calculate_crc(rom_code.rom_code, 7);
	if (rom_code.crc != crc) {
		buffer[3] = 0;
		buffer[2] = (crc%10)+'0'; crc/=10;
		buffer[1] = (crc%10)+'0'; crc/=10;
		buffer[0] = (crc)+'0';
		while (1) {
			_delay_ms(200);
			pcd8544_print(0, 1, buffer, &vertical_byte_font_6x8);
			pcd8544_print(0, 0, "ROM CRC Error!", &vertical_byte_font_6x8);
			_delay_ms(200);
			pcd8544_print(0, 1, "   ", &vertical_byte_font_6x8);
			pcd8544_print(0, 0, "              ", &vertical_byte_font_6x8);
		}
	}

	while (1) {
		_delay_ms(10);
		updateDisplay();
	}
	return 0;
}

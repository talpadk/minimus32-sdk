//#exe

///Reads the temperatur using a DS18S20 and displays it on a LCD

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
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


#include <avr/io.h>
#include <avr/interrupt.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "lcd_44780.h"
#include "onewire.h"
#include "timer1_clock.h"
#include "ds18s20_blocking.h"

const char backslash[8] = {0b00000000,
			   0b00010000,
			   0b00001000,
			   0b00000100,
			   0b00000010,
			   0b00000001,
			   0b00000000,
			   0b00000000};

void pull_low (void) {
  DDRC  |= 1<<7;
  PORTC &= ~(1<<7);
}

void release (void) {
  DDRC &= ~(1<<7);
}

uint8_t get_bit(void){
  if (PINC & (1<<7)) return 1<<7;
  else return 0;
}

ISR(INT4_vect) {
  onewire_interrupt();
}

void readAddress(){
  onewire_send_byte(0x33);
}

void writeHexNibble(uint8_t data){
  if (data<10) lcd_44780_write_byte(data+'0', 0);
  else  lcd_44780_write_byte(data+'A'-10, 0);
}

void writeHex(uint8_t byte){
  writeHexNibble(byte>>4);
  writeHexNibble(byte&0x0F);
}

onewire_rom_code rom_code;
uint8_t rom_crc;
void writeRomCode(void){
  writeHex(rom_code.crc);
  writeHex(rom_code.rom_code[6]);
  writeHex(rom_code.rom_code[5]);
  writeHex(rom_code.rom_code[4]);
  writeHex(rom_code.rom_code[3]);
  writeHex(rom_code.rom_code[2]);
  writeHex(rom_code.rom_code[1]);
  writeHex(rom_code.rom_code[0]);
}

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


int16_t temp=0;

void printTemp(void){
  int i;
  int16_t tmp=temp;
  char buffer[8];
  buffer[7]=0;
  if (tmp&1) buffer[6]='5';
  else buffer[6]='0';
  buffer[5]='.';
  tmp=tmp>>1;
    
  for (i=4; i>=0; i--){
    buffer[i]=(tmp%10)+'0';
    tmp /= 10;
  }
  lcd_44780_print(buffer);
}

int main(){
  char anim=0;
  char msg=0;
  int i=0, j=0;
  ds18s20_scratchpad scratchpad;

  onewire_state bus_state;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  sei(); //ints on
  timer1_clock_init();
  

  lcd_44780_power_up_delay();
  lcd_44780_init();

  lcd_44780_set_custom_char(1, backslash);

  onewire_init(4, &pull_low, &release, &get_bit);
  //  pull_low();
  //release();

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

  while (1){
    lcd_44780_command(LCD_44780_GOTO_CMD);    
    switch (msg){
    case 0:
      lcd_44780_print("  DS18S20 Demo  ");
      if (i==0){
	ds18s20_blocking_start_conversion(&rom_code);
      }
      break;
    case 1:
      lcd_44780_print("CRC+ROM Code:   ");
      break;
    case 2:
      writeRomCode();
      break;
    case 3:
      if (rom_crc==rom_code.crc) lcd_44780_print("ROM CRC: OK     ");
      else                      lcd_44780_print("ROM CRC: FAILED!");
      break;
    case 4:
      lcd_44780_print(" TCurr  TFilter ");
      lcd_44780_command(LCD_44780_GOTO_CMD+64);  
      if (i==0){
	ds18s20_blocking_read_scratchpad(&rom_code, &scratchpad);
	temp = (scratchpad.temperature_msb<<8)+scratchpad.temperature_lsb;
	printTemp();
      }
      break;
    }
    i++;
    if (i>500){
      msg++;
      if (msg>4) msg=0;
      i=0;
    }



    bus_state = onewire_get_state();
    if (bus_state == onewire_error_presence_pulse_missing){
      lcd_44780_command(LCD_44780_GOTO_CMD+64);
      lcd_44780_print(" Presence ERROR ");
    }
    else {
      //Show spinning anim
      lcd_44780_command(LCD_44780_GOTO_CMD+64+15);
      j++;
      if (j>30){
	anim++;
	if (anim>3) anim = 0;
	j=0;
      }
      
      switch (anim){
      case 0:
	lcd_44780_write_byte('|', 0);
	break;
      case 1:
	lcd_44780_write_byte('/', 0);
	break;
      case 2:
	lcd_44780_write_byte('-', 0);
	break;
      case 3:
	lcd_44780_write_byte(1, 0);
      }
    }

  }
}

//#exe

///Reads the temperatur and displays it on a LCD

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @ingroup tests_display
 * @ingroup tests_sensor
 */

/** 
 * @file
 * Proof of concept semi/util functions for reading
 * the temperature from a TC77 sensor.
 *
 * @todo Write a BSP file for handling the SPI bus access
 * @todo Wirte a peripherals file for the TC77
 *
 * Hardware:<br>
 * 16x2 display wired according to LCD_HelloWorld.c<br>
 * PB4 must be wired to the TC77 chip select (CS).<br>
 * PB1 must be wired to the TC77 clock pin (CLK)<br>
 * PB3 (and optionally PB2(not used right now))
 * must be wired to the TC77 data pin (SI/O)<br>
 * 
 */


#include <avr/io.h>

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "lcd_44780.h"

void tempStringClean(char *string){
  unsigned char i;
  for (i=1; i<3; i++){
    if (string[i]!='0') break;
    else string [i]=' ';
  }
  if (string[0]=='-'){
    string[0]=' ';
    string[i-1]='-';
  }
}


void tempToString(int16_t temp, char string[8]){
  int16_t buf;
  if (temp<0){
    temp = -temp;
    string[0]='-';
  }
  else string[0]=' ';
  buf = temp>>4;
  string[3]=buf%10+'0'; buf/=10;
  string[2]=buf%10+'0'; buf/=10;
  string[1]=buf%10+'0';
  string[4]='.';
  buf = (100*(temp & 0b1111))/16;
  string[6]=buf%10+'0'; buf/=10;
  string[5]=buf%10+'0';
  string[7]=0;
}

int16_t tempFilter = 0;

void readTemp(void *userData){
  char string[8];
  int16_t temp;

  //TC77 max clock = 7 MHZ
  //Master, MSB first, clock low=idle, sample on rising edge, divide the clock by 4
  SPCR = 0b01010000;

  //IO setup
  //SPI master forces MISO (PB3) as input
  //PB2 master out(MUST be INPUT), setting PB3 as it won't hurt
  DDRB &= 0b11110011;
  //Chip select on PB4,  PB0 (SS) as output to prevent going into slave mode  and PB1 CLK
  DDRB |= 0b00010011;

  //Chip Sel
  PORTB &= 0b11101111;

  //Initiate transfer
  SPDR = 0;
  //Wait for transfer done
  while (!(SPSR&1<<SPIF)){}
  //clear Int flag by reading (as we don't use a ISR, and we need the data)
  temp = SPDR<<5;

  //Initiate 2. transfer
  SPDR = 0;
  //Wait for transfer done
  while (!(SPSR&1<<SPIF)){}
  //clear Int flag by reading (as we don't use a ISR, and we need the data)
  temp |= (SPDR&0b11111000)>>3;

  //fix sign
  if (temp&(1<<12)) temp = -((1<<13)-temp);

  //Chip unsel
  PORTB |= 0b00010000;  


  //write the display
  lcd_44780_command(LCD_44780_GOTO_CMD+64);

  tempToString(temp, string);
  tempStringClean(string);
  string[6]=0;
  lcd_44780_print(string);

  tempFilter = (tempFilter*7+temp)>>3;
  tempToString(tempFilter, string);
  tempStringClean(string);
  string[6]=0;
  lcd_44780_print(string);

}

const char backslash[8] = {0b00000000,
			   0b00010000,
			   0b00001000,
			   0b00000100,
			   0b00000010,
			   0b00000001,
			   0b00000000,
			   0b00000000};

int main(){
  char anim=0;
  char msg=1;
  int i=0, j=0;
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  lcd_44780_power_up_delay();
  lcd_44780_init();

  lcd_44780_set_custom_char(1, backslash);


  while (1){
    lcd_44780_command(LCD_44780_GOTO_CMD);    
    if (msg) lcd_44780_print(" TC77 Temp demo");
    else  lcd_44780_print(" TCurr  TFilter");
    i++;
    if (i>500){
      msg = msg^1;
      i=0;
    }

    readTemp(0);



    //Show spinning anim
    j++;
    if (j>30){
      anim++;
      if (anim>3) anim = 0;
      j=0;
    }
    lcd_44780_write_byte(' ', 0);

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

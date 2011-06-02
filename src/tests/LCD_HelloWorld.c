//#exe
#include <avr/io.h>
#include <avr/power.h>

#include "minimus32.h"
#include "sys_clock.h"

/** 
 * @file LCD_HelloWorld.c
 * 
 * A 44780 based LCD demo
 *
 * PD0 = DB4
 *
 * PD1 = DB5
 *
 * PD2 = DB6
 *
 * PD3 = DB7
 *
 * PD4 = RS
 *
 * PD5 = RW
 *
 * PD6 = E
 * 
 */

void PORT_Init() {
  PORTD = 0b00000000;
  DDRD  = 0b01111111;		//set as output
}

void delay(int loops){
  for (; loops>0; loops--){}
}


#define LCD_BIT_RS (1<<4)
#define LCD_BIT_RW (1<<5)
#define LCD_BIT_E (1<<6)

#define LCD_CMD (0)
#define LCD_DATA (LCD_BIT_RS)


void writeLCDNible(uint8_t data, uint8_t type){
  PORTD = type;
  delay(500);
  PORTD = type|(data&0x0f);
  delay(500);
  PORTD = LCD_BIT_E|type|(data&0x0f);
  delay(5000);
  PORTD = type|(data&0x0f);
  delay(5000);
}

void writeLCDByte(uint8_t data, uint8_t type){
  writeLCDNible(data>>4, type);
  delay(5000);
  writeLCDNible(data&0x0f, type);
}

int main(void) {
  PORT_Init();
  asm("WDR"); //Watchdog Timer Reset
  
  MCUSR= ~(1<<WDRF);
  WDTCSR = (1<<WDCE) | (1<<WDE);
  WDTCSR = 0x00;
  WDTCKD = 0x00;
  

  delay(20000);	delay(20000);	delay(20000);	delay(20000);	delay(20000);	delay(20000);
  writeLCDNible(3, LCD_CMD);
  delay(20000);
  writeLCDNible(3, LCD_CMD);
  delay(20000);
  writeLCDNible(3, LCD_CMD);
  //4 bit
  delay(20000);
  writeLCDNible(2, LCD_CMD);
  
  //Function set
  writeLCDByte(0x28, LCD_CMD);
  //turn off display
  writeLCDByte(0x08, LCD_CMD);
  
  writeLCDByte(0x06, LCD_CMD);
  
  writeLCDByte(0x01, LCD_CMD);
  writeLCDByte(0x0c, LCD_CMD);
  
  writeLCDByte('H', LCD_DATA);
  writeLCDByte('e', LCD_DATA);
  writeLCDByte('l', LCD_DATA);
  writeLCDByte('l', LCD_DATA);
  writeLCDByte('o', LCD_DATA);
  writeLCDByte(' ', LCD_DATA);
  writeLCDByte('W', LCD_DATA);
  writeLCDByte('o', LCD_DATA);
  writeLCDByte('r', LCD_DATA);
  writeLCDByte('l', LCD_DATA);
  writeLCDByte('d', LCD_DATA);
  
  
  while (1) {
    
  }
  return 0;
}

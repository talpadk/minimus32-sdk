#include <avr/io.h>

#include "lcd_44780.h"


#define LCD_BIT_RS (1<<4)
#define LCD_BIT_RW (1<<5)
#define LCD_BIT_E (1<<6)

#define LCD_CMD (0)
#define LCD_DATA (LCD_BIT_RS)

void lcd_44780_delay(unsigned int loops)
{
  for (; loops>0; loops--){}
}

void lcd_44780_data_in(){
  DDRD &= 0b11110000;
}

void lcd_44780_data_out(){
  DDRD |= 0b00001111;
}

void lcd_44780_command(const uint8_t command){
  lcd_44780_write_byte(command, 1);
}

uint8_t lcd_44780_read_nibble(uint8_t type){
  uint8_t result;
  //one clock at 16MHz i 62.5 nS
  PORTD = type|LCD_BIT_RW;
  //min 40nS delay here code delay is enough
  PORTD = LCD_BIT_E|type|LCD_BIT_RW;
  //min 220nS delay holding E line active (just to be on the safe side)
  asm volatile("nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       ::);
  result = PIND & 0b1111;
  PORTD = type|LCD_BIT_RW;
  //The 10nS holding RS and R/W line handled by code delay
  //Also the E cycle time of 500ns should be caused by other code delays

  return result;
}

void lcd_44780_wait_for_display(){
  char busy=1;
  //lcd_44780_delay(8000);

  lcd_44780_data_in();
  
  while(busy){
 
    //500nS E cycle delay
    asm volatile("nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 ::);
    busy =  lcd_44780_read_nibble(LCD_CMD) & 0b1000;
    //500nS E cycle delay
    asm volatile("nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 "nop\n\t"
		 ::);
    lcd_44780_read_nibble(LCD_CMD);
  }
  
  lcd_44780_data_out();
}

void lcd_44780_write_nibble(uint8_t data, uint8_t type){
  //one clock at 16MHz i 62.5 nS
  PORTD = type|(data&0x0f);
  //min 40nS delay here code delay is enough
  PORTD = LCD_BIT_E|type|(data&0x0f);
  //min 220nS delay holding E line active (just to be on the safe side)
  asm volatile("nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       "nop\n\t"
	       ::);
  PORTD = type|(data&0x0f);
  //The 10nS holding RS and R/W line handled by code delay
  //Also the E cycle time of 500ns should be caused by other code delays
}

void lcd_44780_write_byte(uint8_t data, char is_command)
{
  uint8_t type=LCD_DATA;
  if (is_command) type=LCD_CMD;

  lcd_44780_wait_for_display();

  lcd_44780_write_nibble(data>>4, type);
  lcd_44780_delay(50);
  lcd_44780_write_nibble(data&0x0f, type);
}

void lcd_44780_init()
{
  PORTD &=~0b01111111;
  DDRD  |= 0b01111111;	      
  
  lcd_44780_write_nibble(3, LCD_CMD);
  lcd_44780_delay(20000);
  lcd_44780_write_nibble(3, LCD_CMD);
  lcd_44780_delay(20000);
  lcd_44780_write_nibble(3, LCD_CMD);
  //4 bit
  lcd_44780_delay(20000);
  lcd_44780_write_nibble(2, LCD_CMD);
  
  //Function set
  lcd_44780_command(0x28);
  //turn off display
  lcd_44780_command(0x08);
  
  lcd_44780_command(0x06);
  
  lcd_44780_command(0x01);
  lcd_44780_command(0x0c);
}

void lcd_44780_power_up_delay()
{
  lcd_44780_delay(65000);
  lcd_44780_delay(65000);
  lcd_44780_delay(65000);
  lcd_44780_delay(65000);
  lcd_44780_delay(65000);
  lcd_44780_delay(65000);
}

void lcd_44780_print(const char * string)
{
  while (*string != 0){
    lcd_44780_write_byte(*string, 0);
    string++;
  }
}

void lcd_44780_set_custom_char(uint8_t index, const char graphics[8]){
  lcd_44780_command((index<<3)+64);
  lcd_44780_write_byte(graphics[0], 0);
  lcd_44780_write_byte(graphics[1], 0);
  lcd_44780_write_byte(graphics[2], 0);
  lcd_44780_write_byte(graphics[3], 0);
  lcd_44780_write_byte(graphics[4], 0);
  lcd_44780_write_byte(graphics[5], 0);
  lcd_44780_write_byte(graphics[6], 0);
  lcd_44780_write_byte(graphics[7], 0);

}

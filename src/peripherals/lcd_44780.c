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

void lcd_44780_command(const uint8_t command){
  lcd_44780_write_byte(command, 1);
}

void lcd_44780_write_nibble(uint8_t data, uint8_t type){
  PORTD = LCD_BIT_E|type|(data&0x0f);
  lcd_44780_delay(8000);
  PORTD = type|(data&0x0f);
  lcd_44780_delay(5);
}

void lcd_44780_write_byte(uint8_t data, char is_command)
{
  uint8_t type=LCD_DATA;
  if (is_command) type=LCD_CMD;

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

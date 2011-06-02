#ifndef MOD_LCD_44780_H
#define MOD_LCD_44780_H

#include <stdint.h>

/** 
 * Initializes the LCD, must be called after the display have had time to power up 
 * 
 * @see lcd_44780_power_up_delay
 */
void lcd_44780_init();

/** 
 * Pauses the CPU for more than enough time for the LCD to power up
 * 
 */
void lcd_44780_power_up_delay();

void lcd_44780_command(const uint8_t command);

void lcd_44780_print(const char * string);
void lcd_44780_write_byte(uint8_t data, char is_command);


#endif //MOD_LCD_44780_H

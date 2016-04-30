#ifndef MOD_LCD_44780_H
#define MOD_LCD_44780_H

///Code for using a Hitachi 44780 compatible character LCD

/**
 * @file
 * @author Visti Andresen
 * @ingroup per_displays
 */

/**
 * @file
 * @todo Use the read pin to speed-up the display code by polling the ready state.
 * @todo Update the code to allow the user to select which pins are used.
 */


#include <stdint.h>

///A command that moves the cursor to a different position, the offset is added to the command, the offset depends on the size of the display line 2 on a 16x2 display is LCD_44780_GOTO_CMD+64
#define LCD_44780_GOTO_CMD (0x80)

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

/** 
 * Sends a command byte to the display
 * 
 * @param command the byte to send
 */
void lcd_44780_command(const uint8_t command);

/** 
 * Prints a null terminated string to the current cursor position
 * 
 * @param string the string to print
 */
void lcd_44780_print(const char * string);

/** 
 * Sends a byte to the display, the byte may be a command or a data byte
 * 
 * @param data the byte to send
 * @param is_command if true send the byte as a command else send it as data
 */
void lcd_44780_write_byte(uint8_t data, char is_command);

/** 
 * Set the graphics for a custom character
 * 
 * NOTICE: This function leaves the text cursor inside the CGRAM
 * 
 * @param index The character index to set (range: 0-7).
 * @param graphics The graphics for that character.
 */
void lcd_44780_set_custom_char(uint8_t index, const char graphics[8]);

#endif //MOD_LCD_44780_H

#ifndef LCD_ILI9341_h
#define LCD_ILI9341_h

#include <stdint.h>
#include "io_function.h"
#include "spi.h"
#include "bitfont.h"
#include "image565.h"

/**
 * @file   lcd_ili9341.h
 * @author Visti Andresen <talpa@Hermes.talpa.dk>
 * @date   Tue May 13 18:26:27 2014
 * 
 * @brief  A driver for SPI based ILI9341 QVGA displays
 * 
 * @include per
 * @ingroup per_displays
 * 
 */

//Minimum clock period 100ns(10MHz write), 150ns(6,67MHz read) 
//colour format(R:5-bit, G:6-bit, B:5-bit)

#define ILI9341_COLOUR_WHITE (0xffff)
#define ILI9341_COLOUR_BLACK (0)
#define ILI9341_COLOUR_RED   (0b1111100000000000)
#define ILI9341_COLOUR_GREEN (0b0000011111100000)
#define ILI9341_COLOUR_BLUE  (0b0000000000011111)

///Swaps the cols and rows, with the cheap QVGA displays this is landscape mode
#define ILI9341_DISPLAY_MODE_LANDSCAPE (1<<5)

///Normal ordering cols=x and rows=y
#define ILI9341_DISPLAY_MODE_PORTRAIT (0)

///Write the "x" pixles backwards in memory (default needed for the cheap QVGA displays in portrait mode.
#define ILI9341_DISPLAY_MODE_FLIP_X (1<<6)

///Write the "y" pixles backwards in memory. 
#define ILI9341_DISPLAY_MODE_FLIP_Y (1<<7)

///Used with displays that have a RGB pixel layout
#define ILI9341_DISPLAY_MODE_RGB (0)

///Used with displays that have a BGR pixel layout (seems to be the case with the cheap one from china)
#define ILI9341_DISPLAY_MODE_BGR (1<<3)


typedef struct {
  io_outFunction setChipSelect;
  io_outFunction setDataCommand;
} lcd_ili9341_device;

/** 
 * Obtains the SPI bus for use by the driver.
 * The SPI bus must be obtained before using any functions in this driver 
 * 
 * @param blocking if true the function only returns once the bus have been obtained
 * 
 * @return true if the bus could be obtained
 */
uint8_t lcd_ili9341_obtainBus(uint8_t blocking);

/** 
 * Releases the SPI bus
 * 
 */
void lcd_ili9341_releaseBus(void);

/** 
 * Selectes a device that the API talks to and activates its chip select pin
 * 
 * @see lcd_ili9341_obtainBus
 * @see lcd_ili9341_deselectDevice
 *
 * @param device the device definition that defines the device
 */
void lcd_ili9341_selectDevice(lcd_ili9341_device *device);

/** 
 * Deselectes a device, call this when not talking to the device for extended periods and prior to releasing the bus
 * 
 * @param device 
 */
void lcd_ili9341_deselectDevice(lcd_ili9341_device *device);


//void lcd_ili9341_readDisplayId(uint8_t *manufacturer, uint8_t *driverVersion, uint8_t *driver);

/** 
 * Initialises the display after power on, you must obtain the bus and select the device prior to calling this
 * @see lcd_ili9341_obtainBus
 * @see lcd_ili9341_selectDevice
 * 
 */
void lcd_ili9341_init();


/** 
 * Changes the display mode, used for switching from portarit to landscape
 *
 * @param mode ILI9341_DISPLAY_MODE_* flags
 */
void lcd_ili9341_setDisplayMode(uint8_t mode);

/** 
 * Sets the X address range of the next display region low level write
 * 
 * @param start the x coordinate of the first pixle in the region 
 * @param end the x coordinate of the last pixle in the region 
 */
void lcd_ili9341_setXAddress(uint16_t start, uint16_t end);

/** 
 * Sets the Y address range of the next display region low level write
 * 
 * @param start the y coordinate of the first pixle in the region 
 * @param end the y coordinate of the last pixle in the region 
 */
void lcd_ili9341_setYAddress(uint16_t start, uint16_t end);


/** 
 * Writes N  pixels to the display
 * 
 * @param colour the colour of the pixels to set
 * @param countN the number N
 */
void lcd_ili9341_fillN(uint16_t colour, uint16_t countN);

/** 
 * Writes N timer M pixels to the display
 * 
 * @param colour the colour of the pixels to set
 * @param countN the number N
 * @param countM the number M
 */
void lcd_ili9341_fillNTimesM(uint16_t colour, uint16_t countN, uint16_t countM);

/** 
 * Fills the entire display with a colour
 * 
 * @param colour the colour to use for filling
 */
void lcd_ili9341_fill(uint16_t colour);

/** 
 * Draws a vertical line on the display
 * 
 * @param colour the colour of the line
 * @param x the x coordinate of the line
 * @param startY the y coordinate of the upper pixel of the line
 * @param endY the y coordinate of the lower pixel of the line
 */
void lcd_ili9341_drawVerticalLine(uint16_t colour, uint16_t x, uint16_t startY, uint16_t endY);

/** 
 * Draws a horizontal line on the display
 * 
 * @param colour the colour of the line
 * @param y the y coordinate of the line
 * @param startX the x coordinate of the left most pixel of the line
 * @param endX the x coordinate of the right most pixel of the line
 */
void lcd_ili9341_drawHorizontalLine(uint16_t colour, uint16_t y, uint16_t startX, uint16_t endX);

/** 
 * Draws a filled rectangle
 * 
 * @param colour the colour to draw the rectangle in
 * @param startX the x coordinate of the upper left corner of the rectangle
 * @param endX the x coordinate of the lower right corner of the rectangle
 * @param startY the y coordinate of the upper left corner of the rectangle
 * @param endY the y coordinate of the lower right corner of the rectangle
 */
void lcd_ili9341_drawFilledRectangle(uint16_t colour, uint16_t startX, uint16_t endX, uint16_t startY, uint16_t endY);

/** 
 * Draw a single character onto the display
 * 
 * @param x the x coordinate for the upper left corner of the character
 * @param y the y coordinate for the upper left corner of the character
 * @param character the character to draw
 * @param font the font to render the string using
 * @param fgHigh the high byte of the forground colour
 * @param fgLow  the low byte of the forground colour
 * @param bgHigh the high byte of the background colour
 * @param bgLow the low byte of the background colour
 */
void lcd_ili9341_drawBitFontChar(uint8_t x, uint8_t y, uint8_t character, const bitfont *font, uint8_t fgHigh, uint8_t fgLow, uint8_t bgHigh, uint8_t bgLow);

/** 
 * Draws a string onto the display using a bitfont
 * 
 * @param x the x coordinate for the upper left corner of the string
 * @param y the y coordinate for the upper left corner of the string
 * @param string the null terminated string to draw
 * @param font the font to render the string using
 * @param fg the forground colour
 * @param bg the background colour
 */
void lcd_ili9341_drawBitFontString(uint8_t x, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg);

/** 
 *  Draws a string centered onto a region of the display using a bitfont
 * 
 * @param x the x start of the region 
 * @param width the width of the region, must be wide enough to contain the string
 * @param y the y coordinate for the upper left corner of the string
 * @param string the null terminated string to draw
 * @param font the font to render the string using
 * @param fg the forground colour
 * @param bg the background colour
 */
void lcd_ili9341_drawBitFontCenteredString(uint8_t x, uint16_t width, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg);

/** 
 * Draws a image565 onto the display
 * 
 * @param x the x coordinate for the upper left corner in the image
 * @param y the y coordinate for the upper left corner in the image
 * @param image the image to draw
 */
void lcd_ili9341_drawImage565(uint16_t x, uint16_t y, const image565 *image);

#endif //LCD_ILI9341_h

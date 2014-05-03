#ifndef LCD_ILI9341_h
#define LCD_ILI9341_h

#include <stdint.h>
#include "io_function.h"
#include "spi.h"
#include "bitfont.h"
#include "image565.h"

//min clock 100ns(10MHz write), 150ns(6,67MHz read) 
//colour format(R:5-bit, G:6-bit, B:5-bit)

#define ILI9341_COLOUR_WHITE (0xffff)
#define ILI9341_COLOUR_BLACK (0)
#define ILI9341_COLOUR_RED   (0b1111100000000000)
#define ILI9341_COLOUR_GREEN (0b0000011111100000)
#define ILI9341_COLOUR_BLUE  (0b0000000000011111)

typedef struct {
  io_outFunction setChipSelect;
  io_outFunction setDataCommand;
} lcd_ili9341_device;

uint8_t lcd_ili9341_obtainBus(uint8_t blocking);

void lcd_ili9341_releaseBus(void);

void lcd_ili9341_selectDevice(lcd_ili9341_device *device);
void lcd_ili9341_deselectDevice(lcd_ili9341_device *device);


void lcd_ili9341_readDisplayId(uint8_t *manufacturer, uint8_t *driverVersion, uint8_t *driver);

void lcd_ili9341_init();

void lcd_ili9341_setColumnAddress(uint16_t start, uint16_t end);
void lcd_ili9341_setRowAddress(uint16_t start, uint16_t end);

void lcd_ili9341_fillN(uint16_t colour, uint16_t countN);
void lcd_ili9341_fillNTimesM(uint16_t colour, uint16_t countN, uint8_t countM);
void lcd_ili9341_fill(uint16_t colour);

void lcd_ili9341_drawColumnLine(uint16_t colour, uint8_t column, uint16_t startRow, uint16_t endRow);
void lcd_ili9341_drawRowLine(uint16_t colour, uint16_t row, uint8_t startColumn, uint8_t endColumn);

void lcd_ili9341_drawFilledRectangle(uint16_t colour, uint8_t startColumn, uint8_t endColumn, uint16_t startRow, uint16_t endRow);

void lcd_ili9341_bitFontDrawChar(uint8_t x, uint8_t y, uint8_t character, const bitfont *font, uint8_t fgHigh, uint8_t fgLow, uint8_t bgHigh, uint8_t bgLow);
void lcd_ili9341_bitFontDrawString(uint8_t x, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg);

void lcd_ili9341_drawImage565(uint16_t x, uint16_t y, const image565 *image);

#endif //LCD_ILI9341_h

#ifndef _ILI9341_TEXT_MENU_H_
#define _ILI9341_TEXT_MENU_H_

#include <stdint.h>
#include "bitfont.h"
#include "lcd_ili9341.h"

typedef struct {
  const char * menuText;
} ili9341_TextMenuItem;

typedef struct {
  const bitfont *font;
  const uint16_t bgColour;
  const uint16_t menuColour;
  const uint16_t fontColour;
  const uint8_t padding;
  
  const ili9341_TextMenuItem *items;
  
  uint8_t selectedElement;
  uint8_t elementCount;
  uint8_t widestElement;
} ili9341_TextMenu;

void ili9341_textMenuInit(ili9341_TextMenu *menu);
void ili9341_textMenuDraw(ili9341_TextMenu *menu, uint16_t screenWidth, uint16_t screenHeight);
#endif /* _ILI9341_TEXT_MENU_H_ */

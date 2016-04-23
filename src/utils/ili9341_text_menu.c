#include "ili9341_text_menu.h"

void ili9341_textMenuInit(ili9341_TextMenu *menu){
  const ili9341_TextMenuItem *items = menu->items;
  uint8_t width;
  const char *string;
    
  menu->selectedElement = 0;
  menu->elementCount = 0;
  menu->widestElement = 0;
  while (items->menuText!=0){
    string = items->menuText;
    width = 0;
    while (*string != 0){
      width++;
      string++;
    }
    if (width > menu->widestElement){
      menu->widestElement = width;
    }
    menu->elementCount++;
    items++;
  }
}

void ili9341_textMenuDraw(ili9341_TextMenu *menu, uint16_t screenWidth, uint16_t screenHeight){
  uint16_t totWidth, totHeight;
  uint16_t x,y;
  const ili9341_TextMenuItem *items = menu->items;
  
  lcd_ili9341_drawFilledRectangle(menu->bgColour, 0, 319, 0, 239);
  totHeight = menu->padding*2+menu->font->height*menu->elementCount;
  totWidth  = menu->padding*2+menu->font->width*menu->widestElement;
  lcd_ili9341_drawFilledRectangle(menu->menuColour,
				  screenWidth/2-totWidth/2,  screenWidth/2-totWidth/2+totWidth,
  				  screenHeight/2-totHeight/2, screenHeight/2-totHeight/2+totHeight);
  x = menu->padding+screenWidth/2-totWidth/2;
  y = menu->padding+screenHeight/2-totHeight/2;
  while (items->menuText!=0){
    lcd_ili9341_drawBitFontString(x,y, items->menuText, menu->font, menu->fontColour, menu->menuColour);
    y+=menu->font->height;
    items++;
  }
}

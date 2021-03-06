#include "lcd_ili9341.h"
#include "ili9341_defines.h"
#include <avr/pgmspace.h> 

io_outFunction lcd_ili9341_setDataCommand_;

void lcd_ili9341_selectDevice(lcd_ili9341_device *device){
  device->setChipSelect(io_PIN_LOW);
  lcd_ili9341_setDataCommand_ = device->setDataCommand;
  lcd_ili9341_setDataCommand_(io_PIN_HIGH);
}

void lcd_ili9341_deselectDevice(lcd_ili9341_device *device){
  device->setChipSelect(io_PIN_HIGH);
}

void lcd_ili9341_sendCommand(uint8_t command){
  lcd_ili9341_setDataCommand_(io_PIN_LOW);
  spi_io(command);
  lcd_ili9341_setDataCommand_(io_PIN_HIGH);  
}

void lcd_ili9341_sendCommandAsyncBegin(uint8_t command){
  lcd_ili9341_setDataCommand_(io_PIN_LOW);
  spi_async_io_begin(command);
  spi_async_io_wait();
  lcd_ili9341_setDataCommand_(io_PIN_HIGH);  
}


uint8_t lcd_ili9341_obtainBus(uint8_t blocking){
  if (spi_obtain_bus(blocking)){
    //8 MHz @16MHz sys clock (write only clock speed)
    spi_setup(SPI_DIVIDER_2, SPI_FLAG_BUS_MASTER|SPI_FLAG_CLOCK_RISING|SPI_FLAG_ENABLE_SPI|SPI_FLAG_MSB_FIRST);
    return 1;
  }
  else {
    return 0;
  }
}

void lcd_ili9341_releaseBus(void){
  spi_release_bus();
}


void lcd_ili9341_readDisplayId(uint8_t *manufacturer, uint8_t *driverVersion, uint8_t *driver){
  lcd_ili9341_sendCommand(ILI9341_CMD_READ_DISPLAY_IDENTIFICATION_INFORMATION);
  *manufacturer  = spi_io(0);
  *driverVersion = spi_io(0);
  *driver        = spi_io(0);
}

void lcd_ili9341_init(){

  //Default reset values
  lcd_ili9341_sendCommand(ILI9341_CMD_POWER_CONTROL_A);
  spi_io(0x39);
  spi_io(0x2C);
  spi_io(0x00);
  spi_io(0x34);
  spi_io(0x02);
  

  lcd_ili9341_sendCommand(ILI9341_CMD_POWER_CONTROL_B);
  spi_io(0x00);
  spi_io(0XC1); //PCEQ+DRV_ena
  spi_io(0X30); //DC_ena (esd prot)
  
  lcd_ili9341_sendCommand(ILI9341_CMD_DRIVER_TIMING_CONTROL_A1);
  spi_io(0x85); //no overlapping gate drive
  spi_io(0x00);
  spi_io(0x78); //2 unit precharge
  
  lcd_ili9341_sendCommand(ILI9341_CMD_DRIVER_TIMING_CONTROL_B);
  spi_io(0x00);
  spi_io(0x00);
  
  lcd_ili9341_sendCommand(ILI9341_CMD_POWER_ON_SEQUENCE_CONTROL);
  spi_io(0x64);
  spi_io(0x03);
  spi_io(0X12);
  spi_io(0X81); //DDVDH enhance mode(only for 8 external capacitors)
  
  lcd_ili9341_sendCommand(ILI9341_CMD_PUMP_RATIO_CONTROL);
  spi_io(0x20); //DDVDH=2xVCI
  
  lcd_ili9341_sendCommand(ILI9341_CMD_POWER_CONTROL_1);
  spi_io(0x23); //VRH[5:0] (4.6V)
  
  lcd_ili9341_sendCommand(ILI9341_CMD_POWER_CONTROL_2);
  spi_io(0x10); //SAP[2:0];BT[3:0] 0x10 is valid?
  
  lcd_ili9341_sendCommand(ILI9341_CMD_VCOM_CONTROL_1);
  spi_io(0x3e); //Contrast (high = 4.25V) 
  spi_io(0x28); //(low = -1.50v)
  
  lcd_ili9341_sendCommand(ILI9341_CMD_VCOM_CONTROL_2);
  spi_io(0x86); //--
  
  lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
  spi_io(0x48); //Coloumn address order + BGR
  
  lcd_ili9341_sendCommand(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
  spi_io(0x55); //16bit rgb and mcu, default is 18
  
  lcd_ili9341_sendCommand(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL_MODE);
  spi_io(0x00); //no clock division
  spi_io(0x18); //78 Hz
  
  //incompleate command?
  lcd_ili9341_sendCommand(ILI9341_CMD_DISPLAY_FUNCTION_CONTROL);
  spi_io(0x08);
  spi_io(0x82);
  spi_io(0x27);
  
  
  lcd_ili9341_sendCommand(ILI9341_CMD_ENABLE_3G);
  spi_io(0x00); //disable should be 0b10?
  
  lcd_ili9341_sendCommand(ILI9341_CMD_GAMMA_SET);
  spi_io(0x01); //curve 1
  
  lcd_ili9341_sendCommand(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION); //Set Gamma
  spi_io(0x0F);
  spi_io(0x31);
  spi_io(0x2B);
  spi_io(0x0C);
  spi_io(0x0E);
  spi_io(0x08);
  spi_io(0x4E);
  spi_io(0xF1);
  spi_io(0x37);
  spi_io(0x07);
  spi_io(0x10);
  spi_io(0x03);
  spi_io(0x0E);
  spi_io(0x09);
  spi_io(0x00);
  
  lcd_ili9341_sendCommand(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION); //Set Gamma
  spi_io(0x00);
  spi_io(0x0E);
  spi_io(0x14);
  spi_io(0x03);
  spi_io(0x11);
  spi_io(0x07);
  spi_io(0x31);
  spi_io(0xC1);
  spi_io(0x48);
  spi_io(0x08);
  spi_io(0x0F);
  spi_io(0x0C);
  spi_io(0x31);
  spi_io(0x36);
  spi_io(0x0F);
  
  lcd_ili9341_sendCommand(ILI9341_CMD_SLEEP_OUT); 
  //delay(120);
  
  lcd_ili9341_sendCommand(ILI9341_CMD_DISPLAY_ON); 
  //lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_WRITE);
}

/*
 * Note: Internally the flip x and y bits are swaped when selecting landscape mode.
 * This is done as the lanscape mode swaps the cols and rows around
 */
void lcd_ili9341_setDisplayMode(uint8_t mode){
  uint8_t data=mode;
  lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
  if (mode&ILI9341_DISPLAY_MODE_LANDSCAPE){
    //Swaps the row and col flip bits for landscape
    data &= ~(ILI9341_DISPLAY_MODE_FLIP_X|ILI9341_DISPLAY_MODE_FLIP_Y);
    data |= (mode&ILI9341_DISPLAY_MODE_FLIP_X)<<1;
    data |= (mode&ILI9341_DISPLAY_MODE_FLIP_Y)>>1;
  }
  spi_io(data);
}

void lcd_ili9341_setXAddress(uint16_t start, uint16_t end){
  lcd_ili9341_sendCommand(ILI9341_CMD_COLUMN_ADDRESS_SET);
  spi_io(start >> 8);
  spi_io(start & 0xff);
  spi_io(end   >> 8);
  spi_io(end   & 0xff);
}

void lcd_ili9341_setYAddress(uint16_t start, uint16_t end){
  lcd_ili9341_sendCommand(ILI9341_CMD_PAGE_ADDRESS_SET);
  spi_io(start >> 8);
  spi_io(start & 0xff);
  spi_io(end   >> 8);
  spi_io(end   & 0xff);
}

void lcd_ili9341_fillN(uint16_t colour, uint16_t countN){
  uint8_t colourHigh = colour >> 8;
  uint8_t colourLow  = colour & 0xff;

  lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_WRITE);
  for (; countN!=0; countN--){
    spi_io(colourHigh);
    spi_io(colourLow);
  }
}

void lcd_ili9341_fillNTimesM(uint16_t colour, uint16_t countN, uint16_t countM){
  uint8_t colourHigh = colour >> 8;
  uint8_t colourLow  = colour & 0xff;
  uint16_t i;

  lcd_ili9341_sendCommandAsyncBegin(ILI9341_CMD_MEMORY_WRITE);
  for (; countN!=0; countN--){
    for (i=countM; i!=0; i--){
      spi_async_io(colourHigh);
      spi_async_io(colourLow);
    }
  }
  spi_async_io_end();
}

void lcd_ili9341_fill(uint16_t colour){
  lcd_ili9341_setXAddress(0,239);
  lcd_ili9341_setYAddress(0,339);

  lcd_ili9341_fillNTimesM(colour, 320, 240);
}

void lcd_ili9341_drawVerticalLine(uint16_t colour, uint16_t x, uint16_t startY, uint16_t endY){
  lcd_ili9341_setXAddress(x,x);
  lcd_ili9341_setYAddress(startY, endY);
  lcd_ili9341_fillN(colour, endY-startY+1);
}


void lcd_ili9341_drawHorizontalLine(uint16_t colour, uint16_t y, uint16_t startX, uint16_t endX){
  lcd_ili9341_setXAddress(startX, endX);
  lcd_ili9341_setYAddress(y,y);
  lcd_ili9341_fillN(colour, endX-startX+1);
}

void lcd_ili9341_drawFilledRectangle(uint16_t colour, uint16_t startX, uint16_t endX, uint16_t startY, uint16_t endY){
  lcd_ili9341_setYAddress(startY, endY);
  lcd_ili9341_setXAddress(startX, endX);
  lcd_ili9341_fillNTimesM(colour,  endX-startX+1, endY-startY+1);
}

void lcd_ili9341_drawPixle(uint16_t colour, uint16_t x, uint16_t y);
  uint8_t colourHigh = colour >> 8;
  uint8_t colourLow = colour & 0xff;

  lcd_ili9341_setYAddress(y,y);
  lcd_ili9341_setXAddress(x,x);
  lcd_ili9341_fillN(colour, 1){
  /*lcd_ili9341_sendCommandAsyncBegin(ILI9341_CMD_MEMORY_WRITE);
  spi_async_io(colourHigh);
  spi_async_io(colourLow);
  spi_async_io_end();*/
}

void lcd_ili9341_drawBitFontChar(uint8_t x, uint8_t y, uint8_t character, const bitfont *font, uint8_t fgHigh, uint8_t fgLow, uint8_t bgHigh, uint8_t bgLow){
  const uint8_t *bits = font->bits+((uint16_t)font->char_size)*((uint16_t)(character-font->start));
  uint8_t currentByte;
  uint16_t bitCount = font->width*font->height;
  uint8_t bytePos;

  //Set up region
  lcd_ili9341_setXAddress(x, x+font->width-1);
  lcd_ili9341_setYAddress(y, y+font->height-1);

  //Send data 
  lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_WRITE);

  bytePos=8;
  currentByte = pgm_read_byte(bits++);
  while (bitCount!=0){
    bitCount-=1;
    if (currentByte & 128) {
      spi_io(fgHigh);
      spi_io(fgLow);
    }
    else {
      spi_io(bgHigh);
      spi_io(bgLow);
    }
    bytePos-=1;
    if (bytePos==0){
      bytePos=8;
      currentByte = pgm_read_byte(bits++);
    }
    else {
      currentByte = currentByte << 1;
    }
  }
}

void lcd_ili9341_drawBitFontString(uint8_t x, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg){
  uint8_t character=*(string++);
  uint8_t bgHigh = bg>>8;
  uint8_t bgLow = bg & 0xff;
  uint8_t fgHigh = fg>>8;
  uint8_t fgLow = fg & 0xff;
  while (character!=0){
    lcd_ili9341_drawBitFontChar(x,y, character, font, fgHigh, fgLow, bgHigh, bgLow);
    x+=font->width;
    character=*(string++);
  }
}

void lcd_ili9341_drawBitFontCenteredString(uint8_t x, uint16_t width, uint8_t y, const char *string, const bitfont *font, uint16_t fg, uint16_t bg){
  const char* buffer = string;
  uint8_t fontWidth = font->width;
  while (*buffer!=0){
    width -= fontWidth;
    buffer++;
  }
  lcd_ili9341_drawBitFontString(x+width/2, y, string, font, fg, bg);
}

void lcd_ili9341_drawImage565(uint16_t x, uint16_t y, const image565 *image){
  lcd_ili9341_setXAddress(x, x+image->width-1);
  lcd_ili9341_setYAddress(y, y+image->height-1);

  const uint8_t *bytes = image->bytes;

  lcd_ili9341_sendCommandAsyncBegin(ILI9341_CMD_MEMORY_WRITE);
  for (uint16_t y=image->height; y!=0; y--){
    for (uint16_t x=image->width; x!=0; x--){
      spi_async_io(pgm_read_byte(bytes++));
      spi_async_io(pgm_read_byte(bytes++));
    }
  }
  spi_async_io_end();
}

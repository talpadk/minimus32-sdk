#include "lcd_ili9341.h"
#include "ili9341_defines.h"


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

uint8_t lcd_ili9341_obtainBus(uint8_t blocking){
  if (spi_obtain_bus(blocking)){
    //4 MHz @16MHz sys clock
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
  lcd_ili9341_sendCommand(0xCB);
  spi_io(0x39);
  spi_io(0x2C);
  spi_io(0x00);
  spi_io(0x34);
  spi_io(0x02);
  
  lcd_ili9341_sendCommand(0xCF);
  spi_io(0x00);
  spi_io(0XC1);
  spi_io(0X30);
  
  lcd_ili9341_sendCommand(0xE8);
  spi_io(0x85);
  spi_io(0x00);
  spi_io(0x78);
  
  lcd_ili9341_sendCommand(0xEA);
  spi_io(0x00);
  spi_io(0x00);
  
  lcd_ili9341_sendCommand(0xED);
  spi_io(0x64);
  spi_io(0x03);
  spi_io(0X12);
  spi_io(0X81);
  
  lcd_ili9341_sendCommand(0xF7);
  spi_io(0x20);
  
  lcd_ili9341_sendCommand(0xC0); //Power control
  spi_io(0x23); //VRH[5:0]
  
  lcd_ili9341_sendCommand(0xC1); //Power control
  spi_io(0x10); //SAP[2:0];BT[3:0]
  
  lcd_ili9341_sendCommand(0xC5); //VCM control
  spi_io(0x3e); //Contrast
  spi_io(0x28);
  
  lcd_ili9341_sendCommand(0xC7); //VCM control2
  spi_io(0x86); //--
  
  lcd_ili9341_sendCommand(0x36); // Memory Access Control
  spi_io(0x48); //C8 //48 68绔栧睆//28 E8 妯睆
  
  lcd_ili9341_sendCommand(0x3A);
  spi_io(0x55);
  
  lcd_ili9341_sendCommand(0xB1);
  spi_io(0x00);
  spi_io(0x18);
  
  lcd_ili9341_sendCommand(0xB6); // Display Function Control
  spi_io(0x08);
  spi_io(0x82);
  spi_io(0x27);
  
  lcd_ili9341_sendCommand(0xF2); // 3Gamma Function Disable
  spi_io(0x00);
  
  lcd_ili9341_sendCommand(0x26); //Gamma curve selected
  spi_io(0x01);
  
  lcd_ili9341_sendCommand(0xE0); //Set Gamma
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
  
  lcd_ili9341_sendCommand(0XE1); //Set Gamma
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
  
  lcd_ili9341_sendCommand(0x11); //Exit Sleep
  //delay(120);
  
  lcd_ili9341_sendCommand(0x29); //Display on
  lcd_ili9341_sendCommand(0x2c);
}

void lcd_ili9341_setColumnAddress(uint16_t start, uint16_t end){
  lcd_ili9341_sendCommand(ILI9341_CMD_COLUMN_ADDRESS_SET);
  spi_io(start >> 8);
  spi_io(start & 0xff);
  spi_io(end   >> 8);
  spi_io(end   & 0xff);
}

void lcd_ili9341_setRowAddress(uint16_t start, uint16_t end){
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

void lcd_ili9341_fillNTimesM(uint16_t colour, uint16_t countN, uint8_t countM){
  uint8_t colourHigh = colour >> 8;
  uint8_t colourLow  = colour & 0xff;
  uint8_t i;

  lcd_ili9341_sendCommand(ILI9341_CMD_MEMORY_WRITE);
  for (; countN!=0; countN--){
    for (i=countM; i!=0; i--){
      spi_io(colourHigh);
      spi_io(colourLow);
    }
  }
}

void lcd_ili9341_fill(uint16_t colour){
  lcd_ili9341_setColumnAddress(0,239);
  lcd_ili9341_setRowAddress(0,339);

  lcd_ili9341_fillNTimesM(colour, 320, 240);
}

void lcd_ili9341_drawColumnLine(uint16_t colour, uint8_t column, uint16_t startRow, uint16_t endRow){
  lcd_ili9341_setColumnAddress(column, column);
  lcd_ili9341_setRowAddress(startRow, endRow);
  lcd_ili9341_fillN(colour, endRow-startRow+1);
}


void lcd_ili9341_drawRowLine(uint16_t colour, uint16_t row, uint8_t startColumn, uint8_t endColumn){
  lcd_ili9341_setColumnAddress(startColumn, endColumn);
  lcd_ili9341_setRowAddress(row, row);
  lcd_ili9341_fillN(colour, endColumn-startColumn+1);
}
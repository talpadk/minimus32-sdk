//#exe

//Lazy linking is too agressive, remove som problematic .o files
//#dontlink AudioClassDevice
//#dontlink HIDClassDevice
//#dontlink MassStorageClassDevice
//#dontlink HIDParser


/*
PB0=chip sel.
PB1-3 = SPI
PB4= D/\C
PB5= reset
 */

#include "lcd_ili9341.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>


#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "LUFAConfig_test_only.h"
#include "vt100_codes.h"
#include "usb_serial_helpers.h"
#include "bitfont_6x8.h"

static FILE USBSerialStream_;

void setDataCommandPin(unsigned char value){
  value &= 0b00010000;
  PORTB = (PORTB & 0b11101111)|value;
}

void setChipSelectPin(unsigned char value){
  value &= 0b00000001;
  PORTB = (PORTB & 0b11111110)|value;
}


lcd_ili9341_device display = {
  &setChipSelectPin,
  &setDataCommandPin
};

void blink(void *data){
    led_red_toggle();
}

void printUSBGreeting(){
  fputs(VT100_CLEAR_SCREEN, &USBSerialStream_);
  fputs("ILI9341 QVGA test\r\n\r\n", &USBSerialStream_);
}

#define BAR_WIDTH (6)
#define BAR_MARGIN (2)
#define BAR_BEGIN ((240-64*3)/2)
#define FONT_WIDTH (6)
#define FONT_HEIGHT (8)

#define BAR_COLOUR ((31<<11)+(25<<5))

#define BAR_AREA_RESERVED ((FONT_HEIGHT+BAR_MARGIN)*2+(BAR_MARGIN+BAR_WIDTH)*4+BAR_MARGIN+1)

void drawBaseScreen(void){
  int i;
  uint8_t startRow=BAR_MARGIN+(FONT_HEIGHT+BAR_MARGIN);

  lcd_ili9341_drawFilledRectangle(BAR_COLOUR, 0, 239, 0, BAR_AREA_RESERVED-2);
  lcd_ili9341_drawRowLine(ILI9341_COLOUR_BLACK, BAR_AREA_RESERVED-1, 0, 239);

  lcd_ili9341_bitFontDrawString((240-20*FONT_WIDTH)/2,BAR_MARGIN, "ILI9341 Display test", &bitfont_6x8, ILI9341_COLOUR_WHITE, BAR_COLOUR);


  for (i=0; i<32; i++){
    lcd_ili9341_drawFilledRectangle(i<<11, BAR_BEGIN+i*6, BAR_BEGIN+i*6+5, startRow,startRow+BAR_WIDTH-1);
  }
  startRow += BAR_WIDTH+BAR_MARGIN;

  for (i=0; i<64; i++){
    lcd_ili9341_drawFilledRectangle(i<<5, BAR_BEGIN+i*3, BAR_BEGIN+i*3+2, startRow,startRow+BAR_WIDTH-1);
  }
  startRow += BAR_WIDTH+BAR_MARGIN;

  for (i=0; i<32; i++){
    lcd_ili9341_drawFilledRectangle(i, BAR_BEGIN+i*6, BAR_BEGIN+i*6+5, startRow,startRow+BAR_WIDTH-1);
  }
  startRow += BAR_WIDTH+BAR_MARGIN;

  for (i=0; i<64; i++){
    lcd_ili9341_drawFilledRectangle(((i>>1)<<11)+(i<<5)+(i>>1), BAR_BEGIN+i*3, BAR_BEGIN+i*3+2, startRow,startRow+BAR_WIDTH-1);
  }

  lcd_ili9341_drawFilledRectangle(ILI9341_COLOUR_WHITE, 0, 239, BAR_AREA_RESERVED, 319);

}

void drawRandomBox(void){
  uint16_t colour = rand()+rand();
  uint8_t width=(rand()%240)+1;
  uint16_t height=(rand()%(320-BAR_AREA_RESERVED))+1;
  uint16_t row;
  uint8_t col;

  if (width==240){
    col=0;
  }
  else {
    col = rand() % (240-width);
  }

  if (height==(320-BAR_AREA_RESERVED)){
    row=0;
  }
  else {
    row = rand() % ((320-BAR_AREA_RESERVED)-height);
  }
  row += BAR_AREA_RESERVED;

  lcd_ili9341_drawFilledRectangle(colour, col, col+width-1, row, row+height-1);

}

int main(void){
  uint8_t manufacturer=0;
  uint8_t driverVersion=0;
  uint8_t driver=0;
  timer1_callback blink_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  spi_config_io_for_master_mode();
  DDRB |= 0b00110000; //PB4-5=out
  

  /*USB_Init();
    CDC_Device_CreateStream(&VirtualSerial_CDC_Interface_, &USBSerialStream_);*/

  timer1_clock_init();
  
  PORTB &= ~0b00100000; //reset
  _delay_ms(10);
  PORTB |= 0b00100000;
  _delay_ms(100);

  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  printUSBGreeting();


  lcd_ili9341_obtainBus(1);

  lcd_ili9341_selectDevice(&display);
  lcd_ili9341_init();

  drawBaseScreen();

  while (1){
    drawRandomBox();
  }


  lcd_ili9341_deselectDevice(&display);
  lcd_ili9341_releaseBus();
 
  while(1){
    /*lcd_ili9341_readDisplayId(&display, &manufacturer, &driverVersion, &driver);
    fputs("Manufacture ID:    0x", &USBSerialStream_);
    usb_serial_printHexByte(manufacturer, &USBSerialStream_);
    fputs("\r\n", &USBSerialStream_);
    fputs("Driver Version ID: 0x", &USBSerialStream_);
    usb_serial_printHexByte(driverVersion, &USBSerialStream_);
    fputs("\r\n", &USBSerialStream_);
    fputs("Driver ID:         0x", &USBSerialStream_);
    usb_serial_printHexByte(driver, &USBSerialStream_);
    fputs("\r\n", &USBSerialStream_);*/
    /*
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface_);

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface_);
    USB_USBTask();
    _delay_ms(2000);*/
  }



  return 0;
}

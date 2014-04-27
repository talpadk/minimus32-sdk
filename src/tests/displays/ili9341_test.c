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
#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "LUFAConfig_test_only.h"
#include "vt100_codes.h"
#include "usb_serial_helpers.h"

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
  lcd_ili9341_init(&display);
  lcd_ili9341_fill(&display, ILI9341_COLOUR_RED); 
  lcd_ili9341_fill(&display, ILI9341_COLOUR_GREEN); 
  lcd_ili9341_fill(&display, ILI9341_COLOUR_BLUE); 

  for (int i=7; i<240; i+=7){
    lcd_ili9341_drawColumnLine(&display, ILI9341_COLOUR_BLACK, i, 0, 339);
  }

  lcd_ili9341_drawRowLine(&display, ILI9341_COLOUR_BLACK, 7, 0, 14);


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

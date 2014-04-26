//#exe

//Lazy linking is too agressive, remove som problematic .o files
//#dontlink AudioClassDevice
//#dontlink HIDClassDevice
//#dontlink MassStorageClassDevice
//#dontlink HIDParser

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

void blink(void *data){
    led_red_toggle();
}

void printUSBGreeting(){
  fputs(VT100_CLEAR_SCREEN, &USBSerialStream_);
  fputs("ILI9341 QVGA test\r\n\r\n", &USBSerialStream_);

  usb_serial_printHexByte(255, &USBSerialStream_);
  usb_serial_printHexByte(0, &USBSerialStream_);
  usb_serial_printHexByte(128, &USBSerialStream_);
}


int main(void){
  int16_t serialByte;
  timer1_callback blink_call_back;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();

  USB_Init();
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface_, &USBSerialStream_);

  timer1_clock_init();
  _delay_ms(2000);


  timer1_clock_register_callback(0, 100, 1, &blink, 0, &blink_call_back);

  printUSBGreeting();


  lcd_ili9341_obtain_bus(1);

  lcd_ili9341_release_bus();

  while(1){
    serialByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface_);

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface_);
    USB_USBTask();

  }
  return 0;
}

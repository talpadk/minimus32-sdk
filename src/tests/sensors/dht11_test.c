//#exe

//Lazy linking is too agressive, remove som problematic .o files
//#dontlink AudioClassDevice
//#dontlink HIDClassDevice
//#dontlink MassStorageClassDevice
//#dontlink HIDParser


#include "LUFAConfig_test_only.h"


#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "vt100_codes.h"

#include "io_function.h"

#include "dht11.h"

static FILE USBSerialStream_;

void blue(unsigned char data){
  led_blue(data);
}

io_outFunction setLED_=&blue;

unsigned char led_=0;


void connectedBlink(void *data){
  setLED_(led_);
  if (led_) led_=0;
  else led_=255;

  if (connected_){
    led_red_toggle();
  }
  else {
    led_red(0);
  }
}

void showData(void *data){
  fputs(VT100_CLEAR_SCREEN, &USBSerialStream_);
  fputs("DHT11 Sensor Test\r\n\r\n", &USBSerialStream_);
}

int main(void) {
  timer1_callback blinkCallBack;
  timer1_callback showDataCallBack;
  
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  USB_Init();
  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface_, &USBSerialStream_);
  sei();

  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &connectedBlink, 0, &blinkCallBack);
  timer1_clock_register_callback(2, 0, 1, &showData, 0, &showDataCallBack);

  while(1){
    //Remove junk from the port
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface_);

    //Animate the USB interface
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface_);
    USB_USBTask();
  }

}


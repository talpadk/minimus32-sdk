//#exe

//Lazy linking is too agressive, remove som problematic .o files
//#dontlink AudioClassDevice
//#dontlink HIDClassDevice
//#dontlink MassStorageClassDevice
//#dontlink HIDParser


///A hello world using the LUFA based virtual serial port
/** 
 * @file
 * @author Visti Andresen
 * @ingroup tests
 *
 * Hardware:
 *
 * Just a plain Minimus32 in a USB port
 *
 * Connect using 'minicom -oD /dev/ttyACM0' (you may have to change the port)
 * 
 * Use "dmeg" to determin the port.
 *
 * Red LED flashes when the USB port is connected
 *
 * Blue LED intensity can be controlled over the serial connection.
 */

#include "LUFAConfig_test_only.h"


#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"
#include "sw_dither.h"
#include "vt100_codes.h"

#include <avr/pgmspace.h>


/** Standard file stream for the CDC interface when set up, so that the virtual CDC COM port can be
 *  used like any regular character stream in the C APIs
 */
static FILE USBSerialStream_;

sw_dither dither_;

#define BLUE_TICKS (250)

void connectedBlink(void *data){
  if (connected_){
    led_red_toggle();
  }
  else {
    led_red(0);
  }
}

//LED control
ISR(TIMER1_COMPB_vect){
  timer1_sub_timer_b_delay(BLUE_TICKS);
  led_blue(sw_dither_animate(&dither_));
}

void helloWorld(void *data){
  fputs(VT100_CLEAR_SCREEN, &USBSerialStream_);
  fputs("Hello World\r\n\r\n", &USBSerialStream_);
  fputs("Change the blue LED intensity by pressing 0-9\r\n\r\n", &USBSerialStream_);
  fputs("Current intensity: ", &USBSerialStream_);
  CDC_Device_SendByte(&VirtualSerial_CDC_Interface_, dither_.power+'0');
}

int main(void) {
  int16_t serialByte;
  timer1_callback blink_call_back;
  timer1_callback hello_call_back;
  
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  USB_Init();
  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface_, &USBSerialStream_);
  sei();

  sw_dither_init(&dither_, 9);
  sw_dither_set(&dither_, 5);

  timer1_clock_init();
  timer1_clock_register_callback(0, 100, 1, &connectedBlink, 0, &blink_call_back);
  timer1_clock_register_callback(1, 0, 1, &helloWorld, 0, &hello_call_back);
  timer1_sub_timer_b_delay(BLUE_TICKS);

  while(1){
    serialByte = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface_);
    if (serialByte>='0' && serialByte<='9'){
      sw_dither_set(&dither_, serialByte-'0');
    }

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface_);
    USB_USBTask();
  }

}



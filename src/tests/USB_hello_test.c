//#exe
#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>

//USB stuff
#include "USB_hello_test_Descriptors.h"
#include "USB.h"
#include "CDC.h"

#include "minimus32.h"
#include "watchdog.h"
#include "sys_clock.h"
#include "timer1_clock.h"

#include "minimus32.h"

const char *HELLO = "Hello world\n";
const char *CMD_UNKNOWN = "Command unknown!!!\n";


void timer_demo_a(void *data){
  led_red_toggle();
}

void timer_demo_b(void *data){
  led_blue_toggle();
}


USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
  {
    .Config =
    {
      .ControlInterfaceNumber         = 0,
      
      .DataINEndpointNumber           = CDC_TX_EPNUM,
      .DataINEndpointSize             = CDC_TXRX_EPSIZE,
      .DataINEndpointDoubleBank       = false,
      
      .DataOUTEndpointNumber          = CDC_RX_EPNUM,
      .DataOUTEndpointSize            = CDC_TXRX_EPSIZE,
      .DataOUTEndpointDoubleBank      = false,
      
      .NotificationEndpointNumber     = CDC_NOTIFICATION_EPNUM,
      .NotificationEndpointSize       = CDC_NOTIFICATION_EPSIZE,
      .NotificationEndpointDoubleBank = false,
    },
  };

void EVENT_USB_Device_Connect(void);

void EVENT_USB_Device_Connect(void)
{
}

void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}


/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
}


int main(void) {
  timer1_callback demo_callback_a;
  timer1_callback demo_callback_b;
  

  const char * data =0;
  int16_t tmp=0;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  sei(); //global int enable
  timer1_clock_init();
  
  timer1_clock_register_callback(1, 0, 0, &timer_demo_a, 0, &demo_callback_a);
  timer1_clock_register_callback(0, 50, 1, &timer_demo_b, 0, &demo_callback_b);

  USB_Init();

  while (1){
    /*    i++;
    if (i>60000){
      i=0;
      CDC_Device_SendByte(&VirtualSerial_CDC_Interface, 'R');
    }
    */
    if (data!=0){
      CDC_Device_SendByte(&VirtualSerial_CDC_Interface, *data);
      data++;
      if (*data==0) data=0;	     
    }
    if (data==0 && CDC_Device_BytesReceived(&VirtualSerial_CDC_Interface)!=0){
      tmp = CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
      switch (tmp){
      default:
	data=CMD_UNKNOWN;
	break;
      case '\n':
	data=HELLO;
	  break;
      }
    }

    CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
    USB_USBTask();

  }
  return 0;
}

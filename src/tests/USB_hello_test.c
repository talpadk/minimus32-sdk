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
int clock;

const char *HELLO = "Hello world\n";
const char *CMD_UNKNOWN = "Command unknown!!!\n";


ISR(TIMER1_COMPA_vect)
{
  //on/off period of 1 sec @ 16Mhz
  clock++;
  if (clock>1000){
    clock=0;
    led_blue_toggle();
  }
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
  led_red(1);
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
  unsigned int i;

  const char * data =0;
  int16_t tmp=0;

  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  


  OCR1AH = 62; //set OCR1A to 16000
  OCR1AL = 128; 
  
  TCCR1A = 0; //prescaler = 1 & count up to OCR1A function 
  TCCR1B = 1<<WGM12|1<<CS10;

  TIMSK1 = 1<<ICIE1|1<<OCIE1A; //enable timer1 int and compare a int

  sei(); //global int enable

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

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

void sensorSetOutput(unsigned char data){
  DDRD=(DDRD&0xfe)|(data&1);
}

void sensorOutFunction(unsigned char data){
  PORTD=(PORTD&0xfe)|(data&1);
}

dht11 sensor_;
uint8_t timeouts_=0;
uint8_t checksumErrors_=0;
uint8_t temp_, moist_;
uint8_t first_run_=1;

void intOut(unsigned char data){
  fputc((data/100)+'0', &USBSerialStream_);
  fputc(((data/10)%10)+'0', &USBSerialStream_);
  fputc((data%10)+'0', &USBSerialStream_);
}

void showData(void *data){
  led_red_toggle();
  dht11_state state= dht11_getConversionState(&sensor_);
 
  fputs(VT100_CLEAR_SCREEN, &USBSerialStream_);
  fputs("===== DHT11 Sensor Test =====\r\n\r\n", &USBSerialStream_);
  fputs("DHT11 State: ", &USBSerialStream_);
  switch (state){
  case DHT11_STATE_INIT:
    fputs("Init\r\n", &USBSerialStream_);
    break;
  case DHT11_IDLE:
    fputs("Idle\r\n", &USBSerialStream_);
    break;
  case DHT11_SENDING_START:
    fputs("Sending Start\r\n", &USBSerialStream_);
    break;
  case DHT11_WAITING_FOR_ACK:
    fputs("Waiting for sensor ack\r\n", &USBSerialStream_);
    break;
  case DHT11_GOT_ACK_WAIT_IDLE:
    fputs("Sensor Ack, waiting for bus idle\r\n", &USBSerialStream_);
    break;
  case DHT11_WAITING_FOR_BIT_SYNC:
    fputs("Waiting for bit sync\r\n", &USBSerialStream_);
    break;
  case DHT11_WAITING_FOR_BIT_END:
    fputs("Waiting for end of bit\r\n", &USBSerialStream_);
    break;
  }
  fputs("Temp  " , &USBSerialStream_);
  intOut(temp_);
  fputs(" C\r\n", &USBSerialStream_);

  fputs("Moist ", &USBSerialStream_);
  intOut(moist_);
  fputs(" %\r\n", &USBSerialStream_);

  fputs("Timeouts ", &USBSerialStream_);
  intOut(timeouts_);
  fputs("\r\n", &USBSerialStream_);

  fputs("Checksum errors ", &USBSerialStream_);
  intOut(checksumErrors_);
  fputs("\r\n", &USBSerialStream_);

  //Notice this is illegal and may give broken data, for debug only
  fputs("\r\n\r\n", &USBSerialStream_);
  fputs("=== Debugging only, accesing private data. ===\r\n", &USBSerialStream_);
  fputs("===        Don't try this at home!         ===\r\n", &USBSerialStream_);
  fputs("RAW ", &USBSerialStream_);
  intOut(sensor_.data[0]);
  fputc(',', &USBSerialStream_);
  intOut(sensor_.data[1]);
  fputc(',', &USBSerialStream_);
  intOut(sensor_.data[2]);
  fputc(',', &USBSerialStream_);
  intOut(sensor_.data[3]);
  fputc(',', &USBSerialStream_);
  intOut(sensor_.data[4]);

  fputs("\r\nPos Byte: ", &USBSerialStream_);
  intOut(sensor_.rxByte);
  fputs(" bit: ", &USBSerialStream_);
  intOut(sensor_.rxBit-1);
}

void readSensor(void *data){
  if (!first_run_){
    //Data conversion needs to have been started
    if (dht11_isDataReady(&sensor_)){
      if (dht11_isChecksumOk(&sensor_)){
	temp_=sensor_.data[2];
	moist_=sensor_.data[0];
      }
      else {
	checksumErrors_++;
      }
    }
    else {
      timeouts_++;
    }
  }
  first_run_=0;

  dht11_startConversion(&sensor_);
}

ISR(INT0_vect){
  dht11_irqAnimate(&sensor_);
}

int main(void) {
  timer1_callback showDataCallBack;
  timer1_callback readSensorCallBack;
  
  watchdog_disable();
  minimus32_init();
  clock_prescale_none();
  
  USB_Init();
  /* Create a regular character stream for the interface so that it can be used with the stdio.h functions */
  CDC_Device_CreateStream(&VirtualSerial_CDC_Interface_, &USBSerialStream_);
  sei();

  timer1_clock_init();
  timer1_clock_register_callback(0, 250, 1, &showData, 0, &showDataCallBack);
  timer1_clock_register_callback(1, 0, 1, &readSensor, 0, &readSensorCallBack);

  dht11_init(&sensor_, &sensorSetOutput, &sensorOutFunction, 0);
  while(1){
    //Remove junk from the port
    CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface_);

    //Animate the USB interface
    CDC_Device_USBTask(&VirtualSerial_CDC_Interface_);
    USB_USBTask();
  }

}


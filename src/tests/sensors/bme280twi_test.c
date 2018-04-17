//#global_cflags -DMCU=atmega32u4
///#exe

#include <avr/io.h>
#include <avr/interrupt.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "promicro.h"

#include "async_serial_1.h"
#include "vt100_codes.h"
#include "int_print.h"
#include "timer1_clock.h"

#include "bme280twi.h"



#define PRINT_BUFFER_LENGTH (10)


int main(void) {
  uint8_t returnValue=0;
  uint32_t i;
  char buffer[PRINT_BUFFER_LENGTH];

  timer1_callback readSensorCallBack;
  
  watchdog_disable();
  promicro_init();
  clock_prescale_none();
  sei();
  
  async_serial_1_init(SERIAL_SPEED_9600);

  timer1_clock_init();
  //  timer1_clock_register_callback(10, 000, 1, &readSensor, 0, &readSensorCallBack);

  

  async_serial_1_write_string(VT100_CLEAR_SCREEN);
  async_serial_1_write_string("BME280 test boot done\r\n");


  async_serial_1_write_string("About to detect the sensor on the bus\r\n");

  bme280twi_obtainBus(1);
  while (!returnValue){
    returnValue = bme280twi_testId(BME280TWI_ADDRESS_LOW);
  }
  //uint8Print(returnValue, 3, buffer);
  //async_serial_1_write_string(buffer);
  async_serial_1_write_string("BME280 detected on the TWI bus\r\n");
  while(1){
  }
}

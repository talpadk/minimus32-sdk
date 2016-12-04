//#global_cflags -DMCU=atmega32u4
//#exe

///A simple "hello world" that flashes the two on board LEDs 

/**
 * @file
 * @author Visti Andresen
 * @ingroup tests
 * @ingroup board_promicro
 */


#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "watchdog.h"
#include "sys_clock.h"
#include "promicro.h"

#include "nrf24l01p.h"

#include "async_serial_1.h"
#include "vt100_codes.h"
#include "int_print.h"

int clock;

void setCE(unsigned char value){
  PORTF = (PORTF & (~(1<<5))) | (value & (1<<5));
}

void setCSN(unsigned char value){
  PORTF = (PORTF & (~(1<<6))) | (value & (1<<6));
}

Nrf24l01p nrfDevice_ = {
  .pinCSN = &setCSN,
  .pinCE  = &setCE  
};

#define PRINT_BUFFER_LENGTH (10)

int main(void) {
  uint32_t i;
  char buffer[PRINT_BUFFER_LENGTH];

  uint8_t message[1] = {27};
  uint8_t txAddress[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
  uint8_t statusByte;

  
  watchdog_disable();
  promicro_init();
  clock_prescale_none();

  async_serial_1_init(SERIAL_SPEED_9600);
  ///nRF selects as outputs
  DDRF |= (1<<5)|(1<<6);

  DDRB &= ~0b00001000; //Set PB3 (MISO) as input
  DDRB |=  0b00000110; //Set PB1 (SCLK) and PB2 (MOSI) as output



  async_serial_1_write_string(VT100_CLEAR_SCREEN);
  async_serial_1_write_string("TX Boot done, init of nRF next\r\n");

  
  nrf24l01p_obtainBus(1);
  
  

  nrf24l01p_initDevice(&nrfDevice_);
  
  for (i=0; i<1000; i++){
    //Verify the SPI link to the nRF24L01P by reading some registers and comparing them with the power on values.
    //If we get an unexpected result we halt the "boot" process
    if (nrf24l01p_readRegister(&nrfDevice_, NRF24L01P_REG_SETUP_RETR)!=0b11){  while(1){}    }
    if (nrf24l01p_readRegister(&nrfDevice_, NRF24L01P_REG_EN_AA)!=0b111111) {  while(1){}    }
  }

  async_serial_1_write_string("nRF comm init done\r\n");
  led_yellow(0);
  led_green(1);

  //Set TX address
  nrf24l01p_writeRegisterBytes(&nrfDevice_, NRF24L01P_REG_TX_ADDR, txAddress, 5);
  nrf24l01p_writeRegisterBytes(&nrfDevice_, NRF24L01P_REG_RX_ADDR_P0, txAddress, 5);
 
  //Enable CRC, power up, TX mode
  nrf24l01p_writeRegister(&nrfDevice_, NRF24L01P_REG_CONFIG, 0b1010);
  setCE(255);
  //Flush the TX FIFO
  nrf24l01p_flushTX(&nrfDevice_);
  //Clear any ack timeouts
  nrf24l01p_writeRegister(&nrfDevice_, NRF24L01P_REG_STATUS, 1<<4);
  while (1){
    led_yellow_toggle();
    led_green_toggle();
    for (i=0; i<600000; i++){ }
    //clear any status bits
    //nrf24l01p_writeRegister(&nrfDevice_, NRF24L01P_REG_STATUS, 0xff);
    statusByte = nrf24l01p_writePackageWithAck(&nrfDevice_, message, 1);

    async_serial_1_write_string("Status reg: ");
    uint8Print(statusByte, 3, buffer);
    async_serial_1_write_string(buffer);
    async_serial_1_write_string("\r\n");

    if (statusByte & 1<<4){
      async_serial_1_write_string("Max retries\r\n");
      nrf24l01p_writeRegister(&nrfDevice_, NRF24L01P_REG_STATUS, 1<<4);
    }
    if (statusByte & 1<<0){
      async_serial_1_write_string("TX FIFO full\r\n");
    }
    
  }

  nrf24l01p_releasebus();
  
  return 0;
}

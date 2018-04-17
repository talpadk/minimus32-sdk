#include "bme280twi.h"

#include "twi.h"

#define REG_ID (0xD0)

#define REG_PRESS_MSB  (0xF7)
#define REG_PRESS_LSB  (0xF8)
#define REG_PRESS_XLSB (0xF9)
#define REG_TEMP_MSB   (0xFA)
#define REG_TEMP_LSB   (0xFB)
#define REG_TEMP_XLSB  (0xFC)
#define REG_HUM_MSB    (0xFD)
#define REG_HUM_LSB    (0xFE)

uint8_t bme280twi_obtainBus(uint8_t blocking){
  uint8_t result;
  result = twi_obtain_bus(blocking);
  if (!result) { return result; }

  //16MHz system clock => 400kHz TWI
  twi_initBus(TWI_PRESCALER_1, 12);
  return 1;
}

void bme280twi_releaseBus(void){
  twi_release_bus();
}

TWI_ERROR bme280twi_readRegisters(uint8_t address, uint8_t startRegister, uint8_t *buffer, uint8_t count){
  TWI_ERROR error;
  error = twi_blocking_master_opperation(address|TWI_ADDRESS_WRITE, 1, &startRegister, TWI_SEND_START_COND);
  if (error != TWI_ERROR_NO_ERROR) { return error; }
  twi_blocking_master_opperation(address|TWI_ADDRESS_READ, count, buffer, TWI_SEND_START_COND|TWI_SEND_STOP_COND);
  if (error != TWI_ERROR_NO_ERROR) { return error; }
  return TWI_ERROR_NO_ERROR;
}

TWI_ERROR bme280twi_writeRegisters(uint8_t address, uint8_t startRegister, uint8_t *buffer, uint8_t count){
  TWI_ERROR error;
  uint8_t packet[2];
  uint8_t flags=TWI_SEND_START_COND;

  if (count==0) { return TWI_ERROR_NO_ERROR; }

  while (count){
    packet[0]=startRegister;
    packet[1]=*buffer;
    if (count == 1){ flags |= TWI_SEND_STOP_COND; }

    error = twi_blocking_master_opperation(address|TWI_ADDRESS_WRITE, 2, packet, flags);
    if (error != TWI_ERROR_NO_ERROR) { return error; }
    
    flags=TWI_DONT_SEND_START_COND;
    count--;
    startRegister++;
    buffer++;
  }

  return TWI_ERROR_NO_ERROR;
}


uint8_t bme280twi_testId(uint8_t address){
  uint8_t buffer;
  TWI_ERROR error;

  error = bme280twi_readRegisters(address, REG_ID, &buffer, 1);
  if (error != TWI_ERROR_NO_ERROR) { return 0; }

  return buffer == 0x60;
}

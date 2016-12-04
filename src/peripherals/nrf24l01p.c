#include "nrf24l01p.h"
#include "spi.h"

#define NRF24L01P_CMD_R_REGISTER          (0x00)
#define NRF24L01P_CMD_W_REGISTER          (0x20)
#define NRF24L01P_CMD_R_RX_PAYLOAD        (0b01100001)
#define NRF24L01P_CMD_W_TX_PAYLOAD        (0b10100000)
#define NRF24L01P_CMD_FLUSH_TX            (0b11100001)
#define NRF24L01P_CMD_FLUSH_RX            (0b11100010)
#define NRF24L01P_CMD_REUSE_TX_PL         (0b11100011)
#define NRF24L01P_CMD_R_RX_PL_WID         (0b01100000)
#define NRF24L01P_CMD_W_ACK_PAYLOAD       (0b10101000)
#define NRF24L01P_CMD_W_TX_PAYLOAD_NO_ACK (0b10110000)
#define NRF24L01P_CMD_NOP                 (0xFF)

#define NRF24L01P_REG_RF_CH          (0x05)





uint8_t nrf24l01p_obtainBus(uint8_t blocking){
  if (spi_obtain_bus(blocking)){
    //With 4k7 in series 16Mhz/8 seems to work, 16/4 does not (verified on two AMTEGA32U4 chips with 100k reads of two registers)
    //8 MHz @16MHz sys clock (10Mhz is max)
    spi_setup(SPI_DIVIDER_8, SPI_FLAG_BUS_MASTER|SPI_FLAG_CLOCK_RISING|SPI_FLAG_ENABLE_SPI|SPI_FLAG_MSB_FIRST);
    return 1;
  }
  else {
    return 0;
  }
}

void nrf24l01p_releasebus(void){
  spi_release_bus();
}

void nrf24l01p_initDevice(Nrf24l01p *device){
  device->pinCSN(io_PIN_HIGH);
  device->pinCE(io_PIN_LOW);
}

/** 
 * Updates a single byte register in the nRF24L01+
 * 
 * @param device the device to talk to
 * @param reg the register to update
 * @param value the new value for the register
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_writeRegister(Nrf24l01p *device, uint8_t reg, uint8_t value){
  uint8_t status;
  device->pinCSN(io_PIN_LOW);
  status = spi_io(reg|NRF24L01P_CMD_W_REGISTER);
  spi_io(value);
  device->pinCSN(io_PIN_HIGH);
  return status;
}

uint8_t nrf24l01p_writeRegisterBytes(Nrf24l01p *device, uint8_t reg, uint8_t *value, uint8_t length){
  uint8_t result;
  device->pinCSN(io_PIN_LOW);
  result = spi_io(reg|NRF24L01P_CMD_W_REGISTER);
  while (length!=0){
    spi_io(*value);
    value++;
    length--;
  }
  device->pinCSN(io_PIN_HIGH);
  return result;
}

uint8_t nrf24l01p_readRegister(Nrf24l01p *device, uint8_t address){
  uint8_t result;
  device->pinCSN(io_PIN_LOW);
  spi_io(address);
  result = spi_io(0);
  device->pinCSN(io_PIN_HIGH);
  return result;
}



uint8_t nrf24l01p_getStatusRegister(Nrf24l01p *device){
  uint8_t status;
  device->pinCSN(io_PIN_LOW);
  status = spi_io(NRF24L01P_CMD_NOP);
  device->pinCSN(io_PIN_HIGH);
  return status;
}

void nrf24l01p_enableRadio(Nrf24l01p *device){
  device->pinCE(io_PIN_HIGH);
}

void nrf24l01p_disableRadio(Nrf24l01p *device){
    device->pinCE(io_PIN_LOW);
}

void nrf24l01p_setRadioChannel(Nrf24l01p *device, uint8_t channel){
  nrf24l01p_writeRegister(device, NRF24L01P_REG_RF_CH, channel);
}

uint8_t nrf24l01p_writePackageWithAck(Nrf24l01p *device, uint8_t *data, uint8_t length){
  uint8_t result;
  device->pinCSN(io_PIN_LOW);
  result = spi_io(NRF24L01P_CMD_W_TX_PAYLOAD);
  while (length!=0){
    spi_io(*data);
    data++;
    length--;
  }
  device->pinCSN(io_PIN_HIGH);
  return result;
}

uint8_t nrf24l01p_flushTX(Nrf24l01p *device){
  uint8_t result;
  device->pinCSN(io_PIN_LOW);
  result = spi_io(NRF24L01P_CMD_FLUSH_TX);
  device->pinCSN(io_PIN_HIGH);
  return result;
}


uint8_t nrf24l01p_flushRX(Nrf24l01p *device){
  uint8_t result;
  device->pinCSN(io_PIN_LOW);
  result = spi_io(NRF24L01P_CMD_FLUSH_RX);
  device->pinCSN(io_PIN_HIGH);
  return result;
}

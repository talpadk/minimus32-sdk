#ifndef NRF24L01P
#define NRF24L01P

#include <stdint.h>
#include <io_function.h>

typedef struct {
  io_outFunction pinCSN;
  io_outFunction pinCE;
} Nrf24l01p;


uint8_t nrf24l01p_obtainBus(uint8_t blocking);
void nrf24l01p_releasebus(void);

void nrf24l01p_initDevice(Nrf24l01p *device);
  
void nrf24l01p_enableRadio(Nrf24l01p *device);

void nrf24l01p_disableRadio(Nrf24l01p *device);

uint8_t nrf24l01p_getStatusRegister(Nrf24l01p *device);

void nrf24l01p_setRadioChannel(Nrf24l01p *device, uint8_t channel);


#define NRF24L01P_REG_CONFIG      (0x00)
#define NRF24L01P_REG_EN_AA       (0x01)
#define NRF24L01P_REG_EN_RXADDR   (0x02)
#define NRF24L01P_REG_SETUP_AW    (0x03)
#define NRF24L01P_REG_SETUP_RETR  (0x04)
#define NRF24L01P_REG_RF_CH       (0x05)
#define NRF24L01P_REG_RF_SETUP    (0x06)
#define NRF24L01P_REG_STATUS      (0x07)
#define NRF24L01P_REG_OBSERVE_TX  (0x08)
#define NRF24L01P_REG_RPD         (0x09)
#define NRF24L01P_REG_RX_ADDR_P0  (0x0A)
#define NRF24L01P_REG_RX_ADDR_P1  (0x0B)
#define NRF24L01P_REG_RX_ADDR_P2  (0x0C)
#define NRF24L01P_REG_RX_ADDR_P3  (0x0D)
#define NRF24L01P_REG_RX_ADDR_P4  (0x0E)
#define NRF24L01P_REG_RX_ADDR_P5  (0x0F)
#define NRF24L01P_REG_TX_ADDR     (0x10)
#define NRF24L01P_REG_RX_PW_P0    (0x11)
#define NRF24L01P_REG_RX_PW_P1    (0x12)
#define NRF24L01P_REG_RX_PW_P2    (0x13)
#define NRF24L01P_REG_RX_PW_P3    (0x14)
#define NRF24L01P_REG_RX_PW_P4    (0x15)
#define NRF24L01P_REG_RX_PW_P5    (0x16)
#define NRF24L01P_REG_FIFO_STATUS (0x17)
#define NRF24L01P_REG_DYNPD       (0x1C)
#define NRF24L01P_REG_FEATURE     (0x1D)



/** 
 * Updates a single byte register in the nRF24L01+
 * 
 * @param device the device to talk to
 * @param reg the register to update
 * @param value the new value for the register
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_writeRegister(Nrf24l01p *device, uint8_t reg, uint8_t value);

/** 
 * Updates a N byte register in the nRF24L01+
 * 
 * @param device the device to talk to
 * @param reg the register to update
 * @param value the new value for the register
 * @param length the length of the value in bytes
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_writeRegisterBytes(Nrf24l01p *device, uint8_t reg, uint8_t *value, uint8_t length);




uint8_t nrf24l01p_readRegister(Nrf24l01p *device, uint8_t address);

/** 
 * Writes a package to the TX FIFO with auto ack
 * 
 * @param device the device to send with
 * @param data pointer to the package data to send
 * @param length the length of the data package
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_writePackageWithAck(Nrf24l01p *device, uint8_t *data, uint8_t length);

/** 
 * Flushes the TX FIFO
 * 
 * @param device the device to flush
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_flushTX(Nrf24l01p *device);

/** 
 * Flushes the RX FIFO
 * 
 * @param device the device to flush
 * 
 * @return the content of the STATUS register.
 */
uint8_t nrf24l01p_flushRX(Nrf24l01p *device);

//void nrf24l01p_configureRadio()

#endif //NRF24L01P

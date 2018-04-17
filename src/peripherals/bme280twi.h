#ifndef BME280TWI_H
#define BME280TWI_H

#include <stdint.h>

typedef struct {
  uint8_t humidity_lsb;
  uint8_t humidity_msb;
  uint8_t temperature_lsb;
  uint8_t temperature_msb;
  uint8_t pressure_lsb;
  uint8_t pressure_msb;
} Bme280Measurement;

///The TWI address if SDO is held low
#define BME280TWI_ADDRESS_LOW  (0x76<<1)
///The TWI address if SDO is held high
#define BME280TWI_ADDRESS_HIGH (0x77<<1)

uint8_t bme280twi_obtainBus(uint8_t blocking);
void bme280twi_releaseBus(void);



/** 
 * Returns true if the chip at the given address responded with the correct ID code.
 * 
 * 
 * @return true if chip was found
 */
uint8_t bme280twi_testId(uint8_t address);


#endif //BME280TWI_H

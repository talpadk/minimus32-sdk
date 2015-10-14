#ifndef TWI_H_
#define TWI_H_
#include <stdint.h>


#define TWI_SEND_START_COND      (0)
#define TWI_DONT_SEND_START_COND (1)
#define TWI_SEND_STOP_COND       (2)
#define TWI_DONT_SEND_STOP_COND  (0)

#define TWI_ADDRESS_READ  (1)
#define TWI_ADDRESS_WRITE (0)

typedef enum {
  TWI_PRESCALER_1=0,
  TWI_PRESCALER_4=1,
  TWI_PRESCALER_16=2,
  TWI_PRESCALER_64=3,
} TWI_PRESCALER;

typedef enum {
  TWI_ERROR_NO_ERROR=0,
  TWI_ERROR_START_COND_NOT_SEND,
  TWI_ERROR_MISSING_ADDRESS_ACK,
  TWI_ERROR_MISSING_DATA_ACK,
  
  //Lost arbitration probably caused by another master on the TWI bus
  TWI_ERROR_ARBITRATION_LOST,
} TWI_ERROR;

/** 
 * Obtains the right to use the TWI bus, the bus should be obtained prior using it on order
 * to be able to share the bus among multible devices.
 * 
 * @param blocking if true the call will not return before the bus could be obtained
 * 
 * @return returns true (1) if the bus has been obtained
 */
uint8_t twi_obtain_bus(uint8_t blocking);

/** 
 * Releases the bus after haveing been obtained
 * @see twi_obtain_bus
 * 
 */
void twi_release_bus(void);

/** 
 * Configures the TWI bus clock frequency and enables the TWI hardware.
 * The TWI will be = CPU_clock/(16+2*bitRate*clockPrescaler_factor)
 * 
 * @param clockPrescaler selects the clock prescaler 
 * @param bitRate the bit rate dividor 
 */
void twi_initBus(TWI_PRESCALER clockPrescaler, uint8_t bitRate);

void twi_deinitBus(void);

TWI_ERROR twi_blocking_master_opperation(uint8_t address, uint8_t bufferLength, uint8_t *buffer, uint8_t flags);

#endif /* TWI_H_ */

#ifndef MOD_DHT11_H
#define MOD_DHT11_H

#include "io_function.h"
#include "external_interrupt.h" 
#include "timer1_clock.h"


typedef enum {
  DHT11_STATE_INIT=0,
  DHT11_IDLE,
  DHT11_SENDING_START,
  DHT11_WAITING_FOR_ACK,
  DHT11_GOT_ACK_WAIT_IDLE,
  DHT11_WAITING_FOR_BIT_SYNC,
  DHT11_WAITING_FOR_BIT_END
} dht11_state;

typedef struct {
  dht11_state state;
  uint8_t rxByte;
  uint8_t rxBit;
  uint8_t data_ready;
  uint8_t data[5];
  uint16_t start_msecs;
  uint16_t start_ticks;

  uint8_t extIrq;
  io_setOutput setOutput;
  io_outFunction out;

  timer1_callback callback;
  
} dht11;

/** 
 * Initializes the dth11 sensor system
 * Can be used to reconfigure the IO functions between conversions
 * 
 * @param setOutput the io function pointer used to change the pin from input to output
 * @param out the io function pointer for setting the pin
 * @param extIrq the external interrupt associated with the IO pin
 */
void dht11_init(dht11 *this, io_setOutput setOutput, io_outFunction out, uint8_t extIrq);


void dht11_irqAnimate(dht11 *this);

/** 
 * Starts the conversion cycle.
 * Prior to calling this the timer1_clock MUST be initialized and running
 * 
 */

void dht11_startConversion(dht11 *this);

/** 
 * Returns the state of the conversion, mostly for debugging.
 * Consider using dth11_isDataReady
 *
 * @see dht11_isDataReady
 * 
 * @param this the DHT11 sensor this opperation works on
 * 
 * @return the conversion state.
 */
dht11_state dht11_getConversionState(dht11 *this);

/** 
 * After starting the conversion using dht11_startConversion
 * this function can be used to check if new data is awailable 
 * 
 * @param this the DHT11 sensor this opperation works on
 * 
 * @return 0 if the data read hasn't compleated yet, 1 otherwise.
 */
uint8_t dht11_isDataReady(dht11 *this);

/** 
 * Returns true is the data checksum is valid
 * 
 * @param this the DHT11 sensor this opperation works on
 * 
 * @return "true" if the checksum is valid
 */
uint8_t dht11_isChecksumOk(dht11 *this);

#endif //MOD_DHT11_H

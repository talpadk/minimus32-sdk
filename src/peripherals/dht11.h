#ifndef MOD_DHT11_H
#define MOD_DHT11_H

#include "io_function.h"
#include "external_interrupt.h" 

typedef enum {
  DHT11_INIT=0,
  DHT11_IDLE,
  DHT11_SENDING_START,
  DHT11_WAITING_FOR_ACK,
  DHT11_GOT_ACK_WAIT_IDLE,
  DHT11_WAITING_FOR_BIT_SYNC,
  DHT11_WAITING_FOR_BIT_START,
  DHT11_WAITING_FOR_BIT_END
} dht11_state;

dht11_state dht11_state_;
unsigned char dht11_rxByte_;
unsigned char dht11_rxBit_;

/** 
 * Initializes the dth11 sensor system
 * Can be used to reconfigure the IO functions between conversions
 * 
 * @param setOutput the io function pointer used to change the pin from input to output
 * @param out the io function pointer for setting the pin
 * @param extIrq the external interrupt associated with the IO pin
 */
void dht11_init(io_setOutput setOutput, io_outFunction out, uint8_t extIrq);


void dht11_irqAnimate();

/** 
 * Starts the conversion cycle.
 * Prior to calling this the timer1_clock MUST be initialized and running
 * 
 */

void dht11_startConversion();
dht11_state dht11_getConversionState();


#endif //MOD_DHT11_H

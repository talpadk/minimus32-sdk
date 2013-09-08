#include "dht11.h"

#include <avr/interrupt.h>
#include <util/atomic.h>

#include "timer1_clock.h"


io_setOutput dht11_setOutput_;
io_outFunction dht11_out_;
uint8_t dht11_extIr_;

timer1_callback dth11_callback_;

void dht11_setState(dht11_state state){
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    dht11_state_ = state;
  }
}

void dht11_sendStartDone(void *data){
  //bus hi with pull up
  dht11_setOutput_(0);
  dht11_out_(255);  

  enable_external_interrupt_input(dht11_extIr_, falling);

  dht11_setState(DHT11_WAITING_FOR_ACK);
}

void dht11_init(io_setOutput setOutput, io_outFunction out, uint8_t extIrq){
  disable_external_interrupt_input(extIrq);

  dht11_setOutput_  = setOutput;
  dht11_out_ = out;
  dht11_extIr_ = extIrq;

  //bus hi with pull up
  dht11_setOutput_(0);
  dht11_out_(255);

  dht11_setState(DHT11_INIT);
  
}

void dht11_irqAnimate(){
}

void dht11_startConversion(){
  dht11_setState(DHT11_SENDING_START);

  //Pull low
  dht11_setOutput_(1);
  dht11_out_(0);

  timer1_clock_register_callback (0,1, TIMER1_CLOCK_ONCE, dht11_sendStartDone, TIMER1_NO_USER_DATA, &dth11_callback_);
}

dht11_state dht11_getConversionState(){
  dht11_state result;
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    result = dht11_state_;
  }
  return result;
}

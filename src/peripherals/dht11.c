#include "dht11.h"

#include <util/atomic.h>
#include <avr/interrupt.h>

void dht11_setState(dht11 *this, dht11_state state){
  this->state = state;
}

void dht11_init_data(dht11 *this){
  this->rxByte = 0;
  this->rxBit = 8;
  this->data[0] = 0;
  this->data[1] = 0;
  this->data[2] = 0;
  this->data[3] = 0;
  this->data[4] = 0;
  this->data_ready = 0;
}


void dht11_sendStartDone(void *data){
  dht11 *this = (dht11*)data;
  //bus hi-z with pull up
  this->setOutput(io_PIN_INPUT);
  this->out(io_PIN_HIGH);  

  enable_external_interrupt_input(this->extIrq, falling);

  dht11_setState(this, DHT11_WAITING_FOR_ACK);
}


void dht11_init(dht11 *this, io_setOutput setOutput, io_outFunction out, uint8_t extIrq){
  disable_external_interrupt_input(extIrq);

  this->setOutput  = setOutput;
  this->out = out;
  this->extIrq = extIrq;

  dht11_init_data(this);

  //bus hi with pull up
  this->setOutput(io_PIN_INPUT);
  this->out(io_PIN_HIGH);

  dht11_setState(this, DHT11_STATE_INIT);
  
}

void dht11_irqAnimate(dht11 *this){
  dht11_state state;
  uint16_t ticks;

  ATOMIC_BLOCK(ATOMIC_FORCEON){
    ticks = timer1_get_ticks();
    state=dht11_getConversionState(this);

    switch(state){
    case DHT11_STATE_INIT:
    case DHT11_IDLE:
    case DHT11_SENDING_START:
      //We should never be called by a interrupt here. 
      dht11_setState(this, DHT11_STATE_INIT);
      break;
    case DHT11_WAITING_FOR_ACK:
      enable_external_interrupt_input(this->extIrq, rising);
      dht11_setState(this, DHT11_GOT_ACK_WAIT_IDLE);
      break;
    case DHT11_GOT_ACK_WAIT_IDLE:
      dht11_setState(this, DHT11_WAITING_FOR_BIT_SYNC);
      break;
    case DHT11_WAITING_FOR_BIT_SYNC:
      this->start_ticks = ticks;
      
      enable_external_interrupt_input(this->extIrq, falling);
      dht11_setState(this, DHT11_WAITING_FOR_BIT_END);
      break;
    case DHT11_WAITING_FOR_BIT_END:
      if (ticks<=this->start_ticks) { ticks += TIMER1_TICKS_PER_US*1000; }
      
      //Data sheet:0 bit = 26-28 us, 1 bit = 70 us
      //Real 20->all 255, 25-> some data. 75->all 0, 70 some data
      if ((ticks-this->start_ticks)>TIMER1_TICKS_PER_US*50){
	//bit = 1
	this->data[this->rxByte] = (this->data[this->rxByte]<<1) | 1;
      }
      else {
	//bit = 0;
	this->data[this->rxByte] = (this->data[this->rxByte]<<1);
      }

      this->rxBit--;
      if (this->rxBit == 0) {
	this->rxBit = 8;
	this->rxByte++;
      }
      if (this->rxByte<5){
	enable_external_interrupt_input(this->extIrq, rising);
	dht11_setState(this, DHT11_WAITING_FOR_BIT_SYNC);
      }
      else {
	dht11_setState(this, DHT11_IDLE);
	disable_external_interrupt_input(this->extIrq);
	this->data_ready = 1;
      }
      break;
    }
  }
}

void dht11_startConversion(dht11 *this){
  dht11_setState(this, DHT11_SENDING_START);

  dht11_init_data(this);

  //Pull low
  this->setOutput(io_PIN_OUTPUT);
  this->out(io_PIN_LOW);

  timer1_clock_register_callback (0, 20, TIMER1_CLOCK_ONCE, dht11_sendStartDone, this, &(this->callback));
}

dht11_state dht11_getConversionState(dht11 *this){
  return this->state;
}


uint8_t dht11_isDataReady(dht11 *this) {
  return this->data_ready;
}

uint8_t dht11_isChecksumOk(dht11 *this) {
  uint8_t sum = this->data[0]+this->data[1]+this->data[2]+this->data[3];
  return sum==this->data[4];
}

uint8_t dht11_getTemperature(dht11 *this){
  return this->data[2];
}

uint8_t dht11_getRelativeHumidity(dht11 *this){
  return this->data[0];
}

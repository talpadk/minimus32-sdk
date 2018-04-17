#include "twi.h"
#include <util/atomic.h>

uint8_t twiBusLock_ = 0;

uint8_t twi_obtain_bus(uint8_t blocking){
  uint8_t lockCopy;
  uint8_t doLoop=1;

  while (doLoop){    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      lockCopy = twiBusLock_;
      twiBusLock_=1;
    }
    if (!blocking || lockCopy==0) doLoop=0;
  }

  if (lockCopy==0) return 1;
  else return 0;
}

void twi_release_bus(void){
  twiBusLock_ = 0;
}

void twi_initBus(TWI_PRESCALER clockPrescaler, uint8_t bitRate){
  TWBR = bitRate;
  TWSR = clockPrescaler;
  TWCR |= 1<<TWEN;
}

void twi_deinitBus(void){
  TWCR &= ~(1<<TWEN);
}


#define TWI_STATUS_MASK (0b11111100)

#define TWI_WAIT() while (!(TWCR & (1<<TWINT))){}
#define TWI_STATUS (TWSR & TWI_STATUS_MASK)

TWI_ERROR twi_blocking_master_opperation(uint8_t address, uint8_t bufferLength, uint8_t *buffer, uint8_t flags){
  if (!(flags & TWI_DONT_SEND_START_COND)){
    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWSTA) | (1<<TWEN); //Send start condition
    TWI_WAIT()
    if (!(TWI_STATUS==0x08 ||
	  TWI_STATUS==0x10)){
      return TWI_ERROR_START_COND_NOT_SEND;
    }
    TWDR = address; //Load address to send
    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
    TWI_WAIT();
    if ((TWI_STATUS==0x20 ||
	 TWI_STATUS==0x48)){
      TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWSTO) | (1<<TWEN); //Send stop condition
      return TWI_ERROR_MISSING_ADDRESS_ACK;
    }
    else if (TWI_STATUS==0x38){
      TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //Return to slave mode
      return TWI_ERROR_ARBITRATION_LOST;
    }
  }
  if (address & TWI_ADDRESS_READ){
    //read loop
    while (bufferLength!=0){
      TWCR |= (1<<TWINT);
      TWI_WAIT();
      if (TWI_STATUS==0x58){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWSTO) | (1<<TWEN); //Send stop condition
	return TWI_ERROR_MISSING_DATA_ACK;
      }
      else if (TWI_STATUS==0x38){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //Return to slave mode
	return TWI_ERROR_ARBITRATION_LOST;
      }
      *buffer = TWDR;
      TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      buffer++;
      bufferLength--;
    }
  }
  else {
    //write loop
    while (bufferLength!=0){
      TWDR = *buffer;
      TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
      buffer++;
      bufferLength--;
      TWI_WAIT();
      if (TWI_STATUS==0x30){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWSTO) | (1<<TWEN); //Send stop condition
	return TWI_ERROR_MISSING_DATA_ACK;
      }
      else if (TWI_STATUS==0x38){
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //Return to slave mode
	return TWI_ERROR_ARBITRATION_LOST;
      }
    }
  }
  
  if (flags & TWI_SEND_STOP_COND){
    TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWSTO) | (1<<TWEN); //Send stop condition
  }
  
  return TWI_ERROR_NO_ERROR;
}

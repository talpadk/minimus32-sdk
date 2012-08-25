#include "spi.h"
#include <util/atomic.h> 


uint8_t spiBusLock_ = 0;

uint8_t spi_obtain_bus(uint8_t blocking){
  uint8_t lockCopy;
  uint8_t doLoop=1;

  while (doLoop){    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
      lockCopy = spiBusLock_;
      spiBusLock_=1;
    }
    if (!blocking || lockCopy==0) doLoop=0;
  }

  if (lockCopy==0) return 1;
  else return 0;
}

void spi_release_bus(void)
{
  //write only atomic isn't needed
  spiBusLock_=0;
}

void spi_setup(SpiSpeed speed, uint8_t flags){
  uint8_t tmp;

  flags = flags ^ ((1<<SPE)|(1<<MSTR));   //Toggle the inverse flags
  flags = flags | (speed&3);              //Mask and add speed
  SPCR = flags & (~(1<<SPE));             //Setup but disable (the disable part is only needed when in slave mode)
  if (SPSR & ((1<<SPIF)|(1<<WCOL))) {
    //clear SPIF and/ord WCOL
    tmp = SPDR;
  }
  if (speed & (1<<3)) SPSR = 1; //Double speed
  else SPSR = 0;                //Regular speed
    
  SPCR = flags; //Enable the SPI (if configured so by flags)
}

uint8_t spi_io(uint8_t txData)
{
  SPDR = txData;
  while (!(SPSR & (1<<SPIF))){};
  return SPDR;
}

void spi_config_io_for_master_mode(void){
  DDRB &= ~0b00001000; //Set PB3 (MISO) as input
  DDRB |= 0b00000111; //Set PB0 (SS), PB1 (SCLK) and PB2 (MOSI) as output
}

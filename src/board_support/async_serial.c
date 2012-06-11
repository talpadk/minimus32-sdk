#include "async_serial.h"

#include <avr/io.h>

void async_serial_init(SerialSpeed speed){
  switch(speed){
  case SERIAL_SPEED_9600:
    //Clear double speed bit and errors
    UCSR1A = 0;
    UBRR1 = 103;
    break;

  case SERIAL_SPEED_19200:
    UCSR1A = 0;
    UBRR1 = 51;
    break;   

  case SERIAL_SPEED_38400:
    UCSR1A = 0;
    UBRR1 = 25;
    break;   

  case SERIAL_SPEED_76800:
    UCSR1A = 0;
    UBRR1 = 12;
    break;   

  default: 
  case SERIAL_SPEED_115200:
    //double speed
    UCSR1A = 1<<U2X1;
    UBRR1 = 16;
    break;
  }
  
  //enable tx & rx, no ints
  UCSR1B = (1<<RXEN1) | (1<<TXEN1);
  //8N1 async data
  UCSR1C = (1<<UCSZ11)|(1<<UCSZ10);
}

char async_serial_byte_ready(){
  return UCSR1A & (1<<RXC1);
}

char async_serial_read_byte(){
  while (!(UCSR1A & (1<<RXC1))){}
  return UDR1;
}


void async_serial_write_byte(char data){
  while (!(UCSR1A & (1<<UDRE1))){}
  UDR1 = data;
}

void async_serial_write_string(const char *string){
  int i;
  for (i=0; string[i]; i++){
    async_serial_write_byte(string[i]);
  }
}


void async_serial_rx_interrupt(char on) {
  if (on)  UCSR1B = UCSR1B | 1<<RXCIE1;
  else     UCSR1B = UCSR1B & (~(1<<RXCIE1));
}

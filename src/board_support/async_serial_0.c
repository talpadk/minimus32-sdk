#include "async_serial_0.h"

#include <avr/io.h>

void async_serial_0_init(SerialSpeed speed){
  switch(speed){
  case SERIAL_SPEED_9600:
    //Clear double speed bit and errors
    UCSR0A = 0;
    UBRR0 = 103;
    break;

  case SERIAL_SPEED_19200:
    UCSR0A = 0;
    UBRR0 = 51;
    break;   

  case SERIAL_SPEED_38400:
    UCSR0A = 0;
    UBRR0 = 25;
    break;   

  case SERIAL_SPEED_76800:
    UCSR0A = 0;
    UBRR0 = 12;
    break;   

  default: 
  case SERIAL_SPEED_115200:
    //double speed
    UCSR0A = 1<<U2X0;
    UBRR0 = 16;
    break;
  }
  
  //enable tx & rx, no ints
  UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  //8N1 async data
  UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
}

char async_serial_0_byte_ready(){
  return UCSR0A & (1<<RXC0);
}

char async_serial_0_read_byte(){
  while (!(UCSR0A & (1<<RXC0))){}
  return UDR0;
}


void async_serial_0_write_byte(char data){
  while (!(UCSR0A & (1<<UDRE0))){}
  UDR0 = data;
}

void async_serial_0_write_string(const char *string){
  int i;
  for (i=0; string[i]; i++){
    async_serial_0_write_byte(string[i]);
  }
}


void async_serial_0_rx_interrupt(char on) {
  if (on)  UCSR0B = UCSR0B | 1<<RXCIE0;
  else     UCSR0B = UCSR0B & (~(1<<RXCIE0));
}

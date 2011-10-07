#include "async_serial.h"

#include <avr/io.h>

void async_serial_init(){
  //Clear double speed bit and errors
  UCSR1A = 0;
  //9600 baud
  UBRR1 = 103 ;
  
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

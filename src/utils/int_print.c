#include "int_print.h"


void uint8Print(uint8_t value, uint8_t length, char *buffer){
  uint8_t i;
  buffer[length]=0;
  buffer += length-1;
  for (i=0; i<length; i++){
    *buffer = (value % 10) + '0';
    buffer--;
    value /= 10;
  }
}

void uint16Print(uint16_t value, char *buffer){
	int8_t i;
	for (i=4; i>=0; i--){
		buffer[i]=(value%10)+'0';
		value /= 10;
	} 
}


void uint16PrintNull(uint16_t value, char *buffer){
	uint16Print(value, buffer);
	buffer[5]=0;
}


void uint16PrintDecimal(uint16_t value, uint8_t decimals, char *buffer){
	int8_t i;
  int8_t digets;
  
  if (decimals>5) { decimals=5; }
  digets = 5-decimals;
  
  for (i=0; i<decimals; i++){
		buffer[5-i]=(value%10)+'0';
		value /= 10;
	} 
  
  buffer[5-decimals]='.';
  
	for (i=0; i<digets; i++){
		buffer[5-decimals-1-i]=(value%10)+'0';
		value /= 10;
	} 
}

void uint16PrintDecimalNull(uint16_t value, uint8_t decimals, char *buffer){
  buffer[0]='X';
  buffer[1]='X';
  buffer[2]='X';
  buffer[3]='X';
  buffer[4]='X';
  buffer[5]='X';
	uint16PrintDecimal(value, decimals, buffer);
	buffer[6]=0;    
}

void replaceLeadingZerosN(char *buffer, uint8_t n){
  uint8_t count=0;
  
  while (*buffer=='0'){
    *buffer = ' ';
    buffer++;
    count++;
    if (n!=0 && count>=n){
      break;
    }   
  }
  if ((n!=0 && count>=n) ||
      ((*buffer>'9' || *buffer<'0') && count>0)){
    *(buffer-1)='0';
  }
}

void replaceLeadingZeros(char *buffer){
  replaceLeadingZerosN(buffer, 0);
}

const char *findStartOfNumber(const char *buffer){
  while (*buffer!=0 && (*buffer==' ' || *buffer=='0')){
    buffer++;
  }
  return buffer;
}

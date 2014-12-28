#include "int_print.h"


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


void uint16PrintCenti(uint16_t value, char *buffer){
	int8_t i;
  for (i=4; i>=3; i--){
		buffer[i+1]=(value%10)+'0';
		value /= 10;
	} 
  
  buffer[3]='.';
  
	for (i=2; i>=0; i--){
		buffer[i]=(value%10)+'0';
		value /= 10;
	} 
}

void uint16PrintCentiNull(uint16_t value, char *buffer){
	uint16PrintCenti(value, buffer);
	buffer[6]=0;    
}
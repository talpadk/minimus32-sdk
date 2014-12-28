#ifndef INT_PRINT_H
#define INT_PRINT_H

#include <stdint.h>
 
/** 
 * Prints an uint16_t to a buffer as a decimal number
 * 
 * @param value the value to print
 * @param buffer the buffer to print to (must be atleast 5 bytes long)
 */
void uint16Print(uint16_t value, char *buffer);

/** 
 * Prints an uint16_t to a buffer as a null terminated decimal number
 * 
 * @param value the value to print
 * @param buffer the buffer to print to (must be atleast 5+1 bytes long)
 */
void uint16PrintNull(uint16_t value, char *buffer);


void uint16PrintCenti(uint16_t value, char *buffer);

void uint16PrintCentiNull(uint16_t value, char *buffer);


#endif /* INT_PRINT_H */

#ifndef INT_PRINT_H
#define INT_PRINT_H

#include <stdint.h>

/**
 * @file 
 * @ingroup util
 */
  
  
    
///The buffer size required for uint16Print
#define UINT16_PRINT_SIZE (5)

///The buffer size required for uint16PrintNull
#define UINT16_PRINT_NULL_SIZE (UINT16_PRINT_SIZE+1)

///The buffer size required for uint16PrintDecimal
#define UINT16_PRINT_DECIMAL_SIZE (UINT16_PRINT_SIZE+1)

///The buffer size required for uint16PrintDecimalNull
#define UINT16_PRINT_DECIMAL_NULL_SIZE (UINT16_PRINT_DECIMAL_SIZE+1)
 
/** 
 * Prints an uint16_t (0-65535) to a buffer as a decimal number.
 * The number is printed with leading zeros.
 * 
 * @param value the value to print
 * @param buffer the buffer to print to (must be at least UINT16_PRINT_SIZE bytes long)
 */
void uint16Print(uint16_t value, char *buffer);

/** 
 * Prints an uint16_t (0-65535) to a buffer as a null terminated decimal number
 * The number is printed with leading zeros.
 * 
 * @param value the value to print
 * @param buffer the buffer to print to (must be at least UINT16_PRINT_NULL_SIZE bytes long)
 */
void uint16PrintNull(uint16_t value, char *buffer);

/** 
 * Prints an uint16_t (0-65535) to a buffer as a fix point decimal number
 * The number is printed with leading zeros and a decimal .
 * 
 * @param value the value to print
 * @param decimals the number of decimals after the decimal point, range 0-5 is allowed.
 * @param buffer the buffer to print to (must be at least UINT16_PRINT_DECIMAL_SIZE bytes long)
 */
void uint16PrintDecimal(uint16_t value, uint8_t decimals, char *buffer);

/** 
 * Prints an uint16_t (0-65535) to a buffer as a fix point null terminated decimal number
 * The number is printed with leading zeros and a decimal .
 * 
 * @param value the value to print
 * @param decimals the number of decimals after the decimal point, range 0-5 is allowed.
 * @param buffer the buffer to print to (must be at least UINT16_PRINT_DECIMAL_NULL_SIZE bytes long)
 */
void uint16PrintDecimalNull(uint16_t value, uint8_t decimals, char *buffer);

/**
 * Replaces the first n leading zeroes in a string with spaces
 *
 * @param buffer the buffer to replace '0' chars in.
 * @param n the number of character to replace at most, 0 has a special interpretation as "no limit"
 */
void replaceLeadingZerosN(char *buffer, uint8_t n);

/**
 * Replaces the leading zeroes in a string with spaces.
 *
 * @param buffer a null terminated string to replace in.
 */
void replaceLeadingZeros(char *buffer);

/**
 * Finds the beginning of a number string padded with either ' ' or '0'
 * Technically it simply finds the first non ' ' or '0' character and returns the string form there.
 *
 * @param buffer the to search for the start of a 
 */
const char *findStartOfNumber(const char *buffer);


#endif /* INT_PRINT_H */

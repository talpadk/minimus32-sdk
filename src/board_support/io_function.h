#ifndef MOD_IO_FUNCTION_H
#define MOD_IO_FUNCTION_H

/** 
 * A prototype for a IO function used for creating pointers to functions handling bit IO output
 * 
 * @param value if the bit(s) should be set value must be 255, 0 if it is to be unset
 */
typedef void(*io_outFunction)(unsigned char value);

/** 
 * A prototype for a IO function used for creating pointers to functions handling bit IO input
 * 
 * @return a non 0 value if the bit(s) are set, 0 if the bit is not set 
 */
typedef unsigned char(*io_inFunction)(void);

/** 
 * Sets the pin as an output.
 * If it is input it will be hi-z unless the pin has been: Set high and PUD is not set.
 * 
 * PUD is the global pull up disable, the reset conition is unset.
 * 
 * @param value 255 to set as output, 0 for input
 * 
 */
typedef void (*io_setOutput)(unsigned char value);


#endif //MOD_IO_FUNCTION_H

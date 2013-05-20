#ifndef MOD_IO_FUNCTION_H
#define MOD_IO_FUNCTION_H

/** 
 * A prototype for a IO function used for creating pointers to functions handling bit IO output
 * 
 * @param value if the bit(s) should be set value must be 255, 0 if it is to be unset
 */
typedef void(*ioOutFunction)(unsigned char value);

/** 
 * A prototype for a IO function used for creating pointers to functions handling bit IO input
 * 
 * @return a non 0 value if the bit(s) are set, 0 if the bit is not set 
 */
typedef unsigned char(*ioInFunction)(void);

#endif //MOD_IO_FUNCTION_H

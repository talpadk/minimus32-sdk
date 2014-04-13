#ifndef MOD_IO_FUNCTION_H
#define MOD_IO_FUNCTION_H

///For use with a io_outFunction, sets the pin high when the pin is configured as an output
/**@see io_setOutput
 */
#define io_PIN_HIGH (255)
///For use with a io_outFunction, sets the pin low when the pin is configured as an output
/**@see io_setOutput
 */
#define io_PIN_LOW  (0)
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

///For use with a io_setOutput function, defines the pin as a output
#define io_OUTPUT (255)

///For use with a io_setOutput function, defines the pin as a input
#define io_INPUT  (0)
/** 
 * Sets the pin as an output or input pin.
 * If it is an input pin it will be hi-z unless the pin has been: Set high using io_outFunction and the PUD bit is not set.
 * 
 * The PUD bit is the global pull up disable, the reset conition is unset.
 * 
 * @param value 255 to set as output, 0 for input
 * 
 */
typedef void (*io_setOutput)(unsigned char value);


#endif //MOD_IO_FUNCTION_H

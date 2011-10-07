#ifndef MOD_ASYNC_SERIAL
#define MOD_ASYNC_SERIAL

///Code for using the async serial port (RS-232).

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 */

/** 
 * Initialize the serial port
 *
 * @todo the init should take arguments defining speed and format
 * 
 */
void async_serial_init();

/** 
 * Returns true if there are data available on the serial port 
 * 
 * @return true/false
 */
char async_serial_byte_ready();

/** 
 * Blocking read a byte from the serial port
 * 
 * @see async_serial_byte_ready
 * @return a byte from the serial port
 */
char async_serial_read_byte();

/** 
 * Blocking write a byte to the serial port
 * 
 * @todo Write a output buffer empty test function
 * @param data 
 */
void async_serial_write_byte(char data);

/** 
 * Blocing write a string to the serial port
 * 
 * @param string the string to write
 */
void async_serial_write_string(const char *string);

#endif //MOD_ASYNC_SERIAL

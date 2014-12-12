#ifndef ASYNC_SERIAL_H
#define ASYNC_SERIAL_H

#include "serial.h"
#include "async_serial_1.h"

/**
 * @file   async_serial.h
 * @author Visti Andresen <talpa@Hermes.talpa.dk>
 * @date   Fri Dec 12 21:51:12 2014
 * 
 * @deprecated Please use one async_serial_1.h instead
 * @brief  Deprecated wrapper for old programs
 * 
 * 
 */

inline void async_serial_init(SerialSpeed speed) {async_serial_1_init(speed);}
inline char async_serial_byte_ready() {return async_serial_1_byte_ready();}
inline char async_serial_read_byte() {return async_serial_1_read_byte();}
inline void async_serial_write_byte(char data){async_serial_1_write_byte(data);}
inline void async_serial_write_string(const char *string) {async_serial_1_write_string(string);}
inline void async_serial_rx_interrupt(char on) {async_serial_1_rx_interrupt(on);}

#endif /* ASYNC_SERIAL_H */

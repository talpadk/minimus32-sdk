#ifndef MOD_SYS_CLOCK_H
#define MOD_SYS_CLOCK_H

///Code for controlling the system clock

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 */


#include <stdint.h>

/** 
 * Test the system clock prescaler
 * WARNING: Has the side effect of disabling the global interrupt flag
 * 
 * @param a the prescaler value, look up the value in the data sheet
 */
void clock_prescale_set_local(uint8_t a);

/** 
 * Sets the system clock prescaler to 1
 * WARNING: Has the side effect of disabling the global interrupt flag * 
 */
void clock_prescale_none();

#endif

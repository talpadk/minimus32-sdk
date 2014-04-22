#ifndef MOD_TIMER0_FAST_TIMER
#define MOD_TIMER0_FAST_TIMER

#define USES_RESOURCE_TIMER0 1

///Code for using the timer0 as a fast interrupt source.<br>

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 *
 * CAUTION: You can easily use more that 100% CPU this way.
 */


#include <stdint.h>
#include "timer0.h"

/** 
 * Sets up the timer 0 to generate an interrupt every time prescaler*counter system clocks have passed.
 * The user must define a ISR(TIMER0_COMPA_vect), the user must also enable global interups.
 *
 * As this timer is capable of gennerating lots of interrupts very fast it is imperative that the ISR is keept small and fast!
 *
 * 
 * 
 * @param timer0_prescaler the prescaler to use
 * @param counter the number of prescaled system clocks per interrupt
 */
void timer0_fast_timer_init(timer0_prescaler prescaler, uint8_t counter);

#endif //MOD_TIMER0_FAST_TIMER

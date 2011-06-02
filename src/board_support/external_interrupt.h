#ifndef MOD_EXTERNAL_INTERRUPT_H
#define MOD_EXTERNAL_INTERRUPT_H

#include <avr/interrupt.h>

/**
 * Defining edges to trigger the external interrupts
 */
typedef enum {
	lowlevel = 0b00,
	any = 0b01,
	falling = 0b10,
	rising = 0b11,
} interrupt_edge;


/**
 * Enable an interrupt as input
 * 
 * @param id the interrupt to enable as an input
 * @param edge the edge to trigger on
 */
void enable_external_interrupt_input(uint8_t id, interrupt_edge edge);

/**
 * Disable an interrupt as input
 * 
 * @param id the interrupt to disable as an input
 */
void disable_external_interrupt_input(uint8_t id);

#endif //MOD_EXTERNAL_INTERRUPT_H


#ifndef MOD_MINIMUS32_H
#define MOD_MINIMUS32_H

///Code for the Minimus32 LEDs and button.

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 * @ingroup board_minimus32
 */


/** 
 * Initializes the PORTD pins as input/output as required by the hardware functions of the minimus32.
 * This function must be called before using the other minimus32 functions
 */
void minimus32_init();

/** 
 * Sets the LED state for the red LED
 * 
 * @param on turns the led on if true
 */
void led_red(char on);

/** 
 * Toggles the red LED power state.
 */
void led_red_toggle();


/** 
 * Sets the LED state for the blue LED
 * 
 * @param on turns the led on if true
 */
void led_blue(char on);

/** 
 * Toggles the blue LED power state.
 */
void led_blue_toggle();

/** 
 * Returns true if the HWB button is pressed
 *  
 * @return true if the HWB button is pressed
 */
char hwb_is_pressed();

#endif //MOD_MINIMUS32_H

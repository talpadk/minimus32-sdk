#ifndef PROMICRO_H
#define PROMICRO_H

/**
 * @file   promicro.h
 * @author Visti Andresen
 * @brief  Board support for the LED pins on the Pro Micro
 * @ingroup bsp
 * 
 */


/** 
 * Initializes the Promicro pins used for the on board LEDs
 * 
 */
void promicro_init(void);

/** 
 * Controls the yellow led connected to PB0
 * 
 * @param on turns the LED on if true
 */
void led_yellow(char on);

/** 
 * Toggles the yellow led
 * 
 */
void led_yellow_toggle(void);

/** 
 * Controls the green led connected to PD5
 * 
 * @param on turns the LED on if true
 */
void led_green(char on);

/** 
 * Toggles the green LED
 * 
 */
void led_green_toggle(void);

#endif //PROMICRO_H

#ifndef MOD_SW_DITHER_H
#define MOD_SW_DITHER_H

///Algorithm for software ditering an output. (Analog output signal from a digital pin)

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 *
 * A more CPU fiendly alternative to pulse width modulation.
 * Unlike PWM dithering does not generate a fixed frequency signal
 */


#include <stdint.h>

typedef struct {
  uint16_t counter;
  uint16_t max;
  uint16_t power;
} sw_dither;

/** 
 * Initalizes the dither struct
 * 
 * @param dither the struct to initialize
 * @param max the value that when used as power will cause a always on signal, this value should not exceed 2^16/2
 */
void sw_dither_init(sw_dither *dither, uint16_t max);

/** 
 * Sets the power setting for the dither
 * 
 * @param dither the dither to update
 * @param power the power to set, may not exceed the max given at init
 */
void sw_dither_set(sw_dither *dither, uint16_t power);

/** 
 * Animates the software dithering, call often (enough) and with a constant interval
 * 
 * @param dither the dither to update
 * 
 * @return returns true (1) if you should output a high value, false (0) if low
 */
static inline uint8_t sw_dither_animate(sw_dither *dither){
  dither->counter += dither->power;
  if (dither->counter >= dither->max){
    dither->counter -= dither->max;
    return 1;
  }
  return 0;
}

#endif //MOD_SW_DITHER_H

#ifndef TIMER0_H
#define TIMER0_H

/**
 * @file   timer0.h
 * @author Visti Andresen <talpa@Hermes.talpa.dk>
 * @ingroup bsp
 * 
 * @brief  Some declarations for dealing with timer0
 * 
 * 
 */


typedef enum {
  ///Stop timer 0
  TIMER0_STOP = 0,
  TIMER0_1    = 1,
  TIMER0_8    = 2,
  TIMER0_64   = 3,
  TIMER0_256  = 4,
  TIMER0_1024 = 5,
  ///Clk source is TO falling edge
  TIMER0_TO_F = 6,
  ///Clk source is TO rising edge
  TIMER0_TO_R = 7
} timer0_prescaler;

#endif //TIMER0_H

#ifndef MOD_TIMER1_CLOCK_H
#define MOD_TIMER1_CLOCK_H

///A time of day clock with call me every n msec functionality

/**
 * @file
 * @author Visti Andresen
 * @ingroup bsp
 */


#include <stdint.h>

/** 
 * @file
 * timer1_clock takes ownership/uses the timer1 module of the MCU
 * 
 * timer1_clock provides a mili second resolution clock with callback functionality at given intervals and wall times.
 * WARNING all timer1 function expects the global int to be enabled
 */

#define TIMER1_TICKS_PER_US (16)

#define TIMER1_CLOCK_ONCE (0)
#define TIMER1_CLOCK_REPEAT (1)
#define TIMER1_NO_USER_DATA (0)

/** 
 * A struct that keeps track of time since timer1_clock_init
 * 
 */
typedef struct {
  ///Mili seconds since init, range 0-999.
  uint16_t msec; 
  ///Seconds since init, range 0-59.
  uint8_t sec;
  ///Minutes since init, range 0-59.
  uint8_t min;
  ///Hours since init, range 0-23.
  uint8_t hour;
  ///Days since init, range 0-65535. Note: overflows after roughly 179.6 years
  uint16_t day; 
  ///A free running counter of seconds since init, range 0-65535. Note: overflows after roughly 18.2 hours
  uint16_t freerunning_sec; 
} timer1_wall_time;

///Struct used for holding callback information
typedef struct timer1_callback{
  ///The next callback in the linked list
  struct timer1_callback *next;
  ///An abused wall time struct, used for determining "next" call time
  timer1_wall_time time;

  ///If true the alarm is recurring, only valid for periodic alarms
  uint8_t recurring;

  ///Used to store the miliseconds for recurring events;
  uint16_t recurring_msec;

  ///Data that will be passed on to the call back function
  void *user_data;
  
  void (*callback)(void *);

} timer1_callback;

/** 
 * Initializes the timekeeper and sets up the clock dividers.
 * This function assumes that Tsys=16MHz
 * @see clock_prescale_none
 * 
 */
void timer1_clock_init(void);


/** 
 * This function uses the Output compare B of Timer1 to generate a interrupt.
 * It sets the tick for the next interrupt as an offset from the current tick.
 *
 * Using this function has a lot less overhead than using registered callbacks,
 * and a lot less flexibility.
 * 
 * When using this function you MUST implement a ISR(TIMER1_COMPB_vect) interrupt vector.
 * In this IRQ function you must either call this function or timer1_sub_timer_b_unset,
 * failure to do so will result in the IRQ handler to be called again in 1ms
 *
 * WARNING: Calling this function enables global interrupts!
 * 
 * @param delay_ticks the number of ticks for the next interrupt, don't attemp to create a delay longer tha the 1ms loop period (16000 ticks) also a tiny delay will fail due to code execution time.
 */
void timer1_sub_timer_b_delay(uint16_t delay_ticks); 

/** 
 * Unsets the interrupt generation for sub timer b
 * 
 */
void timer1_sub_timer_b_unset(); 


/** 
 * Registers a callback into the clock
 * 
 * @param sec number of seconds that should pass before calling the callback.
 * @param msec aditional numer of mili seconds, range 0-999 is valid
 * @param recurring if true callback will be re-registered when called, otherwise it will be removed
 * @param callback a pointer to the funtion that will be called
 * @param user_data a pointer to user data that will be passed to the function, may be null.
 * @param callback_struct a pointer to the struct that will be initialized and linked into the clock
 */
void timer1_clock_register_callback(uint16_t sec, uint16_t msec, uint8_t recurring, void (*callback)(void *), void *user_data, timer1_callback *callback_struct);

/** 
 * Unregisters the given callback
 * 
 * @param callback_struct the callback to unregister
 */
void timer1_clock_unregister_callback(timer1_callback *callback_struct);

/** 
 * Returns the current "uptime"
 * 
 * @param time a pointer to the time struct used to hold the result
 */
void timer1_clock_get_time(timer1_wall_time *time);

/** 
 * Obtains a high resolution "timestamp", usefull for measuring the passage of a small time incremment.
 * The returned time is compensated for inconsistency at the cost of increesed jitter.
 * 
 * @param msecs the current mili second, loops ever 1 second
 * @param ticks the current tick (depends on the clock speed, 16 ticks per us @ 16MHz)
 */
void timer1_clock_get_micro_time(uint16_t *msecs, uint16_t *ticks);

/** 
 * Resets the current "uptime"
 * Messes with already registered callbacks, so use with care!
 */
void timer1_clock_reset();

#endif //MOD_TIMER1_CLOCK_H

#ifndef _LED_ALARM_CLOCK_ADC_H_
#define _LED_ALARM_CLOCK_ADC_H_

#include <stdint.h>

void initADCSystem(void);

/** 
 * Signal not available on newer HW
 * @deprecated
 * @return 
 */
uint16_t getInputVoltage();

/** 
 * Returns the current running though the LED
 * 
 * 
 * @return the number of mA running through the LED
 */
uint16_t getLEDCurrent();

/** 
 * Returns the LED heat sink temperature
 * 
 * 
 * @return the heat sink temperature in centi deg. C
 */
uint16_t getLEDTemperature();

void registerCurrentCallback(void (*callback)(uint16_t rawCurrent_));

#endif /* _LED_ALARM_CLOCK_ADC_H_ */

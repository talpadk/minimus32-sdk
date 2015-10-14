#ifndef _LED_ALARM_CLOCK_ADC_H_
#define _LED_ALARM_CLOCK_ADC_H_

#include <stdint.h>

void initADCSystem(void);

uint16_t getInputVoltage();

uint16_t getLEDCurrent();

void registerCurrentCallback(void (*callback)(uint16_t rawCurrent_));

#endif /* _LED_ALARM_CLOCK_ADC_H_ */

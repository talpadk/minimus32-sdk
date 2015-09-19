#ifndef ADC_SINGLE_ENDED_H
#define ADC_SINGLE_ENDED_H


#include <stdint.h>
#include <avr/io.h>

/**
 * @file   adc_single_ended.h
 * @author Visti Andresen <talpa@talpa.dk>
 * @date   Sun Dec 21 14:14:01 2014
 * 
 * @brief  Generic single ended ADC, written based on the Atmega328 documentation, but mostelikely works for others
 * 
 * 
 */

typedef enum {
	ADC0=0,
	ADC1=1,
	ADC2=2,
	ADC3=3,
	ADC4=4,
	ADC5=5,
	ADC6=6,
	ADC7=7,
	ADC_TEMP=8,
	ADC_1V1=14,
	ADC_GND=15
} ADC_Channel;


typedef enum {
	///External voltage applied to AREF pin
	ADC_REF_EXTERNAL=(0<<REFS0),

	///Use AVCC as reference
	ADC_REF_AVCC=(1<<REFS0),

	///Use internal 1.1V source as reference
	ADC_REF_1V1=(3<<REFS0),
} ADC_Reference;


typedef enum {
	ADC_DIV_2=0,
	ADC_DIV_4=2,
	ADC_DIV_8=3,
	ADC_DIV_16=4,
	ADC_DIV_32=5,
	ADC_DIV_64=6,
	ADC_DIV_128=7
} ADC_Speed;

///Readability define for adc_set_speed_and_enable
#define ADC_USE_INTERRUPTS (1)
///Readability define for adc_set_speed_and_enable
#define ADC_USE_POLLING (0)

///Readability define for adc_set_speed_and_enable
#define ADC_AUTO_TRIGGER (1)
///Readability define for adc_set_speed_and_enable
#define ADC_MANUAL_TRIGGER (0)

/** 
 * Sets the reference voltage for the ADC full scale value
 * 
 * @param reference the reference to use
 * @param alignLeft if true the result will be stored left aligned in the data registers
 */
void adc_set_reference_and_adj(ADC_Reference reference, char alignLeft);

/** 
 * Changes the channel the ADC works on.
 * Does not take effect before the current if any conversion is done.
 * 
 * @param channel the channel to switch to.
 */
void adc_set_channel(ADC_Channel channel);

/** 
 * Sets the ADC clock spped and enables the ADC
 * 
 * @param speed the clock divider to use, for full precision the clock should be keept inside 50-200kHz
 * @param useAutoTrigger if true the ADC is to use auto triggering
 * @param useInterrupts 
 */
void adc_set_speed_and_enable( ADC_Speed speed, char useAutoTrigger, char useInterrupts);

/** 
 * Starts a conversion and clears the interrupt/busy flag
 * 
 */
void adc_start_conversion(void);

/** 
 * Disables the ADC, but does not power the clock down.
 * 
 */
void adc_disable(void);

/** 
 * Returns true if the ADC i converting data after a start 
 * State is determined by polling the interrupt flag. 
 * 
 * @return true or false
 */
char adc_is_busy(void);

/** 
 * Returns the high and low byte of the last ADC conversion
 * 
 * 
 * @return the data from the registers
 */
uint16_t adc_get_result();

#endif /* ADC_SINGLE_ENDED_H */

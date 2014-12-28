#include "adc_single_ended.h"


void adc_set_reference_and_adj(ADC_Reference reference, char alignLeft){
	if (alignLeft) { alignLeft = 1<<ADLAR; }
	ADMUX = (ADMUX & 0xF0) | reference | alignLeft;
}

void adc_set_channel(ADC_Channel channel){
	ADMUX = (ADMUX & 0xF0) | channel;
}

void adc_set_speed_and_enable(ADC_Speed speed, char useAutoTrigger, char useInterrupts){
	if (useAutoTrigger) { useAutoTrigger = 1<<ADATE; }
	if (useInterrupts)  { useInterrupts  = 1<<ADIE; }

	ADCSRA = (1<<ADEN) | useAutoTrigger | useInterrupts | speed;
}

void adc_start_conversion(void){
	ADCSRA |= (1<<ADSC) | (1<<ADIF);
}

void adc_disable(void){
	ADCSRA &= ~(1<<ADEN);
}

char adc_is_busy(void){
	return !(ADCSRA & (1<<ADIF));
}

void adc_set_busy(void){
	ADCSRA |= (1<<ADIF);
}

uint8_t adc_get_low_result(){
	return ADCL;
}


uint8_t adc_get_hi_result(){
	return ADCH;
}

uint16_t adc_get_result(){
	return (ADCH<<8)+ADCL;
}

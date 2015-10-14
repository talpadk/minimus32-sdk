#include "led_alarm_clock_adc.h"

#include "adc_single_ended.h"
#include <util/atomic.h>

uint8_t adcConversionIndex_ = 0;

uint16_t rawCurrent_ = 0;
uint16_t rawInputVoltage_ = 0;
uint16_t rawLedTemperature_ = 0;
uint16_t rawLedVoltage_ = 0;

void (*currentCallback_)(uint16_t current_) = 0;
  
void startNextADCConversion(void){
  switch (adcConversionIndex_){
  default:
    adc_get_result(); //Should never happen
    break;
  case 0:
    adc_set_channel(ADC0); //Select input voltage as mux input
  case 2:
    adc_set_channel(ADC2); //Select input LED temperature as mux input
  case 4:
    adc_set_channel(ADC3); //Select input LED voltage as mux input    
    rawCurrent_ = adc_get_result();
    if (currentCallback_ != 0){
      currentCallback_(rawCurrent_);
    }
    break;
  case 1:
    rawInputVoltage_ = adc_get_result();
    adc_set_channel(ADC1);
    break;
  case 3:
    rawLedTemperature_ = adc_get_result();
    adc_set_channel(ADC1);
    break;
  case 5:
    rawLedVoltage_ = adc_get_result();
    adc_set_channel(ADC1);
    break;
  }
  adc_start_conversion();
  adcConversionIndex_++;
  if (adcConversionIndex_>5) { adcConversionIndex_=0; }
}

ISR(ADC_vect){
  startNextADCConversion();
}

void initADCSystem(void){
  adc_set_reference_and_adj(ADC_REF_AVCC, 0);
  adc_set_channel(ADC0);
  DIDR0 = 0b00001111; //Disable the digital inputs on the ADC pins used/
  //128kHZ @ 16MHZ
  adc_set_speed_and_enable(ADC_DIV_128, ADC_MANUAL_TRIGGER, ADC_USE_INTERRUPTS);

  startNextADCConversion();  
}

uint16_t getInputVoltage(){
  uint32_t result;
  
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    result = rawInputVoltage_;
  }
  //1024=5V from a 100k + 220k voltage divider
  result = (result*1600)/1024;
  return result;
}

uint16_t getLEDCurrent(){
  uint32_t result;
  
  ATOMIC_BLOCK(ATOMIC_FORCEON){
    result = rawCurrent_;
  }
  //5V = 877.19 mA @ 0.3 Ohm , 19 times gain
  //3.3V = 578.94 mA @ 0.3 Ohm , 19 times gain
  result = (result*579)/1024;
  return result;
}


void registerCurrentCallback(void (*callback)(uint16_t rawCurrent_)){
  currentCallback_ = callback;
}

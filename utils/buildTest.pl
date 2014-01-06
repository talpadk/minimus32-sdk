#!/usr/bin/perl
use strict;

my @targets = ('timing_test',
	       'lufa_serial_hello',
	       'LCD_HelloWorld',
	       'temperature_with_ds18s20',
	       'nokia_5110',
	       'external_button_busy-loop',
	       'external_button_interrupt',
	       'timer1_sub_timer',
	       'sd_card_test',
	       'capacative_sensing',
	       'stepper_l298n',
	       'stepper_uln2003a',
	       'temperature_with_tc77',
	       'hw_servo_test',
	       'nokia_lcd',
	       'sd_card/petit_fat_micro_test',
	       'serial_hello_test',
	       'capacative_sensing_led',
	       'hs-sr04_test',
	       'sw_rc_servo_test');


foreach my $target (@targets){
    print `./buildpp.pl --clean $target`;
}

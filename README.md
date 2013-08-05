# About the Minimus32 SDK
![A picture of a Minimus32](http://talpa.dk/Electronics/minimus32/minimus32.png)

The Minimus32 is a cheap ATMEGA32U2 development board which comes with very little support from its creators.

While its main focus is currently the ATMEGA32U2 the code should be reusable on other Atmel ATMEGA MCUs.    
Also the code isn't very specific to the Minimus32 other boards should work as well.    
(The Minimus32 after all is just the MCU and a few components)

If you are interested in a schematic of the Minimus32 see [my private web page](http://talpa.dk/?id=12&url=Electronics/minimus32/index.html) for a reverse engineered schematic.    
It also contains some tips regarding changing the voltage of the Minimus32. 


## Some Debian/Ubuntu packages needed/recommended

    sudo apt-get install doxygen graphviz gcc-avr avr-libc libusb-dev perl

## Building the code

Building the code will require:

* avr-gcc (packages: gcc-avr avr-libc)
* perl (required by buildpp.pl, but you could write you own makefiles)
* doxygen (if you want to generate the documentation, packages: doxygen graphviz)

While in the root of the repository issue the following command

      ./buildpp.pl timing_test

to build the tiny timing example/test program.  
(located under src/tests/timing_test.c)

## Uploading the code  

Uploading will require:

* dfu-programmer (to compile you need package: libusb-dev)


You can compile and auto upload the code using 

    ./buildpp.pl timing_test --test ""

Remember to put the Minimus32 into programming mode first (holding down HWB while pressing RESET)    
And user must have write permissions to the USB device.
# About the Minimus32 SDK
### The Minimus32
![A picture of a Minimus32](http://talpa.dk/Electronics/minimus32/minimus32.png)

The Minimus32 is a cheap ATMEGA32U2 development board which comes with very little support from its creators.

While its main focus is currently the ATMEGA32U2 the code should be reusable on other Atmel ATMEGA MCUs.    
Also the code isn't very specific to the Minimus32 other boards should work as well.    
(The Minimus32 after all is just the MCU and a few components)

If you are interested in a schematic of the Minimus32 see [my private web page](http://talpa.dk/?id=12&url=Electronics/minimus32/index.html) for a reverse engineered schematic.    
It also contains some tips regarding changing the voltage of the Minimus32. 

### The SDK
The SDK is written in C. The code is supposed to use the hardware functions inside the MCU to accelerate/offload the main code whenever possible.    
The functions for accessing these resources inside the MCU/"on the board" is located under src/board_support/    
When ever possible the functions should be "multi user" friendly for resources that might be used by more that one peripheral.    
Some examples:

* Asynchronous serial communication (RS232).
* SPI bus interface.
* Dallas one-wire protocol (software implementation).
* Timer based scheduling (HW timer1).
* Fast interrupts (HW timer 0).
* Digital dithering (analog out, software implementation).
* External interrupt setup.
* [LUFA](http://www.fourwalledcubicle.com/LUFA.php) based USB support.

The SDK also contains code for different peripherals (src/peripherals/), some examples:

* DS18S20 temperature sensor over Dallas one-wire protocol.
* Different LCD displays (std. text based 44780, Nokia 6100 based, Nokia 5110 (PCD8544 based)).
* H-bridges.
* RC servos.

Utility code (src/utils/), some examples

* VT100 terminal codes.
* SD card file system support (preliminary).
* Graphical font generators (utils/).

### Some Debian/Ubuntu packages needed/recommended

    sudo apt-get install doxygen graphviz gcc-avr avr-libc libusb-dev perl

## Building the code

Building the code will require:

* avr-gcc (packages: gcc-avr avr-libc)
* perl (required by buildpp.pl, but you could write you own makefiles)
* doxygen (if you want to generate the documentation, packages: doxygen graphviz)

The Minimus32-SDK uses [buildpp](http://sourceforge.net/projects/buildpp/) as build system.    
Buildpp is include in the source and doesn’t need to be installed

Buildpp is a build system that can auto detect the files needed to compile a project (based on the files included).
Removing the need to write makefiles specifying the dependencies by hand.

While in the root of the repository issue the following command

      ./buildpp.pl timing_test

to build the tiny timing example/test program.    
(located under src/tests/timing_test.c)

## Uploading the code  

Uploading will require:

* [dfu-programmer](http://sourceforge.net/projects/dfu-programmer/files/) (to compile you will need the package: libusb-dev)


You can compile and auto upload the code using 

    ./buildpp.pl timing_test --test ""

Remember to put the Minimus32 into programming mode first (holding down HWB while pressing RESET)    
And user must have write permissions to the USB device.   
See [this tutorial](http://minimus.smet.dk/?p=Compiling_in_Linux) for configuring udev to give the regular user write permissions by default.

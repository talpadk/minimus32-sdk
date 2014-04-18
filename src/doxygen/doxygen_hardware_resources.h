/**
 * @page HWRes Using hardware resources exclusively
 * 
 * When writing driver code it should be attempted to ensure that the user do not use two drivers that use the same resource/silicon hardware.
 *
 * There are two way to ensure this.
 * 
 * A) Use a hardware abstraction layer that allows different pieces of code to share the same silicon.
 * An example of this is spi.h, you may still need to take precautions to avoid hoging the resource by not releasing it.
 * 
 * B) Use the hardware as you please and write a define in you header file that generates a conflict with other header files that claims to use the same hardware.
 * spi.h does this with the define "#define USES_RESOURCE_SPI 1" this results in a compiler warning in case of a conflict
 *
 * == List of defined/existing resources ==
 * * #define USES_RESOURCE_SPI 1
 * * #define USES_RESOURCE_TIMER0 1
 * * #define USES_RESOURCE_TIMER1 1
 * * #define USES_RESOURCE_USART 1
 * 
 * If you create another resource define, please update this page!
 *
 */ 

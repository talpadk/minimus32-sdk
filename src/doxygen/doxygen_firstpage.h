/**
 * @mainpage
 *
 * @section intro Introduction
 * This is a collection of open source code for use with the different Atmel based boards
 * 
 * Micro controllers "supported" include:
 * * ATMEGA32U2
 * * ATMEGA32U4
 * * ATMEGA328
 * 
 * but as they are quite similar other variants may be used.
 *
 * Predefined boards include:
 * * Minimus32 (ATMEGA32U2, but seems to be out of production)
 * * Pro Micro (ATMEGA32U4)
 * * Pro Mini (ATMEGA328)
 * 
 * Using your own/different types are quite easy 
 * 
 *
 * @section building Building and uploading/testing the code
 * 
 * @see https://sourceforge.net/projects/buildpp/ for more details about the build system used
 * @see http://buildpp.sourceforge.net/ for the buildpp project home page.
 * 
 * @subsection compiling Compiling
 *
 * To compile the promicro_led_flasher.c "hello world" LED flasher issue the
 * following command from a terminal at the root of this project:
 * > ./buildpp.pl promicro_led_flasher
 * As you might have noticed you should not specify the path to the code.<br>
 * Which just happens to be located at src/tests/boards/promicro_led_flasher.c
 * 
 * @subsection compiling_and_upload Compiling and uploading / programming
 *
 * The following will both compile / build the code and upload it to the micro controller<br>
 * (using avrdude with a usbasp-clone programmer attached, take a look in localbuild.pl if you have a different programmer)
 * > ./buildpp.pl promicro_led_flasher -test ""
 *
 * @section folder_structure The folder structure
 * 
 * @verbatim
 build/            the output location the the compiled files will go.
 doc/              the output for the generated Doxygen documentation.
 fonts/            some graphical resources for displaying font on graphical displays.
 LUFA_*            a copy of the LUFA (Lightweight USB Framework for AVRs) sources, please see there web page for more information.
 src/              contains the project sources, see the Modules tab for more information.
 src/board_support code for micro controller blocks.
 src/doxygen       files that assist in generating this documentation.
 src/peripherals   code for handling peripherals external to the  micro controller. 
 src/programs      a place for programs that are not just demos / test code.
 src/tests         the location for examples and also code used to verify the functionality.
 src/utils         a place for helper / utility functions
@endverbatim 
 *
 */

$lazyLinking = 1;

$compiler = "avr-gcc";
$codeSuffix = "c";
$includeSuffix = "h|c";
$stripIncludePath = 1;
$cflags = "-mmcu=atmega32u2 -D F_CLOCK=16000000 -D F_CPU=16000000 -Wall -c -I/usr/lib/avr/include ";

$cflags .= '-std=gnu99 -D MCU=ATMEGA32U2 -D BOARD=NONE -D USB_DEVICE_ONLY -D FIXED_CONTROL_ENDPOINT_SIZE=8 -D FIXED_NUM_CONFIGURATIONS=1 -D USE_FLASH_DESCRIPTORS -D USE_STATIC_OPTIONS="(USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)" -D INTERRUPT_CONTROL_ENDPOINT ';


$linker = $compiler;
$ldflags = "-mmcu=atmega32u2 ";

$testProgram = "./upload_and_test.sh ";

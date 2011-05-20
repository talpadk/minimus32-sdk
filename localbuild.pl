$lazyLinking = 1;

$compiler = "avr-gcc";
$codeSuffix = "c";
$includeSuffix = "h|c";
$cflags = "-mmcu=atmega32u2 -Wall -c -I/usr/lib/avr/include ";


$linker = $compiler;
$ldflags = "-mmcu=atmega32u2 ";

$testProgram = "./upload_and_test.sh ";

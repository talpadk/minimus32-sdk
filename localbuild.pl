$lazyLinking = 1;
$useColours = 1;
$cleanConfirm = 0;


$compiler = "avr-gcc";
$codeSuffix = "c";
$includeSuffix = "h|c";
$stripIncludePath = 1;
$cflags = "-mmcu=atmega32u2 -O3 -D F_CLOCK=16000000 -D F_CPU=16000000 -Wall -c -I/usr/lib/avr/include ";

$cflags .= '-std=gnu99 -D MCU=ATMEGA32U2 -D BOARD=NONE -D USB_DEVICE_ONLY -D FIXED_CONTROL_ENDPOINT_SIZE=8 -D FIXED_NUM_CONFIGURATIONS=1 -D USE_FLASH_DESCRIPTORS -D USE_STATIC_OPTIONS="(USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)" -D INTERRUPT_CONTROL_ENDPOINT ';


$linker = $compiler;
$ldflags = "-mmcu=atmega32u2 ";

$testProgram = "./upload_and_test.sh ";

sub autoFunction
{
  my $dirName = $_[0];
  my $name = $_[1];
  my $workingDir = $_[2];
  my $utils = getcwd().'/utils';


  chdir($dirName);
  my $retVal = system("UTILS_PATH=$utils make -sqf $name")>>8;
  if ($retVal==1) {
      $rescanFiles = 1;
      system("UTILS_PATH=$utils make -sf $name");
  }
  chdir($workingDir);

}

$autoProcessingRef = \&autoFunction;

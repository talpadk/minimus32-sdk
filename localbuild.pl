$lazyLinking = 1;
$useColours = 1;
$cleanConfirm = 0;
$newTargetClean = 1;

#$showCompilerCommand = 1;


$compiler = "avr-gcc";
$codeSuffix = "c";
$includeSuffix = "h|c";
$stripIncludePath = 1;
$cflags = "-O3 -D F_CLOCK=16000000 -D F_CPU=16000000 -Wall -c -I/usr/lib/avr/include ";

$cflags .= '-std=gnu99 -D BOARD=NONE -D USB_DEVICE_ONLY -D FIXED_CONTROL_ENDPOINT_SIZE=8 -D FIXED_NUM_CONFIGURATIONS=1 -D USE_FLASH_DESCRIPTORS -D USE_STATIC_OPTIONS="(USB_DEVICE_OPT_FULLSPEED | USB_OPT_REG_ENABLED | USB_OPT_AUTO_PLL)" -D INTERRUPT_CONTROL_ENDPOINT ';

#LUFA settings
$cflags .= '-D ARCH=ARCH_AVR8 -D F_USB=16000000 '; 

$linker = $compiler;
$ldflags = "-O3 ";

$testProgram = "./upload_DFU.sh ";

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

#Set overwritable defines
sub beforeCompileRunFunction
{
	#handle mcu choice
	if ($cflags =~ /-D\s*MCU=([^\s]+)/){
		my $mcu = lc($1);
		$cflags .= "-mmcu=$mcu ";
		$ldflags .= "-mmcu=$mcu ";
	}
	else {
		$cflags .= "-D MCU=ATMEGA32U2 -mmcu=atmega32u2 ";
		$ldflags .= "-mmcu=atmega32u2 ";
	}

	#Auto detect programmer based on -mmcu in clags
	if ($cflags =~ /-mmcu\s*=\s*(\w+)/){
	    my $mcu = $1;
	    if ($mcu eq "atmega328p"){
		$testProgram = "./upload_ISP_avrdude.sh usbasp-clone m328p";
	    }
	    elsif ($mcu eq "atmega32u4"){
		$testProgram = "./upload_ISP_avrdude.sh usbasp-clone m32u4 ";
	    }
	}
	    
	
}
$beforeCompileRunRef = \&beforeCompileRunFunction;

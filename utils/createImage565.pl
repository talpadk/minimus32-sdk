#!/usr/bin/perl
use strict;

use File::Basename;
my $dirName = dirname(__FILE__);

require "$dirName/ppmUtils.pl";


if ($#ARGV!=0){
    die("Usage: createImage565.pl /path/to/image.pmm\n");
}

my $inputFilename=$ARGV[0];
my $baseFile;
if ($inputFilename =~ /^(.*)\.ppm/i){
    $baseFile = $1;
}
else {
    die ("File $inputFilename did not end in .ppm\n");
}
my $ppmDataRef_ = readPPM($inputFilename);
my $width = getWidth($ppmDataRef_);
my $height = getHeight($ppmDataRef_);
my $data = "";


sub outputRGB565 {
    (my $r, my $g, my $b) = @_;
    $r &= 0xF8;
    $g &= 0xFC;
    $b &= 0xF8;

    $r = $r | ($g>>5);
    $b = ($b>>3) | ($g & 0xE0);
    $data .= "$r,$b,";
}

for (my $y=0; $y<$height; $y++){
    for (my $x=0; $x<$width; $x++){
	my $r = getPixel($ppmDataRef_, $x, $y, 0, $width);
	my $g = getPixel($ppmDataRef_, $x, $y, 1, $width);
	my $b = getPixel($ppmDataRef_, $x, $y, 2, $width);
	outputRGB565($r,$g,$b);
    }
    $data .= "\\\n";
}
$data =~ s/,\\$//;









my $hFile;
my $cFile;

my $hFileName = $baseFile.'.h';
my $cFileName = $baseFile.'.c';
open($hFile, ">$hFileName") or die ("Cant write to $hFileName");
open($cFile, ">$cFileName") or die ("Cant write to $cFileName");
$baseFile =~ s/.*\///;

print $hFile '#ifndef MOD_IMAGE565_'.$baseFile."\n";
print $hFile '#define MOD_IMAGE565_'.$baseFile."\n\n";

print $hFile '#include "image565.h"'."\n\n";

print $hFile 'const image565 '.$baseFile.";\n\n";

print $hFile '#endif'."\n\n";

print $cFile '#include "'.$baseFile.".h\"\n#include <avr/pgmspace.h>\n\n";
print $cFile 'const uint8_t '.$baseFile."_bytes[] PROGMEM = {\n";
print $cFile $data;
print $cFile "};\n\n";


print $cFile 'const image565 '.$baseFile."={\n";



print $cFile "\t$width,\n";
print $cFile "\t$height,\n";
print $cFile "\t".$baseFile."_bytes\n";
print $cFile "};\n\n";

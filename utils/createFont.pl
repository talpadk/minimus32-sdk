#!/usr/bin/perl
use strict;

my $configName = $ARGV[0];
my $configFile;
my %config = ();

sub configTest{
    my $key = $_[0];
    if (!defined($config{$key})) { die("Config option '$key' missing\n"); }
}

open($configFile, "<$configName") or die("Unable to open config file '$configName'\n");
while (<$configFile>){
    my $line = $_;
    if ($line =~ /^\s*([^#]\w*)\s*=\s*(.+)$/){
	$config{$1}=$2;
    }
}
configTest('input');
configTest('output');
configTest('width');
configTest('height');
configTest('start');
configTest('end');

my $input;
my $inputName=$config{'input'};
open($input, "<$inputName") or die("Unable to open input file '$inputName'");
my $firstLine = <$input>;
if (!$firstLine =~ /^P3/) { die("PPM file did not start with a 'P3'\n"); }

my @data = ();

while (<$input>){
    my $line = $_;
    $line =~ s/#.*$//; #remove coments
    foreach my $token (split(/\s+/, $line)){
	if ($token =~ /^\d+$/){
	    $data[++$#data]=$token;
	}
    }
}

my $imgWidth = $data[0];
my $imgHeight = $data[1];
my $dataLength = $#data+1;
if ($dataLength != $imgWidth*$imgHeight*3+3){ die("Unexpected data length: $dataLength"); } 

my @buffer = ();
my $bufferBitNumber=0;
my $bufferByte=0;
sub getPixel{
    my $x = $_[0];
    my $y = $_[1];
    return $data[($x+$y*$imgWidth)*3+3];
}


my $hFile;
my $cFile;

my $hFileName = $config{'output'}.'.h';
my $cFileName = $config{'output'}.'.c';
open($hFile, ">$hFileName") or die ("Cant write to $hFileName");
open($cFile, ">$cFileName") or die ("Cant write to $cFileName");


print $hFile '#ifndef MOD_BIT_FONT_'.$config{'output'}."\n";
print $hFile '#define MOD_BIT_FONT_'.$config{'output'}."\n\n";

print $hFile '#include "bitfont.h"'."\n\n";

print $hFile 'const bitfont '.$config{'output'}.";\n\n";

print $hFile '#endif'."\n\n";


sub bufferFlush {
    if ($bufferBitNumber!=0){
	$buffer[++$#buffer]=$bufferByte<<(7-$bufferBitNumber);
    }
    #todo align and flush the last byte
    foreach my $byte (@buffer){
	print $cFile "$byte, ";
    }
    @buffer = ();
    print $cFile "\n";
}

sub printBit{
    my $pixel = $_[0];
    if ($pixel != 0) { 
	print $cFile '.';
    }
    else {
	print $cFile 'X';
	$bufferByte |= 1;
    }
    $bufferBitNumber++;
    if ($bufferBitNumber==8){
	$buffer[++$#buffer]=$bufferByte;
	$bufferByte = 0;
	$bufferBitNumber = 0;
    }
    else {   
	$bufferByte = $bufferByte<<1;
    }    
}

print $cFile '#include "'.$hFileName."\"\n#include <avr/pgmspace.h>\n\n";
print $cFile 'const uint8_t '.$config{'output'}."_bits[] PROGMEM = {\n";

my $index = 0;
for (my $i=$config{'start'}; $i<=$config{'end'}; $i++){
    print $cFile "//char $i\n";
    for (my $y=0; $y<$config{'height'}; $y++){
	print $cFile '// ';
	for (my $x=0; $x<$config{'width'}; $x++){
	    my $pixel = getPixel($index*$config{'width'}+$x, $y);
	    printBit($pixel);
	}
	print $cFile "\n";
    }
    bufferFlush();
    print $cFile "\n";
    $index++;
}
print $cFile "};\n\n";


print $cFile 'const bitfont '.$config{'output'}."={\n";


my $charSize=$config{'width'}*$config{'height'};
if ($charSize % 8 != 0) { $charSize = int($charSize/8)+1; }
else { $charSize = int($charSize/8); }

print $cFile "\t$config{'width'},\n";
print $cFile "\t$config{'height'},\n";
print $cFile "\t$charSize,\n";
print $cFile "\t$config{'start'},\n";
print $cFile "\t$config{'end'},\n";
print $cFile "\t$config{'output'}_bits\n";
print $cFile "};\n\n";

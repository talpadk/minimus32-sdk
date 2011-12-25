#!/usr/bin/perl
use strict;
use Math::Trig;


print '{';
my $length = 256;

for (my $i=0; $i<$length; $i++){
    my $value = (sin($i/2.0*pi/($length-1))*255.0);
    printf ("%.0f, ", $value);
}
print '}';

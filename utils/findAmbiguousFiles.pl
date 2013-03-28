#!/usr/bin/perl
#
# Finds .c and .h files that are not unique
#


use strict;

my @files = split(/\n/, `find . -name "*.h" -o  -name "*.c" `);

my %names = ();

foreach my $file (@files) {
    if ($file =~ /^(.+)\/([^\/]+)$/){
	my $path=$1;
	my $name=$2;

	if (!($path =~ /^.\/LUFA/)){next;}
	if ($path =~ /^.\/LUFA\/Demos/){next;}
	if ($path =~ /^.\/LUFA\/Projects/){next;}
	if ($path =~ /^.\/LUFA\/LUFA\/Drivers\/Board/){next;}


	my @empty = ();
	my $pathArrayRef=\@empty;
	
	if (defined($names{$name})){
	    $pathArrayRef=$names{$name};
	}
	push (@$pathArrayRef, ($path));
	$names{$name}=$pathArrayRef;
    }
}

foreach my $name (keys(%names)) {
    if (@{$names{$name}}!=1){
	print "=== $name ===\n";
	print "\t".join("\n\t", @{$names{$name}});
	print "\n";
    }
}

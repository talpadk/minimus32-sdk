use strict;

sub readPPM {
    my $inputFilename = $_[0];
    my $input;
    open ($input, "<$inputFilename") or die ("Unable to open $inputFilename\n");

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

    close($input);
    return \@data;
}

#type 0-2 for R,G,B components
sub getPixel {
    (my $data, my $x, my $y, my $type, my $imageWidth) = @_;
    return $$data[($x+$y*$imageWidth)*3+3+$type];
}

sub getWidth {
    (my $data) = @_;
    return $$data[0];
}

sub getHeight {
    (my $data) = @_;
    return $$data[1];
}


return 1;

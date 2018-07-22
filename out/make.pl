print "all:";
my @a = ();
my %d = ();
while(<>){
	chomp;
	if (m|^\.\./nbts/([^_]+)_(.*)|) {
		my $s = "$1/$2.gz";
		$d{$1} = 1;
		print " $s";
		push(@a, "$s: $_\n\tgzip -c $_ >$s\n");
	}
}
print "\n";
print for (@a);
mkdir for (keys %d);

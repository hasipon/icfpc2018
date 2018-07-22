print "all:";
my @a = ();
my %d = ();
while(<>){
	chomp;
	if (m|^\.\./nbts/([^_]+)_(.*)|) {
		my $s = "$1/$2.gz";
		my $t = "$1/$2.sc6";
		my $p = $2;
		$p =~ s/.nbt$//;
		$d{$1} = 1;
		print " $s $t";
		push(@a, "$s: $_\n\tgzip -c $_ >$s\n");
		push(@a, "$t: $_\n\t../tracer6 ../problemsF/${p}_src.mdl ../problemsF/${p}_tgt.mdl < $_ >$t\n");
	}
}
print "\n";
print for (@a);
mkdir for (keys %d);

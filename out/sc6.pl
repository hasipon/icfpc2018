print "all:";
my @a = ();
while(<>){
	chomp;
	my $s = $_;
	s/\.gz$/.sc6/;
	my $t = $_;
	print " $t";
	s|.*/||;
	s|\..*||;
	my $p = $_;
	push(@a, "$t: $s\n\tgzip -dc $s | ../tracer6 ../problemsF/${p}_src.mdl ../problemsF/${p}_tgt.mdl > $t.tmp && mv $t.tmp $t || mv $t.tmp $t.error\n");
}
print "\n";
print for (@a);

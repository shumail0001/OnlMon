#! /usr/bin/perl
# tar up all Run_*.root histo files run by run into directory
# given in second arg

use warnings;
use strict;
use Getopt::Long;


my $dryrun;
GetOptions("test"=>\$dryrun);

if ( $#ARGV < 1 )
{
    print "\nthis tars up all histogram files of a given run, arguments:\n 1st: indir, 2nd: outdir\n\n";
    print"flags:\n";
    print "--test: run in test mode\n";
    exit(2);
}

if (defined $dryrun)
{
    print "running in test mode\n";
}


if (! -d $ARGV[0])
{
    print "input directory $ARGV[0] not found\n";
    exit(2);
}
if (! -d $ARGV[1])
{
    print "output directory $ARGV[1] not found\n";
    exit(2);
}


my %map = ();

open(F,"find $ARGV[0]  -maxdepth 1 -type f -name 'Run*.root' |");


while (my $line = <F> )
{
    chomp $line;
    if ($line =~ /Run_(\d+)-.+_.+/)
    {
	my $runstr=$1;
	$map{$runstr}++;
    }
    elsif ($line =~ /Run_-(\d+)-.+_.+/)
    {
	die "negative runnumber -$1 in file $line\n";
    }
    else
    {
	die "could not extract run number from $line\n";
    }
}
close(F);

foreach my $i (sort keys %map)
{
    print "Tarring Run $i\n";
    if (! defined $dryrun)
    { 
	system("cd $ARGV[0]; tar -czvf $ARGV[1]/Run_$i.tar.gz *_$i\-*");
    }
}

exit

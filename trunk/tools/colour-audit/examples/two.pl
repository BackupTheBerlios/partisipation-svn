#!/usr/bin/perl -w

use strict;
use Audit::Source::Flawfinder;
use Audit::Source::Rats;

my $file = shift @ARGV;
my $rats = Audit::Source::Rats->new();
my $flawfinder = Audit::Source::Flawfinder->new();

# run both seperately
$rats->run($file);
$flawfinder->run($file);

# merge results into $flawfinder 
$flawfinder->merge($rats);
$flawfinder->textdump();



#!/usr/bin/perl -w

use strict;
use Audit::Source;

# create new audit object
my $audit = new Audit::Source;

# process with all available scanners
$audit->run_all($ARGV[0]);
 
# show text report
$audit->textdump();


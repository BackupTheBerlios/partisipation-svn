#!/usr/bin/perl -w

use strict;
use Audit::Source;

my $audit = new Audit::Source;
my $file = $ARGV[0] || die "usage: three.pl <file>\n";

# process with all available scanners
if (!$audit->run_all($file)) {
  die "scanning error\n";
}
 
# get lines with warnings
my @lines = $audit->lines($file);
print "these lines produced warnings: ", join(", ", @lines), "\n\n";

# get entries for the first line
my $entries = $audit->entries_hashref($file, $lines[0]);

# get the first entry
my $entry = shift @{$entries->{'data'}};

print "the first entry is:\n";
print "     line: ", $lines[0], "\n";
print "   source: ", $entry->{'source'}, "\n";
print " severity: ", $entry->{'severity'}, "\n";
print "  warning: ", $entry->{'desc'}, "\n\n";


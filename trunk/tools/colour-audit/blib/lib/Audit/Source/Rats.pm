#!/usr/bin/perl -w
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 dated June, 1991.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program;  if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111, USA.
#
#  Copyright 2004, Max Vozeler <max@hinterhof.net>
#

package Audit::Source::Rats;

=head1 NAME

Audit::Source::Rats - module to process rats(1) output

=head1 SYNOPSIS

 use Audit::Source::Rats;
 $rats = new Audit::Source::Rats;
 $rats->run($file);
 $rats->textdump();

=head1 DESCRIPTION

B<Audit::Source::Rats> provides an interface to the rats(1)
source code vulnerability scanner. See Audit::Source(3pm) for more
information.

=cut

use strict;
use vars qw(@ISA @EXPORT @EXPORT_OK);
use Exporter;
use Audit::Source;

@ISA = qw(Exporter Audit::Source);
@EXPORT = qw();
@EXPORT_OK = qw(&parse_filehandle &parse_block);

=head1 METHODS

These methods are available in addition to the methods inherited from 
Audit::Source:

=over 4

=item $rats = B<new> Audit::Source::Rats

Creates a new Audit::Source::Rats object.

=cut

sub new {
    my $class = shift;
    my $self = { };

    bless($self, $class);
    $self->{COMMAND} = 'rats';
    $self->{COMMAND_ENV} = { };
    $self->{COMMAND_ARGS} = [ ];
    $self->{FILES} = { };
    $self->{VERBOSE} = 0;

    $self->_activate();

    return $self;
}

=item $rats->B<parse_filehandle>(*HANDLE)

Reads and parses rats output from filehandle HANDLE.

=cut

sub parse_filehandle {
    my $self = shift;
    my $handle = shift;
  
    # paragraphs
    local $/ = '';
  
    while (<$handle>) {
        # strip useless commentary
        s/Entries in \w+ database: \d+\n//sg;
        s/Analyzing [^\n]+\n//sg;
        s/Total lines analyzed: \d+\n//sg;
        s/Total time \d+\.\d+ seconds\n//sg;
        s/\d+ lines per second\n//;
    
        $self->parse_block($_);
    }
  
    return 1;
}

=item $rats->B<exitcheck>($?)

Interprets the exit status.

=cut

sub exitcheck {
    my $self = shift;
    my $code = shift;

    # anything but zero is an error
    if ($code != 0) {
        return undef;
    }

    return 0;
}

=item $rats->B<mapseverity>($s)

Maps rats severity $s to standard severity.

=cut

my %smap = (
   'Low' => 1,
   'Medium' => 2,
   'High' => 3
);

sub mapseverity {
   my $self = shift;
   my $in = shift;
   warn if ! exists $smap{$in};
   return $smap{$in};
}

sub parse_block {
    my $self = shift;
    my $block = shift;
    my $file;
    my $severity;
    my $description;
    my @lines;
  
    # ignore empty blocks
    return 1 if length $block == 0;
  
    while ($block =~ s/^([^:]+):(\d+): (\w+): ([^\n]*\n)//s) {
        push(@lines, $2);
        $file = $1;
        $severity = $3;
        $description = $4;
    }
  
    if ($#lines == -1) {
        # parse error
        return undef;
    }
  
    if (length $block) {
        # long description follows
        chomp($block);
        $description .= $block;
    }
  
    foreach my $line (@lines) {
        push(@{$self->{FILES}->{$file}->{$line}->{'data'}}, {
          'file' => $file,
          'line' => $line,
          'severity' => $severity,
          'desc' => $description,
          'source' => ref($self)
        });
    }
  
    return 1;
}

=back

=head1 SEE ALSO

=over

=item L<Audit::Source(3pm)>

=item L<Audit::Source::Splint(3pm)>

=item L<Audit::Source::Pscan(3pm)>

=item L<Audit::Source::Flawfinder(3pm)>

=back

=head1 AUTHOR

Max Vozeler <max@hinterhof.net>

=cut

1;


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

package Audit::Source::Flawfinder;

=head1 NAME

Audit::Source::Flawfinder - module to process flawfinder(1) output

=head1 SYNOPSIS

 use Audit::Source::Flawfinder;
 $flaw = new Audit::Source::Flawfinder;
 $flaw->run($file);
 $flaw->textdump();

=head1 DESCRIPTION

B<Audit::Source::Flawfinder> provides an interface to the flawfinder(1)
source code vulnerability scanner. See Audit::Source(3pm) for more
information.

=cut

use strict;
use vars qw(@ISA @EXPORT @EXPORT_OK);
use Exporter;

@ISA = qw(Exporter Audit::Source);
@EXPORT = qw();
@EXPORT_OK = qw(&parse_filehandle &parse_block);

=head1 METHODS

These methods are available in addition to the methods inherited from 
Audit::Source:

=over 4

=item $flaw = new Audit::Source::Flawfinder

Creates a new Audit::Source::Flawfinder object.

=cut

sub new {
    my $class = shift;
    my $self = { };

    bless($self, $class);
    $self->{COMMAND} = 'flawfinder';
    $self->{COMMAND_ENV} = { };
    $self->{COMMAND_ARGS} = [ ];
    $self->{FILES} = { };
    $self->{VERBOSE} = 0;

    $self->_activate();

    return $self;
}

=item $flaw->B<exitcheck>($?)

Interprets the exit status. Returns 1 for success or 0 for failure.

=cut

sub exitcheck {
    my $self = shift;
    my $code = shift;

    # everything but zero is an error
    if ($code != 0) {
        return undef;
    }

    return 0;
}

=item $flaw->B<mapseverity>($s)

Maps flawfinder risk levels (1 .. 5) to standard severity.

=cut

sub mapseverity {
   my $self = shift;
   my $in = shift;
   return $in;
}

=item $flaw->B<parse_filehandle>(*HANDLE)

Reads and parses flawfinder output from filehandle HANDLE.

=cut

sub parse_filehandle {
    my $self = shift;
    my $handle = shift;
    my $line = '';

    # simple pattern: every line with information starts
    # with a / or whitespace; we ignore everything else
    while (<$handle>) {
        next unless m,^[/ ],;
        if (m,^ ,) {
            # continuation
            $line .= $_;
        } else {
            # new block
            if (length $line) {
                # process previous block
                $self->parse_block($line);
                $line = $_;
            } else {
                # start new block
                $line = $_;
            }
        }
    }

    # last block
    $self->parse_block($line);

    return 0;
}

sub parse_block {
  my $self = shift;
  my $block = shift;

  # ignore empty blocks
  return 1 if length $block == 0;

  if ($block =~ /([^:]+):(\d+):\s+\[(\d+)\]\s+(.+)/s) {
      push(@{$self->{FILES}->{$1}->{$2}->{'data'}}, {
        'file' => $1,
        'line' => $2,
        'severity' => $3,
        'desc' => $4,
        'source' => ref($self)
      });
      return 1;
  }
  
  # parse error
  printf STDERR "%s: parse error in block '%s'\n", ref($self), $block;
  return undef;
}


=back

=head1 SEE ALSO

=over

=item L<Audit::Source(3pm)>

=item L<Audit::Source::Splint(3pm)>

=item L<Audit::Source::Pscan(3pm)>

=item L<Audit::Source::Rats(3pm)>

=back

=head1 AUTHOR

Max Vozeler <max@hinterhof.net>

=cut

1;


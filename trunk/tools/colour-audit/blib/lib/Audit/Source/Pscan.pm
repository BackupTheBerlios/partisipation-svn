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

package Audit::Source::Pscan;

=head1 NAME

Audit::Source::Pscan - module to process pscan(1) output

=head1 SYNOPSIS

 use Audit::Source::Pscan;
 $pscan = new Audit::Source::Pscan;
 $pscan->run($file);
 $pscan->textdump();

=head1 DESCRIPTION

B<Audit::Source::Pscan> provides an interface to the pscan(1)
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

=item $pscan = B<new> Audit::Source::Pscan

Creates a new Audit::Source::Pscan object.

=cut

sub new {
    my $class = shift;
    my $self = { };

    bless($self, $class);
    $self->{COMMAND} = 'pscan';
    $self->{COMMAND_ENV} = { };
    $self->{COMMAND_ARGS} = [ '-w' ];
    $self->{FILES} = { };
    $self->{VERBOSE} = 0;

    $self->_activate();

    return $self;
}

=item $pscan->B<parse_filehandle>(*HANDLE)

Reads and parses pscan output from filehandle HANDLE.

=cut

sub parse_filehandle {
    my $self = shift;
    my $handle = shift;
  
    while (<$handle>) {
        $self->parse_line($_);
    }
  
    return 1;
}

sub parse_line {
    my $self = shift;
    my $line = shift;
    
    if ($line =~ /([^:]+):(\d+) (\w+): ([^\n]*\n)/s) {
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
    printf STDERR "%s: parse error in line '%s'\n", ref($self), $line;
    return undef;
}

=item $pscan->B<exitcheck>($?)

Interprets the exit status. Always returns 0 for success.

=cut

sub exitcheck {
    my $self = shift;
    my $code = shift;

    # can't use exit code
    #  0  -> nothing found
    #  1  -> error OR warning
    #  2  -> flex error
    return 0;
}

=item $pscan->B<mapseverity>($s)

Maps rats severity $s to standard severity.

=cut

my %smap = (
   'Warning' => 3,
   'SECURITY' => 5,
);

sub mapseverity {
   my $self = shift;
   my $in = shift;
   warn if ! exists $smap{$in};
   return $smap{$in};
}

=back

=head1 SEE ALSO

=over

=item L<Audit::Source(3pm)>

=item L<Audit::Source::Splint(3pm)>

=item L<Audit::Source::Flawfinder(3pm)>

=item L<Audit::Source::Rats(3pm)>

=back

=head1 AUTHOR

Max Vozeler <max@hinterhof.net>

=cut

1;


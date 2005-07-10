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

package Audit::Source::Splint;

=head1 NAME

Audit::Source::Splint - module to process splint(1) output

=head1 SYNOPSIS

 use Audit::Source::Splint;
 $splint = new Audit::Source::Splint;
 $splint->run($file);
 $splint->textdump();

=head1 DESCRIPTION

B<Audit::Source::Splint> provides an interface to the splint(1)
source code vulnerability scanner. See Audit::Source(3pm) for more
information.

=cut

use strict;
use vars qw(@ISA @EXPORT @EXPORT_OK);
use Exporter;
use Carp;

@ISA = qw(Exporter Audit::Source);
@EXPORT = qw();
@EXPORT_OK = qw(&parse_filehandle &parse_block);

# splint is very sensitive to missing headers. These args include
# some "common" include paths in hope of preventing parse errors.
# They also relax the checks down to -weak and disable some noisy
# warnings. 
my @SP_ARGS = (
    '-I.',
    '-I..',
    '-I../..',
    '-I../../..',
    '-Ilib',
    '-Iinclude',
    '+quiet',
    '-warnposix',
    '-showfunc',
    '-weak'
);

my %SP_ENV = (
    'LARCH_PATH' => '/usr/share/splint/lib/',
    'LCLIMPORTDIR' => '/usr/share/splint/imports/'
);

=head1 METHODS

These methods are available in addition to the methods inherited from 
Audit::Source:

=over 4

=item $splint = B<new> Audit::Source::Splint

Creates a new Audit::Source::Splint object.

=cut

sub new {
    my $class = shift;
    my $self = { };
  
    bless($self, $class);

    $self->{COMMAND} = "splint";
    $self->{COMMAND_ENV} = \%SP_ENV;
    $self->{COMMAND_ARGS} = \@SP_ARGS; 
    $self->{FILES} = { };
    $self->{VERBOSE} = 0;
    
    $self->_activate();

    return $self;
}

=item $splint->B<exitcheck>($?)

Interprets the exit status. Always returns 0 for success.

=cut

sub exitcheck {
    my $self = shift;
    my $code = shift;

    # splint's exit status is useless
    #
    # parsed ok, no errors: 0
    # parsed ok, warnings: 1
    # parse aborted due to errors: 1
    #
    # always return success
    return 0;
}

=item $splint->B<parse_filehandle>(*HANDLE)

Reads and parses splint output from filehandle HANDLE.

=cut

sub parse_filehandle {
    my $self = shift;
    my $handle = shift;
    my $line = '';
  
    # blocks are continued with whitespace
    # indented lines.
    while (<$handle>) {
        if (m,^[\t ],) {
            # continuation
            $line .= $_;
        } else {
            # new block
            if (length $line) {
                # process previous block
                $self->parse_block($line) || return undef;
                $line = $_;
            } else {
                # start new block
                $line = $_;
            }
        }
    }
  
    # last block, if any
    $self->parse_block($line) || return undef;
  
    return 0;
}

sub parse_block {
    my $self = shift;
    my $block = shift;
  
    # ignore empty blocks
    return 1 if length $block == 0;
  
    if ($block =~ /([^:]+):(\d+):(\d+):\s+(.+)/s) {
        my ($file, $line, $desc) = ($1, $2, $4);
        
        # catch parse errors reported by splint
        if ($block =~ /Parse Error\./s) {
            print STDERR ref($self),
              ": splint stopped due to parse errors.\n\n";
            return undef;
        }
        
        $file = $self->_absolutepath($file);
        push(@{$self->{FILES}->{$file}->{$line}->{'data'}}, {
          'file' => $file,
          'line' => $line,
          'severity' => 'unknown', # $3 is "error type"
          'desc' => $desc,
          'source' => ref($self)
        });

        return 1;
    }
    
    # ignore whitespace-only blocks
    if ($block =~ /^\s+$/s) {
        return 1;
    }
    
    if ($block =~ /Preprocessing error for file:/s) {
        chomp($block);
        printf STDERR "%s: %s\n%s: %s\n", ref($self),
          "splint stopped due to preprocessing errors.", ref($self),
          "Try adding -I<includepath>\n\n  '$block'\n";
        return undef;
    }
        
    # silently return error on "Cannot continue"
    if ($block =~ /Cannot continue\./) {
        return undef;
    }
    
    # unrecognized pattern
    chomp($block);
    printf STDERR "%s: parse error in block\n  '%s'\n", ref($self), $block;
    return undef;
}

sub includepaths {
    my $self = shift;
    my @paths = @_;
    foreach (@paths) {
        push(@{$self->{INCLUDEPATHS}}, $_);
        push(@{$self->{COMMAND_ARGS}}, "-I$_");
    }
    unless (exists $self->{INCLUDEPATHS}) {
        $self->{INCLUDEPATHS} = [ ];
    }
    return @{$self->{INCLUDEPATHS}};
}

=back

=head1 SEE ALSO

=over

=item L<Audit::Source(3pm)>

=item L<Audit::Source::Rats(3pm)>

=item L<Audit::Source::Pscan(3pm)>

=item L<Audit::Source::Flawfinder(3pm)>

=back

=head1 AUTHOR

Max Vozeler <max@hinterhof.net>

=cut

1;


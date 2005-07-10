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

package Audit::Source;

require 5.0008;

$Audit::Source::VERSION = '0.10';

=head1 NAME

Audit::Source - an interface to source code vulnerability scanners

=cut

=head1 SYNOPSIS

 use Audit::Source;

=head1 DESCRIPTION

B<Audit::Source> is a group of modules to make processing of source
code vulnerability scanners easier. Each scanner is implemented as a
separate module (see B<MODULES>) which parses the results and stores
them into a common data format. Results from different scanners can
be matched against each other.

=cut

use strict;
use vars qw(@ISA @EXPORT @EXPORT_OK);
use Exporter;

@ISA = qw(Exporter);
@EXPORT = qw();
@EXPORT_OK = qw();

use Carp;
use Cwd 'abs_path';
use Text::Wrap;

=head1 MODULES

These scanner modules are part of Audit::Source:

=over 4

=item Audit::Source::Splint

B<splint> (Secure Programming Lint),

=item Audit::Source::Rats

B<rats> (Rough Auditing Tool for Security).

=item Audit::Source::Flawfinder

B<flawfinder>

=item Audit::Source::Pscan

B<pscan>

=back

=cut

use Audit::Source::Splint;
use Audit::Source::Flawfinder;
use Audit::Source::Pscan;
use Audit::Source::Rats;

=head1 METHODS

=over 4

=item new Audit::Source

Creates a new Audit::Source object.

=cut

sub new {
    my $class = shift;
    my $self = { };

    bless($self, $class);
    $self->{FILES} = { };
    $self->{ACTIVE} = 0;
    $self->{VERBOSE} = 0;

    return $self;
}

=item $audit->B<includepaths>(@paths)

Adds @paths to the header search paths.

  $audit->includepaths("/usr/local/include");
  $audit->includepaths("lib", "common");
  @paths = $audit->includepaths;

=cut

sub includepaths {
    my $self = shift;
    my @adds = @_;
    
    push(@{$self->{INCLUDEPATHS}}, @adds);

    if (!exists $self->{INCLUDEPATHS}) {
        return undef;
    }

    return @{$self->{INCLUDEPATHS}};
}

=item $audit->B<run>($file)

Should not be called on Audit::Source objects. Use run_all() instead.

=cut

sub run {
    my $self = shift;
    my $file = shift;
    my @args;
    my $npath;
    my $pid;

    croak "file unspecified" if !defined $file;
    croak "not a scanner module " if !exists $self->{COMMAND};
    croak ref($self), " is not available" if $self->{ACTIVE} != 1;
    croak "cannot read $file" if ! -r $file;

    # make sure $file is an absolute path
    if ($file !~ m|^/|) {
        $file = abs_path($file);
    }

    $self->{COMMAND_PATH} = $file; 
    $self->{COMMAND_PATH} =~ s|/[^/]+$|/|;

    if (defined $self->{COMMAND_ARGS}) {
        @args = @{$self->{COMMAND_ARGS}};
    }

    push(@args, $file);
    
    # fork and exec the scanner
    $pid = open(SCANNER, "-|");
    if ($pid == 0) {
        # dup sterr to stdout
        open(STDERR, ">&", STDOUT);
        if (defined $self->{COMMAND_ENV}) {
            foreach (keys %{$self->{COMMAND_ENV}}) {
                $ENV{$_} = $self->{COMMAND_ENV}->{$_};
            }
        }
        chdir($self->{COMMAND_PATH});
        exec($self->{COMMAND}, @args) || exit(1);
    }

    $self->parse_filehandle(*SCANNER);

    # this calls waitpid on the child process
    close(SCANNER);
    if ($? == -1) {
        carp $self->{COMMAND}, ": $?";
        return undef;
    }

    # interpret the exit status
    if ($self->exitcheck($?) != 0) {
        # TODO invalidate results
        carp ref($self), " scanner returned an error\n";
        return undef;
    }

    return 1;
}

=item $audit->B<run_all>($file)

Processes $file with all available vulnerability scanners and merges
the warnings into the Audit::Source object. Returns 1 on success or 
undef if any of the scanners produced an error.

  if (!$audit->run_all($file)) {
    die "error\n";
  }

=cut

sub run_all {
    my $self = shift;
    my $file = shift;
    my $usable = 0;
    my %outcome;
    my $scanner;

    unless (exists $self->{SCANNERS}) {
        foreach ("Splint", "Flawfinder", "Rats", "Pscan") {
            eval {
                no strict 'refs';
                push(@{$self->{SCANNERS}},
                  ("Audit::Source::" . $_)->new());
            }
        }
    }

    foreach $scanner (@{$self->{SCANNERS}}) {
        $scanner->verbose($self->verbose());
        $scanner->includepaths($self->includepaths());
        print "using scanner ", ref($scanner), "\n" if $self->{VERBOSE} == 1;
        $usable++;
    }

    croak "no scanners available" if $usable < 1;

    foreach $scanner (@{$self->{SCANNERS}}) {
        $scanner->run($file);
        # copy the full path determined by the scanner
        $self->{COMMAND_PATH} = $scanner->{COMMAND_PATH};
    }

    $self->{FILES} = $self->match(@{$self->{SCANNERS}});
    return $self->{FILES};
}

=item $audit->B<merge>($scanner)

Merges the warnings from any number of scanners or Audit::Source objects
into the Audit::Source object. Returns 1 on success and undef in case of
errors.  

  $audit->merge($split, $oldaudit);
  ..
  $audit->textdump();

=cut

sub merge {
    my $self = shift;
    my @others = @_;

    foreach (@others) {
        if (ref !~ /^Audit::Source/) {
           croak "not an Audit::Source object";
       }
    }

    $self->{FILES} = $self->match(@others);
    return 1;
}

=item $audit->verbose(1);

Enables verbose warnings of the internal processing.

=cut

sub verbose {
    my $self = shift;
    my $val = shift;
    if (defined $val) {
        $self->{VERBOSE} = $val;
    }
    return $self->{VERBOSE};
}

sub _absolutepath {
    my $self = shift;
    my $file = shift;

    croak if !exists $self->{COMMAND_PATH};

    if ($file !~ /^\//) {
        $file =~ s|.*/||;
        $file = join("/", $self->{COMMAND_PATH}, $file);
        $file =~ s|/{2,}|/|g;
    }

    return $file;
}

=item $audit->B<match>($scanner1, ...)

Matches the warnings from any number of scanners or Audit::Source objects.
Returnes a hash reference of matching entries in the format described in
B<AUDIT-HASH> below.

  $match = $audit->match($rats, $flawfinder);

  foreach $file (keys %$match) {
    foreach $line (keys %{$match->{$file}}) {
      ..

=cut

sub match {
    my $self = shift;
    my @others = @_;
    my %outcome;

    foreach (@others) {
        if (ref !~ /^Audit::Source/) {
            croak "not an Audit::Source object";
        }
    }

    # merge the results of each scan
    foreach my $scanner ($self, @others) {
        my $data = $scanner->files_hashref;

        foreach my $file (keys %$data) {
            foreach my $line (keys %{$data->{$file}}) {

                # add entries
                push(@{$outcome{$file}->{$line}->{'data'}},
                  @{$data->{$file}->{$line}->{'data'}});
                # increase number of matches
                $outcome{$file}->{$line}->{'matchnum'}++;
            }
        }
    }

    return \%outcome;
}

=item $audit->B<textdump>()

Prints a text report of all warnings in the Audit::Source object. Returns a
string if used in scalar context.

=cut 

sub textdump {
    my $self = shift;
    my $files = shift || $self->{FILES};
    my $text;

    # loop trough them and display
    foreach my $filename (keys %$files) {
        $text .= "file $filename\n";

        foreach my $line (sort {$a <=> $b} keys %{$files->{$filename}}) {
            my $entries = $files->{$filename}->{$line};
            $text .= sprintf "  line %d: matched %d times\n", $line,
              $entries->{'matchnum'};
        
            foreach my $e (@{$entries->{'data'}}) {
                $text .= sprintf "  line %d: %s: severity %s\n", $line,
                  $e->{'source'},
                  $e->{'severity'};
                $text .= sprintf "  line %d: %s: description:\n%s\n\n", 
                  $line, $e->{'source'}, _indent($e->{'desc'});
            }
        }
    }

    # scalar context?
    if (defined wantarray() && !wantarray()) {
        return $text;
    }

    print $text;
    return 1;
}

sub _indent {
    my $str = shift || '';
    $Text::Wrap::columns = 60;
    return wrap("    ", "    ", split(/\s*\n\s*/, $str));
}

=item $audit->B<files>

Returns all filenames for which there are warnings in an array.

  @filenames = $audit->files;

=cut

sub files {
    my $self = shift;
    my @k = keys %{$self->{FILES}};
    return @k;
}

=item $audit->B<file>($file)

Returns a reference to the %lines hash for $file.

  $auditfile = $audit->file($file);
  %lines = %{$auditfile{$line}}

=cut

sub file {
    my $self = shift;
    my $f = shift;
    my $fileref = $self->{FILES};
 
    $f =~ s|.*/||;
 
    croak "no file" unless defined $f;
    unless (exists $fileref->{$self->_absolutepath($f)}) {
       # no entries recorded
       return { };
    }

    return \%{$fileref->{$self->_absolutepath($f)}};
}

=item $audit->B<files_hashref>

Returns a hash reference to the internal Audit-Hash (see B<AUDIT-HASH>).

=cut

sub files_hashref {
    my $self = shift;
    return $self->{FILES};
}


=item $audit->B<lines>($file)

Returns all lines in $file for which there are warnings in an array.

  @lines = $audit->lines;
  foreach (@lines) {
    print "line $_ matched one or more scanners\n";
  }

=cut

sub lines {
    my $self = shift;
    my $file = shift;
    my @k = sort { $a <=> $b } keys %{$self->{FILES}->{$file}};
    return @k;
}

=item $audit->B<lines_hashref>($file)

Returns a hash reference of lines and their warnings.

=cut

sub lines_hashref {
    my $self = shift;
    my $file = shift;
    return $self->{FILES}->{$file};
}

=item $audit->B<entries_matchnum>($file, $line)

Returns the number of distinct scanners that reported warnings for this
$file and $line.

  $num = $audit->entries_matchnum($file, $line);
  print "matched by $num different scanners\n";

=cut

sub entries_matchnum {
    my $self = shift;
    my $file = shift;
    my $line = shift;
    return $self->{FILES}->{$file}->{$line}->{'matchnum'};
}

=item $audit->B<entries>($file, $line)

Returns an array of entries in the B<ENTRY-HASH> format.

  @entries = $audit->entries($file, $line);
  print "reported by ", $entries[0]->{'source'}, "\n";

=cut

sub entries {
    my $self = shift;
    my $file = shift;
    my $line = shift;
    return @{$self->{FILES}->{$file}->{$line}->{'data'}};
}

=item $audit->B<entries_hashref>($file, $line)

Returns a hash reference of entries for $file and $line.

=cut

sub entries_hashref {
    my $self = shift;
    my $file = shift;
    my $line = shift;
    return $self->{FILES}->{$file}->{$line};
}

sub _activate {
    my $self = shift;
    my $path;
    my $cmd;

    $self->{ACTIVE} = 0;

    if (ref($self) eq 'Audit::Source') {
        # silently ignore
        return 1;
    }

    # test if command is available
    foreach $path (split(':', $ENV{'PATH'})) {
        next if $path eq '.';
        $cmd = join('/', $path, $self->{COMMAND});
        if (-x $cmd) {
            $self->{ACTIVE} = 1;
            $self->{COMMAND} = $cmd;
            return 1;
        }
    }

    # it isn't
    croak ref($self), " scanner not available";
}

=back

=head1 AUDIT-HASH

The Audit-Hash is used internally to store the information parsed by the
scanner modules. You can access it's content through the files() , lines() ,
lines_hashref() , entries_matchnum() , entries() and entries_hashref()
methods.

  ..
  @entries = $audit->entries($file, $line);
  foreach (@entries) {
    # do something with the entry
  }

This should normally be all you need to know about the Audit-Hash. You
can safely skip to B<ENTRY-HASH> below unless you plan to use match() or
fiddle with the internals of Audit::Source (not recommended).

In case you want to access the Audit-Hash directly, you can call
files_hashref() on any Audit::Source object to get a reference to it.
A reference of the same format is also returned my match().

You can use an Audit-Hash like this:

  my $audithashref = $audit->files_hashref;
  my %files = %{$audithashref}

The keys of %files elements are filenames that have warnings recorded. Each
element contains a reference to the %lines hash for the filename. 

  my %lines = %{$files{$file}}

The %lines keys are line numbers. Each element has a reference to the
%entries hash for the line.

  my %entries = %{$lines{$line}}

The %entries hash has two fields of interest. $entries{'matchnum'} 
contains the number of different scanners for which this line caused
warnings. Then, there is a reference to an array of Entry-Hashes
in $entries{'data'}.

=head1 ENTRY-HASH

The Entry-Hash contains exactly one scanner warning. An array reference of
all entries for a given file and line is returned by entries() .

=over 4

=item $entry{'B<source>'}

Name of the originating scanner module.

=item $entry{'B<severity>'}

Severity of the warning. The format of this element currently varies between
the scanner modules.

=item $entry{'B<desc>'}

Full text of the scanner warnings that describes the potential vulnerability.

=item $entry{'B<file>'}

Absolute filename of the scanned file.

=item $entry{'B<line>'}

Line number of the entry.

=back

=cut

=head1 EXAMPLES

 #!/usr/bin/perl -w

 use strict;
 use Audit::Source;

 my $audit = new Audit::Source;
 if (!$audit->run_all($ARGV[0])) {
   die "scanning error\n";
 }

 my @lines = $audit->lines($file);
 print "these lines produced warnings: ", join(", ", @lines), "\n\n";

 my @entries = $audit->entries($file, $lines[0]);
 my $entry = shift @entries;

 print "the first entry is:\n";
 print "     line: ", $lines[0], "\n";
 print "   source: ", $entry->{'source'}, "\n";
 print " severity: ", $entry->{'severity'}, "\n";
 print "  warning: ", $entry->{'desc'}, "\n\n";

=head1 SEE ALSO

=over 

=item L<Audit::Source::Rats(3pm)>

=item L<Audit::Source::Flawfinder(3pm)>

=item L<Audit::Source::Pscan(3pm)>

=item L<Audit::Source::Splint(3pm)>

=item The files one.pl and two.pl in examples/

=back

=cut

=head1 AUTHOR

Max Vozeler <max@hinterhof.net>

=cut

1;


DOXYGEN = /usr/bin/doxygen
PERL = /usr/bin/perl
export PERL

TOOLSDIR = $(CURDIR)/../tools/
CAINCLUDES = /usr/local/include/
export TOOLSDIR
export CAINCLUDES

DOCDIR = $(CURDIR)/docs/
STYLEDOCDIR = $(DOCDIR)/style/
export STYLEDOCDIR

COMPILE.c = $(CC) $(CFLAGS) -Wall $(INCLUDES) $(TARGET_ARCH) -c
LINK.c = $(CC) $(CFLAGS) $(LDFLAGS) $(TARGET_ARCH)
CC = gcc
export COMPILE.c
export LINK.c

INDENT_PROFILE = $(CURDIR)/indent.pro
export INDENT_PROFILE

default: compile

all: indent compile style doxygen #check

style:
	$(MAKE) -C $(TOOLSDIR)colour-audit -f Makefile.gen
	mkdir -p $(STYLEDOCDIR)
	$(MAKE) -C util style
	$(MAKE) -C sipstack style
	$(MAKE) -C xmlrpc-test style
	$(MAKE) -C core style

doxygen:
	mkdir -p $(DOCDIR)
	$(DOXYGEN)

#check:
#	find ./ | grep check_ | grep -v docs/ | xargs bash -c

compile:
	$(MAKE) -C util
	$(MAKE) -C sipstack
	$(MAKE) -C xmlrpc-test
	$(MAKE) -C core

clean:
	-rm $(DOCDIR) -r
	$(MAKE) -C util clean
	$(MAKE) -C sipstack clean
	$(MAKE) -C xmlrpc-test clean
	$(MAKE) -C core clean

indent:
	$(MAKE) -C util indent
	$(MAKE) -C sipstack indent
	$(MAKE) -C xmlrpc-test indent
	$(MAKE) -C core indent

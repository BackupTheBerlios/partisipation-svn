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

all: compile style doxygen #check

style:
	$(MAKE) -C $(TOOLSDIR)colour-audit -f Makefile.gen
	mkdir -p $(STYLEDOCDIR)
	$(MAKE) -C sipstack style
	$(MAKE) -C util style
	$(MAKE) -C xmlrpc-test style
	$(MAKE) -C core style

doxygen:
	mkdir -p $(STYLEDOCDIR)
	$(DOXYGEN)

#check:
#	find ./ | grep check_ | grep -v docs/ | xargs bash -c

compile:
	echo $(COMPILE.c)
	$(MAKE) -C sipstack
	$(MAKE) -C util
	$(MAKE) -C xmlrpc-test
	$(MAKE) -C core

clean:
	-rm $(DOCDIR) -r
	$(MAKE) -C sipstack clean
	$(MAKE) -C util clean
	$(MAKE) -C xmlrpc-test clean
	$(MAKE) -C core clean

COMPILER = /usr/bin/gcc
DOXYGEN = /usr/bin/doxygen
PERL = /usr/bin/perl

TOOLSDIR = ../tools/
XMLRPCLIB = /usr/local/include/

SOURCEFILES = xmlrpc-test/*.c sipstack/*.c

DOCDIR = ./docs/

STYLEDOCDIR = $(DOCDIR)style/

compile:
	$(MAKE) -C sipstack
	$(MAKE) -C util
	$(MAKE) -C xmlrpc-test
	$(MAKE) -C core

all: compile style doxygen check

style:
	$(MAKE) -C $(TOOLSDIR)colour-audit -f Makefile.gen
	mkdir -p $(STYLEDOCDIR)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(XMLRPCLIB) -d $(STYLEDOCDIR) $(SOURCEFILES)

doxygen:
	$(DOXYGEN)

check:
	find ./ | grep check_ | grep -v docs/ | xargs bash -c

compile:
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

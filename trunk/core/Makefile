COMPILER = /usr/bin/gcc
DOXYGEN = /usr/bin/doxygen
PERL = /usr/bin/perl

TOOLSDIR = ../tools/
XMLRPCLIB = /usr/local/include/

SOURCEFILES = xmlrpc-test/*.c sipstack/*.c

DOCDIR = ./docs/

STYLEDOCDIR = $(DOCDIR)style/


all: compile style doxygen

style:
	$(MAKE) -C $(TOOLSDIR)colour-audit
	mkdir -p $(STYLEDOCDIR)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(XMLRPCLIB) -d $(STYLEDOCDIR) $(SOURCEFILES)

doxygen:
	$(DOXYGEN)

compile:
	$(MAKE) -C sipstack
	$(MAKE) -C xmlrpc-test

clean:
	-rm $(DOCDIR) -r
	$(MAKE) -C sipstack clean
	$(MAKE) -C xmlrpc-test clean

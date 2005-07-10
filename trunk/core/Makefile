COMPILER = /usr/bin/gcc
DOXYGEN = /usr/bin/doxygen
PERL = perl

TOOLSDIR = ../tools/
XMLRPCLIB = /usr/local/include/

SOURCEFILES = xmlrpc-test/*.c sipstack/*.c

DOCDIR = ./docs/

STYLEDOCDIR = $(DOCDIR)style/


all: style doxygen

style:
	mkdir -p $(STYLEDOCDIR)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(XMLRPCLIB) -d $(STYLEDOCDIR) $(SOURCEFILES)

doxygen:
	$(DOXYGEN)

clean:
	-rm $(DOCDIR) -r
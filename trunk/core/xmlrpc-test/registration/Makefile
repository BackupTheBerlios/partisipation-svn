STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/registration

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I ../

	$(MAKE) -C xmlrpc

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	$(MAKE) -C xmlrpc clean

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C xmlrpc style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C xmlrpc indent

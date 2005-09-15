STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/util

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../
	$(MAKE) -C config
	$(MAKE) -C logging
	$(MAKE) -C queue
	$(MAKE) -C test

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	$(MAKE) -C config clean
	$(MAKE) -C logging clean
	$(MAKE) -C queue clean
	$(MAKE) -C test clean

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C config style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C logging style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C queue style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C test style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C config indent
	$(MAKE) -C logging indent
	$(MAKE) -C queue indent
	$(MAKE) -C test indent

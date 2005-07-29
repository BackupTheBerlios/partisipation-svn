STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/core

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I ./

	$(MAKE) -C queue
	$(MAKE) -C test

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	$(MAKE) -C queue clean
	$(MAKE) -C test clean

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C queue style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C test style

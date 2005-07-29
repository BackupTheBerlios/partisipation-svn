STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/sipstack

compile:
	find ./ | grep \\.c$$ | grep -v tests/ | xargs $(COMPILE.c) -I./
#	$(LINK.c) -o check_osip_adapter test_osip.o osip_adapter.o -L/usr/local/lib -lcheck -leXosip2

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f

check: compile

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

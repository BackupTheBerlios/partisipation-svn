STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/sipstack

compile:
	find ./ | grep \\.c$$ | grep -v tests/ | xargs $(COMPILE.c) -I./
	$(LINK.c) -o check_osip_adapter test_osip.o osip_adapter.o ../util/queue/queue.o ../util/queue/queue_datatype_event.o -L/usr/local/lib -lcheck -leXosip2 -losip2 -losipparser2

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f check_osip_adapter

check: compile

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent

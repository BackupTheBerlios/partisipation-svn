STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/sipstack

compile:
	find ./ | grep \\.c$$ | grep -v tests/ | xargs $(COMPILE.c) -I./
	$(LINK.c) -o check_sipstack_adapter test_sipstack.o osip_adapter.o osip_adapter_cm.o sip_listener.o ../util/thread_management.o ../util/logging/logger.o ../util/queue/queue.o -L/usr/local/lib -lcheck -leXosip2 -losip2 -losipparser2 -lpthread

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f check_*

check: compile

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent

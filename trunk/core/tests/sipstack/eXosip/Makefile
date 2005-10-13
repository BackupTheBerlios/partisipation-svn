XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/eXosip

compile:
	find ./ | grep \\.c$$ | grep -v tests/ | xargs $(COMPILE.c) -I../../../
	$(LINK.c) -o check_sipstack_adapter test_sipstack.o ../../../sipstack/eXosip/osip_adapter.o ../../../sipstack/eXosip/sip_listener.o ../../../util/threads/thread_management.o ../../../util/logging/simple/logger.o ../../../util/config/xml/config_reader.o ../../../util/queue/queue.o -L/usr/local/lib -lcheck -leXosip2 -losip2 -losipparser2 -lpthread $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f check_*

check: compile

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent

XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/test

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../../

	$(LINK.c) -o test_dispatcher test_dispatcher.o ../event_dispatcher.o ../statemachine.o ../../util/thread_management.o ../../util/logging/logger.o ../../util/config/config_reader.o ../../util/queue/queue.o ../call_id_generator.o ../../xmlrpc-test/callback/gui_callback.o ../../xmlrpc-test/callback/callback_util.o ../../sipstack/osip_adapter.o ../../sipstack/sip_listener.o ../../sipstack/osip_adapter_cm.o -lpthread $(LIBS_CLIENT) $(LIBS_ABYSS) -leXosip2 -losip2 -losipparser2
	$(LINK.c) -o test_cig test_cig.o ../call_id_generator.o ../../util/thread_management.o ../../util/logging/logger.o ../../util/config/config_reader.o -lcheck -lpthread $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_dispatcher
	rm -f test_cig

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent

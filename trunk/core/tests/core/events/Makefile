XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/events

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I../../../
	$(LINK.c) -o test_dispatcher test_dispatcher.o ../../../core/events/event_dispatcher.o ../../../core/events/statemachine.o ../../../util/threads/thread_management.o ../../../util/logging/simple/logger.o ../../../util/list/list.o ../../../util/config/xml/config_reader.o ../../../util/queue/queue.o ../../../core/callIDs/call_id_generator.o ../../../remote/callback/xmlrpc/gui_callback.o ../../../remote/callback/xmlrpc/xmlrpc_callback_client.o ../../../sipstack/eXosip/osip_adapter.o ../../../core/sip_input/sip_listener.o ../../../core/gui_output/gui_callback_sender.o ../../../core/sip_output/registrar_manager.o ../../../accounts/account_management.o ../../../util/config/xml/config_writer.o -L../../../lib/genx -lpthread -lgenx $(LIBS_CLIENT) $(LIBS_ABYSS) -leXosip2 -losip2 -losipparser2

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f test_dispatcher

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent

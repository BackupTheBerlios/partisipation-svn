DOXYGEN = /usr/bin/doxygen
PERL = /usr/bin/perl
export PERL

TOOLSDIR = $(CURDIR)/../tools/
CAINCLUDES = /usr/local/include/
export TOOLSDIR
export CAINCLUDES

DOCDIR = $(CURDIR)/doc/
DOXYGENDOCDIR = $(DOCDIR)/api/
STYLEDOCDIR = $(DOCDIR)/style/
export STYLEDOCDIR

COMPILE.c = $(CC) $(CFLAGS) -Wall $(INCLUDES) $(TARGET_ARCH) -c
LINK.c = $(CC) $(CFLAGS) $(LDFLAGS) $(TARGET_ARCH)
CC = gcc
export COMPILE.c
export LINK.c

INDENT_PROFILE = $(CURDIR)/indent.pro
export INDENT_PROFILE

XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)

default: compile

all: compile style doxygen #check

style:
	$(MAKE) -C $(TOOLSDIR)colour-audit -f Makefile.gen
	mkdir -p $(STYLEDOCDIR)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR) *.c
	$(MAKE) -C util style
	$(MAKE) -C sipstack style
	$(MAKE) -C remote style
	$(MAKE) -C accounts style
	$(MAKE) -C core style
	$(MAKE) -C tests style

docu: doxygen

doxygen:
	mkdir -p $(DOCDIR)
	$(DOXYGEN) doc/Doxyfile	

#check:
#	find ./ | grep check_ | grep -v docs/ | xargs bash -c

compile: indent
	$(MAKE) -C util
	$(MAKE) -C sipstack
	$(MAKE) -C remote
	$(MAKE) -C accounts
	$(MAKE) -C core
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I./
	$(LINK.c) -o core_launcher core_launcher.o util/config/xml/config_reader.o util/config/xml/config_writer.o util/logging/simple/logger.o util/threads/thread_management.o util/queue/queue.o accounts/account_management.o util/list/list.o core/callIDs/call_id_generator.o core/events/event_dispatcher.o core/events/statemachine.o core/events/call_status.o core/events/transaction_info.o sipstack/eXosip/osip_adapter.o core/sip_input/sip_listener.o remote/callback/xmlrpc/xmlrpc_callback_client.o remote/callback/xmlrpc/gui_callback.o remote/server/xmlrpc/xmlrpc_server.o remote/server/xmlrpc/accounts_remote.o remote/server/xmlrpc/calls_remote.o remote/server/xmlrpc/extras_remote.o remote/server/xmlrpc/registration_remote.o remote/server/xmlrpc/volume_remote.o remote/server/accounts.o remote/server/calls.o remote/server/extras.o remote/server/registration.o remote/server/volume.o core/gui_input/accounts_receiver.o core/gui_input/calls_receiver.o core/gui_input/extras_receiver.o core/gui_input/registration_receiver.o core/gui_input/volume_receiver.o core/gui_output/gui_callback_sender.o core/sip_output/registrar_manager.o -Llib/genx -lpthread -lgenx -leXosip2 -losip2 -losipparser2 $(LIBS_CLIENT) $(LIBS_ABYSS)
	$(MAKE) -C tests

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -rf $(DOXYGENDOCDIR)
	rm -rf $(STYLEDOCDIR)
	rm -f core_launcher
	$(MAKE) -C util clean
	$(MAKE) -C sipstack clean
	$(MAKE) -C remote clean
	$(MAKE) -C accounts clean
	$(MAKE) -C core clean
	$(MAKE) -C tests clean

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C util indent
	$(MAKE) -C sipstack indent
	$(MAKE) -C remote indent
	$(MAKE) -C accounts indent
	$(MAKE) -C core indent
	$(MAKE) -C tests clean

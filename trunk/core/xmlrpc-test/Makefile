XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
STYLEDOCDIR_LOCAL = $(STYLEDOCDIR)/xmlrpc-test

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs $(COMPILE.c) -I ./

	$(MAKE) -C accounts
	$(MAKE) -C callback
	$(MAKE) -C calls
	$(MAKE) -C extras
	$(MAKE) -C registration
	$(MAKE) -C volume

	$(LINK.c) -o xmlrpc_server xmlrpc_server.o registration/registration_management.o registration/xmlrpc/dispatcher.o calls/call_management.o calls/xmlrpc/dispatcher.o accounts/account_management.o accounts/xmlrpc/dispatcher.o volume/volume_management.o volume/xmlrpc/dispatcher.o extras/extras_management.o extras/xmlrpc/dispatcher.o callback/callback_util.o callback/gui_callback.o $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	find ./ -maxdepth 1 | grep \\.o$$ | xargs rm -f
	rm -f xmlrpc_server
	$(MAKE) -C accounts clean
	$(MAKE) -C callback clean
	$(MAKE) -C calls clean
	$(MAKE) -C extras clean
	$(MAKE) -C registration clean
	$(MAKE) -C volume clean

style:
	mkdir -p $(STYLEDOCDIR_LOCAL)
	$(PERL) $(TOOLSDIR)colour-audit/colour-audit -i $(CAINCLUDES) -d $(STYLEDOCDIR_LOCAL) *.c
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C accounts style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C callback style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C calls style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C extras style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C registration style
	$(MAKE) STYLEDOCDIR=$(STYLEDOCDIR_LOCAL) -C volume style

indent:
	find ./ -maxdepth 1 | grep \\.[ch]$$ | grep -v docs/ | xargs indent
	$(MAKE) -C accounts indent
	$(MAKE) -C callback indent
	$(MAKE) -C calls indent
	$(MAKE) -C extras indent
	$(MAKE) -C registration indent
	$(MAKE) -C volume indent

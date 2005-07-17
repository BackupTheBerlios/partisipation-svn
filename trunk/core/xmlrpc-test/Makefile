XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)

compile:
	find ./ -maxdepth 1 | grep \\.c$$ | grep -v docs/ | xargs gcc -c -I ./

	$(MAKE) -C accounts
	$(MAKE) -C callback
	$(MAKE) -C calls
	$(MAKE) -C extras
	$(MAKE) -C registration
	$(MAKE) -C threads
	$(MAKE) -C volume

	gcc -o xmlrpc_server xmlrpc_server.o registration/registration_management.o registration/xmlrpc/dispatcher.o calls/call_management.o calls/xmlrpc/dispatcher.o accounts/account_management.o accounts/xmlrpc/dispatcher.o volume/volume_management.o volume/xmlrpc/dispatcher.o extras/extras_management.o extras/xmlrpc/dispatcher.o callback/callback_util.o callback/gui_callback.o $(LIBS_CLIENT) $(LIBS_ABYSS)

clean:
	rm -f *.o */*.o */*/*.o
	rm -f xmlrpc_server

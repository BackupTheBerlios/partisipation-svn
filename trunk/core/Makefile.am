XMLRPC_C_CONFIG = /usr/local/bin/xmlrpc-c-config
LIBS_CLIENT = $(shell $(XMLRPC_C_CONFIG) client --ldadd)
LIBS_ABYSS = $(shell $(XMLRPC_C_CONFIG) abyss-server --ldadd)
CONFIG_DIR = @USERHOME@/.partiSIPation/

bin_PROGRAMS = core_launcher
core_launcher_SOURCES = core_launcher.c util/config/xml/config_reader.c util/config/xml/config_writer.c util/logging/simple/logger.c util/threads/thread_management.c util/queue/queue.c accounts/account_management.c util/list/list.c core/callIDs/call_id_generator.c core/events/event_dispatcher.c core/events/statemachine.c core/events/call_status.c core/events/transaction_info.c sipstack/eXosip/osip_adapter.c core/sip_input/sip_listener.c remote/callback/xmlrpc/xmlrpc_callback_client.c remote/callback/xmlrpc/gui_callback.c remote/server/xmlrpc/xmlrpc_server.c remote/server/xmlrpc/accounts_remote.c remote/server/xmlrpc/calls_remote.c remote/server/xmlrpc/extras_remote.c remote/server/xmlrpc/registration_remote.c remote/server/xmlrpc/volume_remote.c remote/server/accounts.c remote/server/calls.c remote/server/extras.c remote/server/registration.c remote/server/volume.c core/gui_input/accounts_receiver.c core/gui_input/calls_receiver.c core/gui_input/extras_receiver.c core/gui_input/registration_receiver.c core/gui_input/volume_receiver.c core/gui_output/gui_callback_sender.c core/sip_output/registrar_manager.c util/config/xml/config_reader.h util/config/xml/config_writer.h  util/threads/thread_management.h util/queue/queue.h accounts/account_client_interface.h accounts/account_core_interface.h util/list/list.h core/callIDs/call_id_generator.h core/events/event_dispatcher.h core/events/statemachine.h core/events/call_status.h core/events/transaction_info.h sipstack/sip_stack_interface.h sipstack/sip_listener_interface.h remote/callback/xmlrpc/xmlrpc_callback_client.h remote/server/xmlrpc/xmlrpc_server.h remote/server/xmlrpc/accounts_remote.h remote/server/xmlrpc/calls_remote.h remote/server/xmlrpc/extras_remote.h remote/server/xmlrpc/registration_remote.h remote/server/xmlrpc/volume_remote.h remote/server/accounts.h remote/server/calls.h remote/server/extras.h remote/server/registration.h remote/server/volume.h core/gui_input/accounts_receiver.h core/gui_input/calls_receiver.h core/gui_input/extras_receiver.h core/gui_input/registration_receiver.h core/gui_input/volume_receiver.h util/logging/logger.h util/config/globals.h util/threads/tm_storage.h remote/callback/gui_callback.h

core_launcher_LDADD = -lpthread -lgenx -leXosip2 -losip2 -losipparser2

core_launcher_LDFLAGS = -Llib/genx $(LIBS_CLIENT) $(LIBS_ABYSS)

EXTRA_DIST = README INSTALL COPYING ChangeLog

SUBDIRS = accounts config core doc lib remote rtp sipstack util 

install-data-local:
	$(mkinstalldirs) $(CONFIG_DIR)
	$(INSTALL_DATA) config/xml/core_config.dtd $(CONFIG_DIR)core_config.dtd
	$(INSTALL_DATA) config/xml/core_config.xml $(CONFIG_DIR)core_config.xml

install-data-hook:
	chown @LOCALUSER@:users $(CONFIG_DIR)core_config.dtd
	chown @LOCALUSER@:users $(CONFIG_DIR)core_config.xml

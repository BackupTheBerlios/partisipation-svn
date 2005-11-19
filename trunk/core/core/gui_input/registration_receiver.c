#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <util/threads/thread_management.h>
#include <core/gui_input/registration_receiver.h>
#include <core/gui_output/gui_callback_sender.h>
#include <util/logging/logger.h>
#include <util/config/globals.h>
#include <core/gui_input/polling_gui.h>
#include <core/sip_output/registrar_manager.h>
#include <core/events/event_dispatcher.h>

#define REG_RCVR_MSG_PREFIX "[registration receiver] "

extern int shutdownPolling;

char *gi_register_gui(char *const address, int const port) {
	int rc;

	LOG_DEBUG(REG_RCVR_MSG_PREFIX "register_gui(address: %s port: %d)",
			  address, port);

	if (config.remote.callback.guiCallback.guiURL) {
		free(config.remote.callback.guiCallback.guiURL);
	}

	config.remote.callback.guiCallback.guiURL =
		(char *) malloc(1024 * sizeof(char));
	sprintf(config.remote.callback.guiCallback.guiURL, "http://%s:%d/RPC2",
			address, port);

	LOG_DEBUG(REG_RCVR_MSG_PREFIX "callback URL is now %s",
			  config.remote.callback.guiCallback.guiURL);

	rc = go_register_core();

	if (!rc) {
		LOG_ERROR(REG_RCVR_MSG_PREFIX "failed to register GUI "
				  "(address: %s port: %d)", address, port);

		free(config.remote.callback.guiCallback.guiURL);
		// explicitly setting to NULL so callback functions can savely check 
		// this 
		config.remote.callback.guiCallback.guiURL = NULL;
		return "ERROR";
	}
	// starts polling_thread for GUI Polling
	rc = start_thread(polling_gui, NULL);

	if (!rc) {
		LOG_ERROR(REG_RCVR_MSG_PREFIX "register_gui: failed to start "
				  "polling_gui thread");
		return "ERROR";
	}

	LOG_DEBUG(REG_RCVR_MSG_PREFIX "register_gui result=OK");
	return "OK";
}

int gi_unregister_gui(char *const address, int const port) {
	char *url;
	int rc;

	LOG_DEBUG(REG_RCVR_MSG_PREFIX "unregister_gui(address: %s port: %d)",
			  address, port);

	// shutdown thread for GUI Polling
	shutdownPolling = 1;

	url = (char *) malloc(1024 * sizeof(char));
	sprintf(url, "http://%s:%d/RPC2", address, port);

	if (!config.remote.callback.guiCallback.guiURL ||
		strcmp(config.remote.callback.guiCallback.guiURL, url) != 0) {

		LOG_ERROR(REG_RCVR_MSG_PREFIX "trying to unregister GUI without "
				  "previous registration (url: %s)", url);
		free(url);
		return 0;
	}

	free(config.remote.callback.guiCallback.guiURL);
	free(url);

	// explicitly setting to NULL so callback functions can savely check this 
	config.remote.callback.guiCallback.guiURL = NULL;

	// shutdown all statemachines immediately:
	rc = ed_shutdown_all();
	if (!rc) {
		LOG_ERROR(REG_RCVR_MSG_PREFIX "stopping of all calls failed");
		return 0;
	}
	// unregister all accounts with the SIP registrars:
	rc = rm_unregister_all();
	if (!rc) {
		LOG_ERROR(REG_RCVR_MSG_PREFIX "unregistering of all accounts "
				  "failed");
		return 0;
	}

	LOG_DEBUG(REG_RCVR_MSG_PREFIX "leaving unregister_gui()");
	return 1;
}

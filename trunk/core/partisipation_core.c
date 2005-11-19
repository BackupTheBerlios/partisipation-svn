#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <util/config/xml/config_reader.h>
#include <util/config/xml/config_writer.h>
#include <util/logging/logger.h>
#include <util/threads/thread_management.h>
#include <accounts/account_core_interface.h>
#include <core/callIDs/call_id_generator.h>
#include <core/events/event_dispatcher.h>
#include <core/sip_output/registrar_manager.h>
#include <core/gui_output/gui_callback_sender.h>
#include <sipstack/sip_stack_interface.h>
#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <remote/server/xmlrpc/xmlrpc_server.h>

#define CONFIG_FILE_PART "/.partiSIPation/core_config.xml"

char *configFile;

int init_config_reader() {
	int rc;
	rc = cr_init(configFile);
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_config_writer() {
	int rc;
	rc = cw_init(configFile, 0);
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_logger() {
	int rc;
	rc = logger_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_thread_management() {
	int rc;
	rc = tm_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_account_management() {
	am_init();
	return 1;
}

int init_call_id_generator() {
	int rc;
	rc = cig_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_event_dispatcher() {
	int rc;
	rc = ed_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_registrar_manager() {
	int rc;
	rc = rm_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_sipstack() {
	int rc;
	rc = sipstack_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_gui_callback() {
	int rc;
	rc = cb_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	// init callback sender
	rc = go_init();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_xmlrpc_server() {
	int rc;
	rc = start_xmlrpc_server_thread();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_utils() {
	int rc;
	rc = init_config_reader();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_config_writer();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_logger();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_thread_management();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_core() {
	int rc;
	rc = init_call_id_generator();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_event_dispatcher();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_registrar_manager();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_remote() {
	int rc;
	rc = init_gui_callback();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = init_xmlrpc_server();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_config_reader() {
	int rc;
	rc = cr_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_config_writer() {
	int rc;
	rc = cw_destroy(0);
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_logger() {
	int rc;
	rc = logger_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_thread_management() {
	int rc;
	rc = tm_destroy(0);
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_account_management() {
	return 1;
}

int destroy_call_id_generator() {
	int rc;
	rc = cig_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_event_dispatcher() {
	int rc;
	rc = ed_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_registrar_manager() {
	int rc;
	rc = rm_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_sipstack() {
	sipstack_quit();
	return 1;
}

int destroy_gui_callback() {
	int rc;
	rc = cb_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = go_destroy();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_xmlrpc_server() {
	return 1;
}

int destroy_utils() {
	int rc;
	rc = destroy_thread_management();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_logger();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_config_writer();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_config_reader();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_core() {
	int rc;
	rc = destroy_registrar_manager();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_event_dispatcher();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_call_id_generator();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_remote() {
	int rc;
	rc = destroy_xmlrpc_server();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	rc = destroy_gui_callback();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int main(int const argc, const char **const argv) {
	int rc;
	char *home;

	home = getenv("HOME");
	configFile =
		(char *) malloc((strlen(home) + strlen(CONFIG_FILE_PART)) *
						sizeof(char) + 1);
	sprintf(configFile, "%s%s", home, CONFIG_FILE_PART);

	rc = init_utils();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = init_account_management();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = init_core();
	if (rc == 0) {
		// ERROR
		return -1;
	}

	rc = init_sipstack();
	if (rc == 0) {
		// ERROR
		return -1;
	}

	rc = init_remote();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	// wait until threads are actually started (prevent rare bug):
	sleep(3);

	// wait until main threads are finished:
	rc = tm_join_active_threads();
	if (rc == 0) {
		// ERROR
		return -1;
	}

	rc = destroy_remote();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = destroy_sipstack();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = destroy_core();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = destroy_account_management();
	if (rc == 0) {
		// ERROR
		return -1;
	}
	rc = destroy_utils();
	if (rc == 0) {
		// ERROR
		return -1;
	}

	free(configFile);
	return 0;
}

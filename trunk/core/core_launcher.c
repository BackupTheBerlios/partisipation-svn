#include <stdlib.h>
#include <stdio.h>

#include <util/config/xml/config_reader.h>
#include <util/logging/logger.h>
#include <util/threads/thread_management.h>
#include <accounts/account_core_interface.h>
#include <core/callIDs/call_id_generator.h>
#include <core/events/event_dispatcher.h>
#include <sipstack/sip_stack_interface.h>
#include <remote/callback/xmlrpc/xmlrpc_callback_client.h>
#include <remote/server/xmlrpc/xmlrpc_server.h>

int init_config_reader() {
	int rc;
	rc = cr_init("./config/xml/core_config.xml");
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int init_config_writer() {
	int rc;
	/* 
	 * ATTENTION: this path must be the same as for config_reader. At the moment, it is different
	 * for the sake of safety.
	*/
	rc = cw_init("./config/xml/core_config_by_writer.xml", 0);
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

int init_sipstack() {
	int rc;
	rc = sipstack_init();
	if (rc != 0) {
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
	rc = destroy_config_reader();
	if (rc == 0) {
		// ERROR
		return 0;
	}
	return 1;
}

int destroy_core() {
	int rc;
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
	return 0;
}

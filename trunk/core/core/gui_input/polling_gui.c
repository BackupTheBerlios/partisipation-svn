#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <core/gui_input/polling_gui.h>
#include <core/gui_input/registration_receiver.h>
#include <core/gui_output/gui_callback_sender.h>

void *polling_gui(void *args) {

	int rc;
	void **params;

	int port;
	char *address;
	params = (void **) args;
	port = (int) params[1];
	address = (char *) params[0];

	while (!NULL) {

		sleep(60);
		rc = go_register_core();

		if (!rc) {
			int gu;
			gu = gi_unregister_gui(address, port);
			free(address);
			free(params);
			thread_terminated();
			return NULL;
		}

	}
	return NULL;
}

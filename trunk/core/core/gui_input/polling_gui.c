#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <core/gui_input/polling_gui.h>
#include <core/gui_output/gui_callback_sender.h>
#include <util/threads/thread_management.h>

#define POLLING_TIME 60

int shutdownPolling;

void *polling_gui(void *args) {
	int rc;
	int ctr;

	shutdownPolling = 0;
	while (1) {
		ctr = 0;
		while (ctr < POLLING_TIME) {
			sleep(1);
			ctr++;
			if (shutdownPolling) {
				thread_terminated();
				return NULL;
			}
		}
		rc = go_register_core();

		if (!rc) {
			thread_terminated();
			return NULL;
		}
	}

	thread_terminated();
	return NULL;
}

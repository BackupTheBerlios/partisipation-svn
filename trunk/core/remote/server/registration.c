#include <stdlib.h>
#include <stdio.h>

#include <remote/server/registration.h>
#include <core/gui_input/registration_receiver.h>

char *register_gui(char *const address, int const port) {
	return gi_register_gui(address, port);
}

int unregister_gui(char *const address, int const port) {
	return gi_unregister_gui(address, port);
}

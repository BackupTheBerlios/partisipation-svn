#include <stdlib.h>
#include <stdio.h>

#include <core/gui_input/extras_receiver.h>

int gi_send_dtmf(char *const character, int const callId) {

	// do something

	printf("char: %s\n", character);
	printf("callId: %d\n", callId);

	return 1;
}

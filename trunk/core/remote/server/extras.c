#include <stdlib.h>
#include <stdio.h>

#include <remote/server/extras.h>
#include <core/gui_input/extras_receiver.h>

int send_dtmf(char *const character, int const callId) {
	return gi_send_dtmf(character, callId);
}

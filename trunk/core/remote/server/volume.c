#include <stdlib.h>
#include <stdio.h>

#include <remote/server/volume.h>
#include <core/gui_input/volume_receiver.h>

int set_speaker_volume(double const level) {
	return gi_set_speaker_volume(level);
}

double get_speaker_volume() {
	return gi_get_speaker_volume();
}

int set_micro_volume(double const level) {
	return gi_set_micro_volume(level);
}

double get_micro_volume() {
	return gi_get_micro_volume();
}

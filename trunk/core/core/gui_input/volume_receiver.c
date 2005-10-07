#include <stdlib.h>
#include <stdio.h>

#include <core/gui_input/volume_receiver.h>

int gi_set_speaker_volume(double const level) {

	// do something

	printf("level: %f\n", level);

	return 1;
}

double gi_get_speaker_volume() {

	// do something

	return 0.5;
}

int gi_set_micro_volume(double const level) {

	// do something

	printf("level: %f\n", level);

	return 1;
}

double gi_get_micro_volume() {

	// do something

	return 0.75;
}

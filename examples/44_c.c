#include "cmetro.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <bpm (int)>\n", argv[0]);
		exit(1);
	}

	int bpm = atoi(argv[1]);
	int ret;

	struct metronome *metro = metronome_create(bpm);

	struct note *downbeat = note_create_drum_downbeat_1();
	struct note *beat = note_create_drum_beat_1();

	struct measure *m1 = measure_create(4);

	if ((ret = measure_set_note(m1, 0, downbeat)) != 0) {
		fprintf(stderr, "error from libmetro");
		exit(1);
	}

	for (int i = 1; i < 4; ++i) {
		if ((ret = measure_set_note(m1, i, beat)) != 0) {
			fprintf(stderr, "error from libmetro");
			exit(1);
		}
	}

	if ((ret = metronome_add_measure(metro, m1)) != 0) {
	}
	metronome_start_and_loop(metro);

	metronome_destroy(metro);
	measure_destroy(m1);
	note_destroy(downbeat);
	note_destroy(beat);

	return 0;
}

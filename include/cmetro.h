#ifndef CMETRO_H
#define CMETRO_H

#ifdef __cplusplus
extern "C" {
#endif

// simplified libmetro api
// opinionated notes for drum downbeats + regular beats
struct note;

// return NULL if libmetro C++ library threw exception
// and prints exception to stderr
struct note *note_create_drum_downbeat_1();
struct note *note_create_drum_beat_1();
struct note *note_create_drum_downbeat_2();
struct note *note_create_drum_beat_2();

void note_destroy(struct note *n);

// measures
struct measure;

// returns NULL if libmetro C++ library threw exception
// and prints exception to stderr
struct measure *measure_create(int size);

int measure_set_note(struct measure *m, int index, struct note *n);

void measure_destroy(struct measure *m);

// metronome
struct metronome;

// returns NULL if libmetro C++ library threw exception
// and prints exception to stderr
struct metronome *metronome_create(int bpm);

int metronome_add_measure(struct metronome *metro, struct measure *measure);
int metronome_start_and_loop(struct metronome *metro);

void metronome_destroy(struct metronome *m);

#ifdef __cplusplus
}
#endif

#endif /* CMETRO_H */

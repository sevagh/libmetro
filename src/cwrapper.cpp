#include "cmetro.h"
#include "libmetro.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

#define DOWNBEAT_VOL 100.0
#define BEAT_VOL 50.0

#define DOWNBEAT_1 92.50
#define BEAT_1 73.42

// _1 and _2 have different timbre
// for potential polrhythm cases
#define DOWNBEAT_2 185.0
#define BEAT_2 207.65

struct note {
	void* obj;
};

struct measure {
	void* obj;
};

struct metronome {
	void* obj;
};

struct note* note_create_drum_downbeat_1()
{
	struct note* n;
	metro::Note* note;

	try {
		note = new metro::Note(
		    metro::Note(metro::Note::Timbre::Drum, DOWNBEAT_1, DOWNBEAT_VOL)
		    + metro::Note(metro::Note::Timbre::Drum, BEAT_1, DOWNBEAT_VOL));
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	n = ( struct note* )std::malloc(sizeof(struct note*));
	n->obj = note;
	return n;
}

struct note* note_create_drum_beat_1()
{
	struct note* n;
	metro::Note* note;

	try {
		note = new metro::Note(
		    metro::Note(metro::Note::Timbre::Drum, BEAT_1, BEAT_VOL));
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	n = ( struct note* )std::malloc(sizeof(struct note*));
	n->obj = note;
	return n;
}

struct note* note_create_drum_downbeat_2()
{
	struct note* n;
	metro::Note* note;

	try {
		note = new metro::Note(
		    metro::Note(metro::Note::Timbre::Drum, DOWNBEAT_2, DOWNBEAT_VOL)
		    + metro::Note(metro::Note::Timbre::Drum, BEAT_2, DOWNBEAT_VOL));
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	n = ( struct note* )std::malloc(sizeof(struct note*));
	n->obj = note;
	return n;
}

struct note* note_create_drum_beat_2()
{
	struct note* n;
	metro::Note* note;

	try {
		note = new metro::Note(
		    metro::Note(metro::Note::Timbre::Drum, BEAT_2, BEAT_VOL));
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	n = ( struct note* )std::malloc(sizeof(struct note*));
	n->obj = note;
	return n;
}

void note_destroy(struct note* n)
{
	if (n == NULL) {
		return;
	}
	delete static_cast<metro::Note*>(n->obj);
	free(n);
}

struct measure* measure_create(int size)
{
	struct measure* m;
	metro::Measure* measure;

	try {
		measure = new metro::Measure(size);
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	m = ( struct measure* )std::malloc(sizeof(struct measure*));
	m->obj = measure;
	return m;
}

int measure_set_note(struct measure* m, int index, struct note* n)
{
	try {
		auto m_ = static_cast<metro::Measure*>(m->obj);
		auto n_ = static_cast<metro::Note*>(n->obj);

		(*m_)[index] = (*n_);
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return -1;
	}
	return 0;
}

void measure_destroy(struct measure* m)
{
	if (m == NULL) {
		return;
	}
	delete static_cast<metro::Measure*>(m->obj);
	free(m);
}

struct metronome* metronome_create(int bpm)
{
	struct metronome* m;
	metro::Metronome* metro;

	try {
		metro = new metro::Metronome(bpm);
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return NULL;
	}

	m = ( struct metronome* )std::malloc(sizeof(struct metronome*));
	m->obj = metro;
	return m;
}

int metronome_add_measure(struct metronome* metro, struct measure* measure)
{
	try {
		auto metro_ = static_cast<metro::Metronome*>(metro->obj);
		auto measure_ = static_cast<metro::Measure*>(measure->obj);

		metro_->add_measure(*measure_);
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return -1;
	}
	return 0;
}

int metronome_start_and_loop(struct metronome* metro)
{
	try {
		auto metro_ = static_cast<metro::Metronome*>(metro->obj);
		metro_->start_and_loop();
	}
	catch (const std::exception& exc) {
		std::cerr << "exception: " << exc.what() << std::endl;
		return -1;
	}
	return 0;
}

void metronome_destroy(struct metronome* m)
{
	if (m == NULL) {
		return;
	}
	delete static_cast<metro::Metronome*>(m->obj);
	free(m);
}

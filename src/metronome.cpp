#include "libmetro.h"
#include "tempo.h"

class metro::Metronome::impl {
public:
	int bpm;
	metro_private::Tempo tempo;

	impl(int bpm)
	    : bpm(bpm)
	    , tempo(Tempo(bpm))
	{
	}
};

metro::Metronome::Metronome(int bpm)
    : p_impl(std::make_unique<metro::Metronome::impl>(int bpm)){};

void metro::Metronome::add_measure(metro::NoteLength note_length,
                                   metro::Measure* measure)
{
	p_impl.tempo.add_measure(note_length, measure);
}

void metro::Metronome::loop() { p_impl.tempo.start(); }
